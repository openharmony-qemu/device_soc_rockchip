/*
 * pedestal_compat.c — baremetal-only 裁剪的链接垫片（OH 侧适配，非上游代码）
 *
 * 背景：oh-port 裁剪掉 xen/jailhouse/riscv pedestal 源文件后，
 *       library/remoteproc/remoteproc_core.c 的 create_client() 仍引用
 *       rproc_jailhouse_ops / rproc_xen_ops / rproc_riscv_ops 三个符号；
 *       baremetal 路径（ped == BARE_METAL）永远不会选中它们。
 *
 * 另：OH musl 未实现 pthread_cancel；libmica 仅在 gdb 线程清理与
 *     baremetal_rproc 的 rproc_init 错误路径引用。本测试路径（无 gdb）
 *     不会触发其真实语义，返回 ESRCH 占位。
 *
 * GN 化时的正规做法：在 GN config 中按 pedestal 选择源文件，并用
 * libmica 的 musl 适配 patch 处理 pthread_cancel，替代本垫片。
 */

#include <errno.h>
#include <pthread.h>

#include <openamp/remoteproc.h>

const struct remoteproc_ops rproc_jailhouse_ops = { 0 };
const struct remoteproc_ops rproc_xen_ops = { 0 };
const struct remoteproc_ops rproc_riscv_ops = { 0 };

int pthread_cancel(pthread_t th)
{
	(void)th;
	return ESRCH;
}
