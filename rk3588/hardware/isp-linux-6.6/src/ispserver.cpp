#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h> /* getopt_long() */
#include <fcntl.h> /* low-level i/o */
#include <inttypes.h>
#include <unistd.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <pthread.h>

#include <linux/videodev2.h>
#include "uAPI2/rk_aiq_user_api2_sysctl.h"
#include "mediactl/mediactl.h"

#define CLEAR(x) memset(&(x), 0, sizeof(x))
#define DBG(...) do { if (!g_silent) printf("DBG: " __VA_ARGS__);} while (0)
#define ERR(...) do { fprintf(stderr, "ERR: " __VA_ARGS__); } while (0)


/* Private v4l2 event */
#define CIFISP_V4L2_EVENT_STREAM_START  \
                    (V4L2_EVENT_PRIVATE_START + 1)
#define CIFISP_V4L2_EVENT_STREAM_STOP   \
                    (V4L2_EVENT_PRIVATE_START + 2)

#define RKAIQ_FILE_PATH_LEN                       64
#define RKAIQ_FLASH_NUM_MAX                       2

/* 1 vicap + 2 mipi + 1 bridge + 1 redundance */
#define MAX_MEDIA_NODES                           16

#define IQ_PATH "/vendor/etc/iqfiles/"

static int g_silent = 0;
static int g_width = 2688;
static int g_height = 1520;
static int g_hasMulCam = 0;

struct rkaiq_media_info {
    char sdIspPath[RKAIQ_FILE_PATH_LEN];
    char vdParamsPath[RKAIQ_FILE_PATH_LEN];
    char vdStatsPath[RKAIQ_FILE_PATH_LEN];
    char mainpath[RKAIQ_FILE_PATH_LEN];
    char sensorEntityName[32];

    char mdevPath[32];
    int available;
    rk_aiq_sys_ctx_t* aiq_ctx;

    pthread_t pid;
};

static struct rkaiq_media_info g_mediaInfos[MAX_MEDIA_NODES];

static void ErrnoExit(const char *s)
{
    ERR("%s error %d, %s\n", s, errno, strerror(errno));
    exit(EXIT_FAILURE);
}

static int Xioctl(int fh, int request, void *arg)
{
    int r;

    do {
        r = ioctl(fh, request, arg);
    } while (-1 == r && EINTR == errno);

    return r;
}


static int rkaiq_get_devname(struct media_device *device, const char *name, char *devName)
{
    const char *devname;
    struct media_entity *entity = nullptr;

    entity = media_get_entity_by_name(device, name, strlen(name));
    if (!entity)
        return -1;

    devname = media_entity_get_devname(entity);
    if (!devname) {
        fprintf(stderr, "can't find %s device path!", name);
        return -1;
    }

    strncpy(devName, devname, RKAIQ_FILE_PATH_LEN - 1);
    devName[RKAIQ_FILE_PATH_LEN - 1] = '\0';

    DBG("get %s devname: %s\n", name, devName);

    return 0;
}

int rkaiq_get_media_info(struct rkaiq_media_info *mediaInfo)
{
    struct media_device *device = nullptr;
    const char *sensorName;
    int ret;

    device = media_device_new (mediaInfo->mdevPath);
    if (!device)
        return -ENOMEM;
    /* Enumerate entities, pads and links. */
    ret = media_device_enumerate (device);
    if (ret)
        return ret;
    if (!ret) {
        /* Try rkisp */
        ret = rkaiq_get_devname(device, "rkisp-isp-subdev",
                                mediaInfo->sdIspPath);
        ret |= rkaiq_get_devname(device, "rkisp-input-params",
                                mediaInfo->vdParamsPath);
        ret |= rkaiq_get_devname(device, "rkisp-statistics",
                                mediaInfo->vdStatsPath);
        ret |= rkaiq_get_devname(device, "rkisp_mainpath",
                                mediaInfo->mainpath);
    }
    if (ret) {
        fprintf(stderr, "Cound not find rkisp dev names, skipped %s\n", mediaInfo->mdevPath);
        media_device_unref (device);
        return ret;
    }

    sensorName = rk_aiq_uapi2_sysctl_getBindedSnsEntNmByVd(mediaInfo->mainpath);
    if (sensorName == nullptr || strlen(sensorName) == 0) {
        fprintf(stderr, "ERR: No sensor attached to %s\n", mediaInfo->mdevPath);
        media_device_unref (device);
        return -EINVAL;
    }

    strncpy(mediaInfo->sensorEntityName, sensorName, sizeof(mediaInfo->sensorEntityName) - 1);
    mediaInfo->sensorEntityName[sizeof(mediaInfo->sensorEntityName) - 1] = '\0';

    media_device_unref (device);

    return ret;
}

