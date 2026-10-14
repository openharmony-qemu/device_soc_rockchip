#!/usr/bin/env bash
# Build mcs_km.ko (out-of-tree kernel module) against the OH linux-6.6 tree.
set -u
# Package-relative layout (this script lives in device/soc/rockchip/rk3568/mcs/scripts/):
#   ROOT         kernel/linux/common_modules/mcs (ko source: $ROOT/kernel/mcs_km)
#   OUT          build outputs -> $ROOT/out/logs
# External deps (overridable): CODE = OpenHarmony full source tree
# (needs a built kernel: out/kernel/{src_tmp,OBJ}/linux-6.6).
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
SRC=$CODE/out/kernel/src_tmp/linux-6.6
OBJ=$CODE/out/kernel/OBJ/linux-6.6
_llvm=$CODE/prebuilts/clang/ohos/linux-x86_64/llvm/bin
[ -x $_llvm/clang ] || { echo "ERROR: clang not found under $_llvm (repo sync prebuilts/clang)"; exit 1; }
[ -d $SRC ] || { echo "ERROR: built kernel not found at $SRC (run the kernel build first)"; exit 1; }
export PATH=$CODE/prebuilts/clang/ohos/linux-x86_64/llvm/bin:$PATH
mkdir -p $OUT/logs
cd $ROOT/mcs_km
rm -f *.o *.ko *.mod *.mod.c Module.symvers modules.order
rm -rf .tmp_versions
make -C $SRC O=$OBJ M=$PWD ARCH=arm64 CC=$CODE/prebuilts/clang/ohos/linux-x86_64/llvm/bin/clang LLVM=1 KBUILD_MODPOST_WARN=1 modules > $OUT/logs/ko-build-ohrt.log 2>&1
echo BUILD_EXIT=$?
tail -4 $OUT/logs/ko-build-ohrt.log
$OBJ/scripts/sign-file sha512 $OBJ/certs/signing_key.pem $OBJ/certs/signing_key.x509 mcs_km.ko && echo SIGNED
modinfo mcs_km.ko | head -6
echo ==COMPAT_STRINGS==
echo OHRT_COUNT=$(strings mcs_km.ko | grep -c "oh_rt,")
echo OE_COUNT=$(strings mcs_km.ko | grep -c "oe,")
echo ==SYMBOL_CHECK==
grep -oE "modpost: \"[A-Za-z_][A-Za-z0-9_]*\"" $OUT/logs/ko-build-ohrt.log | sed "s/modpost: \"//;s/\"//" | sort -u > /tmp/need2.txt
echo NEED=$(wc -l < /tmp/need2.txt)
grep -rhoE "EXPORT_SYMBOL(_GPL|_NS|_GPL_NS)?\([A-Za-z0-9_]+" $SRC --include=*.c --include=*.h --include=*.S | sed -E "s/.*\(([A-Za-z0-9_]+)$/\1/" | sort -u > /tmp/have2.txt
echo HAVE=$(wc -l < /tmp/have2.txt)
comm -23 /tmp/need2.txt /tmp/have2.txt > /tmp/missing2.txt
echo MISSING=$(wc -l < /tmp/missing2.txt)
cat /tmp/missing2.txt
