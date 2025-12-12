/*
 *
 * Copyright 2015 Rockchip Electronics Co., LTD.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#define ALOG_TAG "rk_list"

#include <cerrno>
#include <cstring>
#include <cstdlib>
#include <securec.h>
#include "rk_list.h"
#include "mpp_log.h"

#define RK_LIST_ERROR

#ifdef RK_LIST_DEUBG
#define LIST_DEBUG(fmt, ...) mpp_log(fmt, ##__VA_ARGS__)
#else
#define LIST_DEBUG(fmt, ...) /* not debugging: nothing */
#endif

#ifdef RK_LIST_ERROR
#define LIST_ERROR(fmt, ...) mpp_err(fmt, ##__VA_ARGS__)
#else
#define LIST_ERROR(fmt, ...)
#endif

typedef struct rk_list_node {
    rk_list_node *prev;
    rk_list_node *next;
    RK_U32 key;
    RK_S32 size;
} rk_list_node;

static inline void list_node_init(rk_list_node *node)
{
    node->prev = node->next = node;
}

static inline void list_node_init_with_key_and_size(rk_list_node *node, RK_U32 key, RK_S32 size)
{
    list_node_init(node);
    node->key = key;
    node->size = size;
}

static rk_list_node *create_list(void *data, RK_S32 size, RK_U32 key)
{
    if (size <= 0) {
        LIST_ERROR("invalid size parameter: %d", size);
        return nullptr;
    }

    if (data == nullptr) {
        LIST_ERROR("data pointer is NULL");
        return nullptr;
    }
    rk_list_node *node = static_cast<rk_list_node *>(malloc(sizeof(rk_list_node) + size));
    if (node) {
        void *dst = reinterpret_cast<void *>(reinterpret_cast<uint8_t *>(node) + sizeof(rk_list_node));
        list_node_init_with_key_and_size(node, key, size);
        int32_t ret = memcpy_s(dst, size, data, size);
        if (ret != EOK) {
            LIST_ERROR("%{public}s: copy failed, error code: %{public}d", __func__, ret);
        }
    } else {
        LIST_ERROR("failed to allocate list node");
    }
    return node;
}

static inline void rk_list_add(rk_list_node *new_entry, rk_list_node *prev, rk_list_node *next)
{
    next->prev = new_entry;
    new_entry->next = next;
    new_entry->prev = prev;
    prev->next = new_entry;
}

static inline void rk_list_add(rk_list_node *new_entry, rk_list_node *head)
{
    rk_list_add(new_entry, head, head->next);
}

static inline void rk_list_add_tail(rk_list_node *new_entry, rk_list_node *head)
{
    rk_list_add(new_entry, head->prev, head);
}

RK_S32 rk_list::add_at_head(void *data, RK_S32 size)
{
    RK_S32 ret = -EINVAL;
    pthread_mutex_lock(&mutex);
    if (head) {
        rk_list_node *node = create_list(data, size, 0);
        if (node) {
            rk_list_add(node, head);
            count++;
            ret = 0;
        } else {
            ret = -ENOMEM;
        }
    }
    pthread_mutex_unlock(&mutex);
    return ret;
}

RK_S32 rk_list::add_at_tail(void *data, RK_S32 size)
{
    RK_S32 ret = -EINVAL;
    pthread_mutex_lock(&mutex);
    if (head) {
        rk_list_node *node = create_list(data, size, 0);
        if (node) {
            rk_list_add_tail(node, head);
            count++;
            ret = 0;
        } else {
            ret = -ENOMEM;
        }
    }
    pthread_mutex_unlock(&mutex);
    return ret;
}

static void release_list(rk_list_node *node, void *data, RK_S32 size)
{
    void *src = reinterpret_cast<void *>(reinterpret_cast<uint8_t *>(node) + sizeof(rk_list_node));
    if (node->size == size) {
        int32_t ret = memcpy_s(data, node->size, src, size);
        if (ret != EOK) {
            LIST_ERROR("%{public}s: copy failed, error code: %{public}d", __func__, ret);
        }
    } else {
        LIST_ERROR("node size check failed when release_list");
        size = (size < node->size) ? (size) : (node->size);
        int32_t ret = memcpy_s(data, node->size, src, size);
        if (ret != EOK) {
            LIST_ERROR("%{public}s: copy failed, error code: %{public}d", __func__, ret);
        }
    }
    free(node);
}