static void init_engine(struct rkaiq_media_info *mediaInfo)
{
    mediaInfo->aiq_ctx = rk_aiq_uapi2_sysctl_init(mediaInfo->sensorEntityName,
                                                  IQ_PATH, NULL, NULL);
    /* 3A server has listened stream events already */
    rk_aiq_uapi2_sysctl_setListenStrmStatus(mediaInfo->aiq_ctx, false);
    if (g_hasMulCam)
        rk_aiq_uapi2_sysctl_setMulCamConc(mediaInfo->aiq_ctx, 1);

    if (rk_aiq_uapi2_sysctl_prepare(mediaInfo->aiq_ctx,
            g_width, g_height, RK_AIQ_WORKING_MODE_NORMAL)) {
        ERR("rkaiq engine prepare failed !\n");
        exit(-1);
    }
}

static void start_engine(struct rkaiq_media_info *mediaInfo)
{
    DBG("device manager start\n");
    rk_aiq_uapi2_sysctl_start(mediaInfo->aiq_ctx);
    if (mediaInfo->aiq_ctx == NULL) {
        ERR("rkisp_init engine failed\n");
        exit(-1);
    } else {
        DBG("rkisp_init engine succeed\n");
    }
}

static void stop_engine(struct rkaiq_media_info *mediaInfo)
{
    rk_aiq_uapi2_sysctl_stop(mediaInfo->aiq_ctx, false);
}

static void deinit_engine(struct rkaiq_media_info *mediaInfo)
{
    rk_aiq_uapi2_sysctl_deinit(mediaInfo->aiq_ctx);
}

// blocked func
static int wait_stream_event(int fd, unsigned int event_type, int timeOutMs)
{
    int ret;
    struct v4l2_event event;

    CLEAR(event);

    do {
        /*
         * Xioctl instead of poll.
         * Since poll() cannot wait for input before stream on,
         * it will return an error directly. So, use ioctl to
         * dequeue event and block until sucess.
         */
        ret = Xioctl(fd, VIDIOC_DQEVENT, &event);
        if (ret == 0 && event.type == event_type)
            return 0;
    } while (true);

    return -1;
}

static int subscrible_stream_event(struct rkaiq_media_info *mediaInfo, int fd, bool subs)
{
    struct v4l2_event_subscription sub;
    int ret = 0;

    CLEAR(sub);
    sub.type = CIFISP_V4L2_EVENT_STREAM_START;
    ret = Xioctl(fd,
                 subs ? VIDIOC_SUBSCRIBE_EVENT : VIDIOC_UNSUBSCRIBE_EVENT,
                 &sub);
    if (ret) {
        ERR("can't subscribe %s start event!\n", mediaInfo->vdParamsPath);
        exit(EXIT_FAILURE);
    }

    CLEAR(sub);
    sub.type = CIFISP_V4L2_EVENT_STREAM_STOP;
    ret = Xioctl(fd,
                 subs ? VIDIOC_SUBSCRIBE_EVENT : VIDIOC_UNSUBSCRIBE_EVENT,
                 &sub);
    if (ret) {
        ERR("can't subscribe %s stop event!\n", mediaInfo->vdParamsPath);
    }

    DBG("subscribe events from %s success !\n", mediaInfo->vdParamsPath);

    return 0;
}

