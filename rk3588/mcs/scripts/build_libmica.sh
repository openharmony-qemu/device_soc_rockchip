#!/usr/bin/env bash
# Build libmica.a + libmica.so (master side, baremetal pedestal only) with OH
# clang + musl sysroot. Source: libmica/ in this module (vanilla
# snapshot of the openEuler mcs `library/` subtree + build-time patches/).
# Self-managed — this script does not read the openEuler repo.
# Objects are compiled -fPIC so the same set feeds both the static archive
# and the shared object; libmica.so embeds the pedestal_compat shims (the
# trimmed jailhouse/xen/riscv ops placeholders) so a dynamic program only
# needs -lmica on the link line.
set -eu
# Package-relative layout (this script lives in device/soc/rockchip/rk3588/mcs/scripts/):
#   ROOT         kernel/linux/common_modules/mcs
#   OUT          build outputs -> $ROOT/out/{staging,logs} (not for upload)
# External deps (overridable): CODE = OpenHarmony full source tree.
# Prereq: none - libmetal/open-amp are consumed as prebuilt shared objects from
# $ROOT/prebuilt/lib (+ headers from $ROOT/prebuilt/include); libmica is built
# from the sources in this repo.
# CODE must point to a repo-synced OpenHarmony full source tree. It provides
# the OH clang toolchain (prebuilts/clang/ohos/linux-x86_64) and, after one
# product build, the musl sysroot (out/<product>/obj/third_party/musl/usr).
# Autodetected by walking up from this script (the tree root has prebuilts/);
# override with:  export CODE=/path/to/openharmony/code
CODE=${CODE:-}
if [ -z "$CODE" ]; then
  _d=$(cd "$(dirname "$0")" && pwd)
  while [ "$_d" != "/" ]; do
    if [ -d "$_d/prebuilts/clang" ]; then CODE=$_d; break; fi
    _d=$(dirname "$_d")
  done
fi
CODE=${CODE:?ERROR: export CODE=<OpenHarmony full source tree root>}
ROOT=$(cd "$(dirname "$0")/.." && pwd)
OUT=${OUT:-$ROOT/out}
LLVMBIN=$CODE/prebuilts/clang/ohos/linux-x86_64/llvm/bin

for _t in clang llvm-ar llvm-strip; do
  [ -x "$LLVMBIN/$_t" ] || { echo "ERROR: $_t not found at $LLVMBIN"; \
    echo "       repo sync prebuilts/clang in your OH tree (or fix CODE)"; exit 1; }
done
CLANG=$LLVMBIN/clang
# musl sysroot: autodetected from the built products (override: export MUSL=...)
# (a product out dir may exist before musl is fully built - only a sysroot with
#  crt1.o counts as usable)
MUSL=${MUSL:-}
for _d in $CODE/out/*/obj/third_party/musl/usr; do
  [ -f "$_d/lib/aarch64-linux-ohos/crt1.o" ] || continue
  MUSL=$_d; break
done
[ -n "$MUSL" ] || { echo "ERROR: no usable musl sysroot under $CODE/out/*/obj/third_party/musl/usr"; \
  echo "       build the OH product once, or export MUSL=<sysroot>"; exit 1; }
echo "musl sysroot: $MUSL"
STAGING=$OUT/staging
LOGS=$OUT/logs
TFLAGS="--target=aarch64-linux-ohos --sysroot=$MUSL -I$MUSL/include/aarch64-linux-ohos -B$MUSL/lib/aarch64-linux-ohos -L$MUSL/lib/aarch64-linux-ohos"

# work-dir copy: package source stays vanilla/pristine
WORK=/tmp/libmica-oh-build
rm -rf $WORK && mkdir -p $WORK
cp -a "$ROOT/libmica" $WORK/libmica

# apply patches/ (kept as files; snapshot stays vanilla)
for p in "$ROOT"/libmica/patches/*.patch; do
  echo "[libmica] applying $(basename $p)"
  patch -d $WORK/libmica -p1 -s < "$p"
done

mkdir -p $LOGS
cd $WORK/libmica
rm -f /tmp/libmica_*.o
OBJS=""
for f in remoteproc/baremetal_rproc.c remoteproc/remoteproc_core.c remoteproc/mica_rsc_table.c \
         memory/shm_pool.c rpmsg_device/rpmsg_service.c rpmsg_device/rpmsg_vdev.c \
         mica/mica.c rbuf_device/rbuf_dev.c rbuf_device/ring_buffer.c \
         user_msg/user_msg.c user_msg/user_msg_mem.c; do
  o=/tmp/libmica_$(echo "$f" | tr / _ | sed "s/\.c$/.o/")
  $CLANG $TFLAGS -I$WORK/libmica/include -I$ROOT/prebuilt/include -O2 -fPIC -Wall -c "$f" -o "$o" 2>> $LOGS/libmica-build.log || { echo COMPILE_FAIL:$f; tail -20 $LOGS/libmica-build.log; exit 1; }
  OBJS="$OBJS $o"
done
# pedestal compat shims (trimmed pedestal ops placeholders + musl pthread_cancel
# shim): compiled first, embedded into libmica.so, archived separately as .a
# for the static reference chain
$CLANG $TFLAGS -I$WORK/libmica/include -I$ROOT/prebuilt/include -O2 -fPIC -Wall -c "$ROOT/libmica-compat/pedestal_compat.c" -o /tmp/pedestal_compat.o
$LLVMBIN/llvm-ar rcs $STAGING/lib/libmica_compat.a /tmp/pedestal_compat.o

$LLVMBIN/llvm-ar rcs $STAGING/lib/libmica.a $OBJS
$CLANG $TFLAGS -shared -o $STAGING/lib/libmica.so $OBJS /tmp/pedestal_compat.o \
  -L$ROOT/prebuilt/lib -l:libmetal.so.1 -l:libopen_amp.so.1 -lpthread

# runtime deployment set for dynamically linked apps: the just-built libmica.so
# plus the third-party SONAMEs from prebuilt - push that dir to the board
# alongside the binaries.
mkdir -p $OUT/artifacts/lib
rm -f $OUT/artifacts/lib/*.so*
cp -L $STAGING/lib/libmica.so $OUT/artifacts/lib/
cp -L $ROOT/prebuilt/lib/libmetal.so.1 $ROOT/prebuilt/lib/libopen_amp.so.1 $OUT/artifacts/lib/


ls -la $STAGING/lib/libmica.a $STAGING/lib/libmica.so $STAGING/lib/libmica_compat.a
echo "== smoke link (mica, dynamic) =="
cat > /tmp/smoke3.c <<'EOF'
#include <mica/mica.h>
#include <openamp/remoteproc.h>
extern const struct remoteproc_ops rproc_bare_metal_ops;
int main(void)
{
    (void)mica_create;
    (void)rproc_bare_metal_ops;
    return 0;
}
EOF
# dynamic link smoke: resolves libmica (staging) against prebuilt libmetal/open-amp
$CLANG $TFLAGS -I$WORK/libmica/include -I$ROOT/prebuilt/include /tmp/smoke3.c \
  -L$STAGING/lib -L$ROOT/prebuilt/lib -lmica -lmica_compat -l:libmetal.so.1 -l:libopen_amp.so.1 -lpthread -o /tmp/smoke3 \
  && echo MICA_LINK_OK || echo MICA_LINK_FAIL