static inline void _rk_list_del(rk_list_node *prev, rk_list_node *next)
{
    next->prev = prev;
    prev->next = next;
}

static inline void rk_list_del_init(rk_list_node *node)
{
    _rk_list_del(node->prev, node->next);
    list_node_init(node);
}

static inline void _list_del_node_no_lock(rk_list_node *node, void *data, RK_S32 size)
{
    rk_list_del_init(node);
    release_list(node, data, size);
}

RK_S32 rk_list::del_at_head(void *data, RK_S32 size)
{
    RK_S32 ret = -EINVAL;
    pthread_mutex_lock(&mutex);
    if (head && count) {
        _list_del_node_no_lock(head->next, data, size);
        count--;
        ret = 0;
    }
    pthread_mutex_unlock(&mutex);
    return ret;
}

RK_S32 rk_list::del_at_tail(void *data, RK_S32 size)
{
    RK_S32 ret = -EINVAL;
    pthread_mutex_lock(&mutex);
    if (head && count) {
        _list_del_node_no_lock(head->prev, data, size);
        count--;
        ret = 0;
    }
    pthread_mutex_unlock(&mutex);
    return ret;
}

RK_S32 rk_list::list_is_empty()
{
    pthread_mutex_lock(&mutex);
    RK_S32 ret = (count == 0);
    pthread_mutex_unlock(&mutex);
    return ret;
}

RK_S32 rk_list::list_size()
{
    pthread_mutex_lock(&mutex);
    RK_S32 ret = count;
    pthread_mutex_unlock(&mutex);
    return ret;
}

RK_S32 rk_list::add_by_key(void *data, RK_S32 size, RK_U32 *key)
{
    RK_S32 ret = 0;
    (void)data;
    (void)size;
    (void)key;
    return ret;
}

RK_S32 rk_list::del_by_key(void *data, RK_S32 size, RK_U32 key)
{
    RK_S32 ret = 0;
    (void)data;
    (void)size;
    (void)key;
    return ret;
}


RK_S32 rk_list::show_by_key(void *data, RK_U32 key)
{
    RK_S32 ret = 0;
    (void)data;
    (void)key;
    return ret;
}

RK_S32 rk_list::flush()
{
    pthread_mutex_lock(&mutex);
    if (head) {
        while (count) {
            rk_list_node *node = head->next;
            rk_list_del_init(node);
            if (destroy) {
                void *data_ptr =
                    static_cast<void *>(static_cast<char *>(static_cast<void *>(node)) + sizeof(rk_list_node));
                destroy(data_ptr);
            }
            free(node);
            count--;
        }
    }
    pthread_mutex_unlock(&mutex);
    return 0;
}

rk_list::rk_list(node_destructor func) : destroy(nullptr), head(nullptr), count(0)
{
    pthread_mutexattr_t attr;
    pthread_mutexattr_init(&attr);
    pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
    pthread_mutex_init(&mutex, &attr);
    pthread_mutexattr_destroy(&attr);
    destroy = func;
    head = (rk_list_node *)malloc(sizeof(rk_list_node));
    if (head == nullptr) {
        LIST_ERROR("failed to allocate list header");
    } else {
        list_node_init_with_key_and_size(head, 0, 0);
    }
}

rk_list::~rk_list()
{
    flush();
    if (head) {
        free(head);
    }
    head = nullptr;
    destroy = nullptr;
    pthread_mutex_destroy(&mutex);
}

#if BUILD_RK_LIST_TEST
#include "vpu_mem.h"
#include <stdio.h>
#include <stdlib.h>
#include <cstring>

#define LOOP_RK_LIST 600

#define COUNT_ADD 100
#define COUNT_DEL 99

static volatile int err = 0;

static int rk_list_fifo_test(rk_list *list_0)
{
    int count;
    VPUMemLinear_t m;
    const MEMORY_SIZE = 100;
    for (count = 0; count < COUNT_ADD; count++) {
        err |= VPUMallocLinear(&m, MEMORY_SIZE);
        if (err) {
            printf("VPUMallocLinear in rk_list_fifo_test\n");
            break;
        }
        err |= list_0->add_at_head(&m, sizeof(m));
        if (err) {
            printf("add_at_head in rk_list_fifo_test\n");
            break;
        }
    }

    if (!err) {
        for (count = 0; count < COUNT_DEL; count++) {
            err |= list_0->del_at_tail(&m, sizeof(m));
            if (err) {
                printf("del_at_tail in rk_list_fifo_test\n");
                break;
            }
            err |= VPUFreeLinear(&m);
            if (err) {
                printf("VPUFreeLinear in rk_list_fifo_test\n");
                break;
            }
        }
    }
    return err;
}