void ParseArgs(int argc, char **argv)
{
    int c;

    while (1) {
        int optionIndex = 0;
        static struct option longOptions[] = {
            {"silent",    no_argument,       nullptr, 's' },
            {"help",      no_argument,       nullptr, 'h' },
            {nullptr,     no_argument,       nullptr,  0  }
        };

        c = getopt_long(argc, argv, "sh", longOptions, &optionIndex);
        if (c == -1)
            break;

        switch (c) {
        case 'w':
            g_width = atoi(optarg);
            break;
        case 'h':
            g_height = atoi(optarg);
            break;
        case 's':
            g_silent = 1;
            break;
        case '?':
            ERR("Usage: %s to start 3A engine\n"
                "         --silent,  optional, subpress debug log\n",
                argv[0]);
            exit(-1);

        default:
            ERR("?? getopt returned character code %c ??\n", c);
        }
    }
}

void *EngineThread(void *arg)
{
    int ispFd;
    struct rkaiq_media_info *mediaInfo;

    mediaInfo = (struct rkaiq_media_info *) arg;

    ispFd = open(mediaInfo->vdParamsPath, O_RDWR);
    if (ispFd < 0) {
        ERR("open %s failed %s\n", mediaInfo->vdParamsPath, strerror(errno));
        return NULL;
    }

    init_engine(mediaInfo);
    subscrible_stream_event(mediaInfo, ispFd, true);

    for (;;) {
        DBG("%s: wait stream start event...\n", mediaInfo->mdevPath);
        wait_stream_event(ispFd, CIFISP_V4L2_EVENT_STREAM_START, -1);
        DBG("%s: wait stream start event success ...\n", mediaInfo->mdevPath);
        start_engine(mediaInfo);

        DBG("%s: wait stream stop event...\n", mediaInfo->mdevPath);
        wait_stream_event(ispFd, CIFISP_V4L2_EVENT_STREAM_STOP, -1);
        DBG("%s: wait stream stop event success ...\n", mediaInfo->mdevPath);

        stop_engine(mediaInfo);
    }

    subscrible_stream_event(mediaInfo, ispFd, false);
    deinit_engine(mediaInfo);
    close(ispFd);

    return nullptr;
}

int main(int argc, char **argv)
{
    int ret, i;
    int threads = 0;

    /* Line buffered so that printf can flash every line if redirected to
     * no-interactive device.
     */
    setlinebuf(stdout);

    ParseArgs(argc, argv);

    for (i = 0; i < MAX_MEDIA_NODES; i++) {
        snprintf(g_mediaInfos[i].mdevPath, sizeof(g_mediaInfos[i].mdevPath), "/dev/media%d", i);
        if (rkaiq_get_media_info(&g_mediaInfos[i])) {
            ERR("Bad media topology for: %s\n", g_mediaInfos[i].mdevPath);
            g_mediaInfos[i].available = 0;
            continue;
        }
        g_mediaInfos[i].available = 1;
        threads++;
    }

    if (threads > 1)
        g_hasMulCam = 1;

    for (i = 0; i < MAX_MEDIA_NODES; i++) {
        if (!g_mediaInfos[i].available)
            continue;
        ret = pthread_create(&g_mediaInfos[i].pid, nullptr, EngineThread, &g_mediaInfos[i]);
        if (ret) {
            g_mediaInfos[i].pid = 0;
            ERR("Failed to create camera engine thread for: %s\n", g_mediaInfos[i].mdevPath);
            ErrnoExit("Create thread failed");
        }
    }

    for (i = 0; i < MAX_MEDIA_NODES; i++) {
        if (!g_mediaInfos[i].available || g_mediaInfos[i].pid == 0)
            continue;
        pthread_join(g_mediaInfos[i].pid, nullptr);
    }

    return 0;
}
