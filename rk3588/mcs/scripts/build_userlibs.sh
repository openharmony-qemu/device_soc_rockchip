#!/usr/bin/env bash
# Build libmetal / libopen_amp for OHOS aarch64 (musl) with OH clang.
# Source: $CODE/third_party/{libmetal,open-amp} (vanilla snapshots +
# build-time patches/). Self-managed — this script does not read openEuler-repo
# copies.
#
# Produces BOTH .a (static reference chain) and .so (apps link the .so).
# libmetal builds with the sysfs device layer disabled (WITH_LIBSYSFS=OFF, see
# third_party/libmetal/patches/): no sysfsutils dependency.
set -eu
# Package-relative layout (this script lives in device/soc/rockchip/rk3588/mcs/scripts/):
#   ROOT         device/board/hihope/rk3568/mcs
#   THIRD_PARTY  vanilla snapshots; bundled $ROOT/third_party wins, falls back to $CODE/third_party
#   OUT          build outputs -> $ROOT/out/{staging,logs} (not for upload)
# External dep (overridable): CODE = OpenHarmony full source tree.
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
# Bundled third_party/ (self-contained integration copy) wins when present.
[ -d "$ROOT/third_party" ] && THIRD_PARTY=${THIRD_PARTY:-$ROOT/third_party}
THIRD_PARTY=${THIRD_PARTY:-$CODE/third_party}
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
MIRROR=$THIRD_PARTY
STAGING=$OUT/staging
LOGS=$OUT/logs
JOBS=8
export PATH=$LLVMBIN:$PATH
export AR=$LLVMBIN/llvm-ar RANLIB=$LLVMBIN/llvm-ranlib STRIP=$LLVMBIN/llvm-strip
TFLAGS="--target=aarch64-linux-ohos --sysroot=$MUSL -I$MUSL/include/aarch64-linux-ohos -B$MUSL/lib/aarch64-linux-ohos -L$MUSL/lib/aarch64-linux-ohos"
CM="-DCMAKE_POLICY_VERSION_MINIMUM=3.5"

echo "THIRD_PARTY=$THIRD_PARTY"
mkdir -p $STAGING/lib $STAGING/include $LOGS
[ -f $STAGING/lib/librt.a ] || $LLVMBIN/llvm-ar rcs $STAGING/lib/librt.a

# Drop stale sysfsutils outputs from older script versions (no longer built).
rm -f $STAGING/lib/libsysfs.a $STAGING/lib/libsysfs.la $STAGING/lib/libsysfs.so \
      $STAGING/lib/libsysfs.so.* $STAGING/lib/pkgconfig/libsysfs.pc
rm -rf $STAGING/include/sysfs
rm -f $OUT/artifacts/lib/libsysfs.so* $OUT/artifacts/bin/systool

echo "=== 1/2 libmetal (vanilla + patches, sysfs device layer OFF) ==="
W=/tmp/metal-oh-src
rm -rf $W /tmp/metal-oh-build
cp -a $MIRROR/libmetal $W
for p in $MIRROR/libmetal/patches/*.patch; do
  echo "[libmetal] applying $(basename $p)"
  patch -d $W -p1 -s < "$p"
done
cmake -B /tmp/metal-oh-build -S $W $CM \
  -DCMAKE_SYSTEM_NAME=Linux -DCMAKE_SYSTEM_PROCESSOR=aarch64 \
  -DCMAKE_C_COMPILER=$CLANG -DCMAKE_C_FLAGS="$TFLAGS" \
  -DCMAKE_FIND_ROOT_PATH=$STAGING -DCMAKE_INCLUDE_PATH=$STAGING/include -DCMAKE_LIBRARY_PATH=$STAGING/lib \
  -DCMAKE_INSTALL_PREFIX=$STAGING -DCMAKE_BUILD_TYPE=Release \
  -DWITH_STATIC_LIB=ON -DWITH_SHARED_LIB=ON -DWITH_LIBSYSFS=OFF -DWITH_EXAMPLES=OFF -DWITH_TESTS=OFF -DWITH_DOC=OFF \
  > $LOGS/metal-cfg.log 2>&1 || { echo METAL_CFG_FAIL; tail -25 $LOGS/metal-cfg.log; exit 1; }
make -C /tmp/metal-oh-build -j$JOBS > $LOGS/metal-build.log 2>&1 || { echo METAL_BUILD_FAIL; tail -25 $LOGS/metal-build.log; exit 1; }
make -C /tmp/metal-oh-build install > /dev/null 2>&1
echo METAL_OK

echo "=== 2/2 open-amp (vanilla + patches) ==="
W=/tmp/openamp-oh-src
rm -rf $W /tmp/openamp-oh-build
cp -a $MIRROR/open-amp $W
for p in $MIRROR/open-amp/patches/*.patch; do
  echo "[open-amp] applying $(basename $p)"
  patch -d $W -p1 -s < "$p"
done
cmake -B /tmp/openamp-oh-build -S $W $CM \
  -DCMAKE_SYSTEM_NAME=Linux -DCMAKE_SYSTEM_PROCESSOR=aarch64 \
  -DCMAKE_C_COMPILER=$CLANG -DCMAKE_C_FLAGS="$TFLAGS" \
  -DCMAKE_FIND_ROOT_PATH=$STAGING -DCMAKE_INCLUDE_PATH=$STAGING/include -DCMAKE_LIBRARY_PATH=$STAGING/lib \
  -DCMAKE_INSTALL_PREFIX=$STAGING -DCMAKE_BUILD_TYPE=Release \
  -DMACHINE=generic -DWITH_STATIC_LIB=ON -DWITH_SHARED_LIB=ON -DWITH_APPS=OFF -DWITH_PROXY=OFF \
  > $LOGS/openamp-cfg.log 2>&1 || { echo OPENAMP_CFG_FAIL; tail -25 $LOGS/openamp-cfg.log; exit 1; }
make -C /tmp/openamp-oh-build -j$JOBS > $LOGS/openamp-build.log 2>&1 || { echo OPENAMP_BUILD_FAIL; tail -25 $LOGS/openamp-build.log; exit 1; }
make -C /tmp/openamp-oh-build install > /dev/null 2>&1
echo OPENAMP_OK

echo "== staging libs =="
ls -la $STAGING/lib/*.a $STAGING/lib/*.so* 2>/dev/null
echo "== smoke link (static) =="
cat > /tmp/smoke_ohos.c <<'EOF'
#include <metal/sys.h>
#include <metal/io.h>
#include <openamp/open_amp.h>
#include <openamp/rpmsg_virtio.h>
int main(void)
{
    struct metal_init_params p = {0};
    (void)metal_init(&p);
    (void)rpmsg_virtio_get_buffer_size;
    return 0;
}
EOF
$CLANG $TFLAGS -I$STAGING/include /tmp/smoke_ohos.c -L$STAGING/lib -lmetal -lopen_amp -lpthread -o /tmp/smoke_ohos \
  && echo LINK_OK && $LLVMBIN/llvm-file /tmp/smoke_ohos 2>/dev/null || $LLVMBIN/llvm-readelf -h /tmp/smoke_ohos | head -6