static int rk_list_filo_test(rk_list *list_0)
{
    int count;
    VPUMemLinear_t m;
    const int BUFFER_SIZE = 100;
    for (count = 0; count < COUNT_ADD + COUNT_DEL; count++) {
        if (count & 1) {
            err |= list_0->del_at_head(&m, sizeof(m));
            if (err) {
                printf("del_at_head in rk_list_filo_test\n");
                break;
            }
            err |= VPUFreeLinear(&m);
            if (err) {
                printf("VPUFreeLinear in rk_list_fifo_test\n");
                break;
            }
        } else {
            err |= VPUMallocLinear(&m, BUFFER_SIZE);
            if (err) {
                printf("VPUMallocLinear in rk_list_filo_test\n");
                break;
            }
            err |= list_0->add_at_head(&m, sizeof(m));
            if (err) {
                printf("add_at_head in rk_list_fifo_test\n");
                break;
            }
        }
    }

    return err;
}


void *rk_list_test_loop_0(void *pdata)
{
    int i;
    rk_list *list_0 = (rk_list *)pdata;

    printf("rk_list test 0 loop start\n");
    for (i = 0; i < LOOP_RK_LIST; i++) {
        err |= rk_list_filo_test(list_0);
        if (err) {
            break;
        }
    }

    if (err) {
        printf("thread: found vpu mem operation err %d\n", err);
    } else {
        printf("thread: test done and found no err\n");
    }
    return nullptr;
}

int rk_list_test_0()
{
    int i=0, err = 0;
    printf("rk_list test 0 FIFO start\n");

    rk_list *list_0 = new rk_list((node_destructor)VPUFreeLinear);

    pthread_t mThread;
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

    pthread_create(&mThread, &attr, rk_list_test_loop_0, (void *)list_0);
    pthread_attr_destroy(&attr);

    for (i = 0; i < LOOP_RK_LIST; i++) {
        err |= rk_list_fifo_test(list_0);
        if (err) {
            break;
        }
    }
    if (err) {
        printf("main  : found rk_list operation err %d\n", err);
    } else {
        printf("main  : test done and found no err\n");
    }

    void *dummy;
    pthread_join(mThread, &dummy);

    printf("rk_list test 0 end size %d\n", list_0->list_size());
    delete list_0;
    return err;
}

#define TOTAL_RK_LIST_TEST_COUNT 1
#define ARGC_NUMBER_2 2
#define ARGC_NUMBER_3 3

typedef int (*RK_LIST_TEST_FUNC)(void);
RK_LIST_TEST_FUNC test_func[TOTAL_RK_LIST_TEST_COUNT] = {
    rk_list_test_0,
};

int main(int argc, char *argv[])
{
    int i=0, start = 0, end = 0;
    const int DEFAULT_INCREMENT = 1;
    const int START_ARG_INDEX = 1;
    const int END_ARG_INDEX = 2;
    if (argc < ARGC_NUMBER_2) {
        end = TOTAL_RK_LIST_TEST_COUNT;
    } else if (argc == ARGC_NUMBER_2) {
        start = atoi(argv[START_ARG_INDEX]);
        end = start + DEFAULT_INCREMENT;
    } else if (argc == ARGC_NUMBER_3) {
        start = atoi(argv[START_ARG_INDEX]);
        end = atoi(argv[END_ARG_INDEX]);
    } else {
        printf("too many argc %d\n", argc);
        return -1;
    }
    if (start < 0 || start > TOTAL_RK_LIST_TEST_COUNT || end < 0 || end > TOTAL_RK_LIST_TEST_COUNT) {
        printf("invalid input: start %d end %d\n", start, end);
        return -1;
    }
    for (i = start; i < end; i++) {
        int err = test_func[i]();
        if (err) {
            printf("test case %d return err %d\n", i, err);
            break;
        }
    }
    return 0;
}
#endif
