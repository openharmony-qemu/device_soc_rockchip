# prebuilt/ — open-amp / libmetal 预编译动态库

源码**不在本仓**（待三方开源引入评审通过后进入 OpenHarmony third_party）。
本目录只交付动态库、头文件与适配补丁；libmica 的二进制不在此处，
由本仓 `libmica/` 源码经 `scripts/build_libmica.sh` 编译产出。

## 来源

- libmetal：上游 2022.10 + `patches/libmetal-0001-build-make-the-sysfs-device-layer-optional.patch`
- open-amp：上游 2022.10.1 + `patches/0001-lib-remoteproc-elf_loader-fix-trailing-non-LOAD-segment-load-failure.patch`
- 构建环境：OpenHarmony clang（prebuilts/clang/ohos/linux-x86_64）+ OH musl sysroot（aarch64）
- 构建配置：`WITH_LIBSYSFS=OFF`（libmetal 不依赖 sysfsutils）

## 文件清单

| 文件 | 说明 |
|---|---|
| `lib/libmetal.so.1` | libmetal 动态库（SONAME 名；链接用 `-l:libmetal.so.1` 精确指定） |
| `lib/libopen_amp.so.1` | open-amp 动态库（SONAME 名；链接用 `-l:libopen_amp.so.1`） |
| `include/metal/`、`include/openamp/` | libmetal / open-amp 对外头文件（libmica 头文件在 `../libmica/include/`） |

不含静态库（.a）与无版本名开发链接（`.so`）——链接一律用 `-l:` 语法按 SONAME 精确指定；
musl loader/libc 也不入库存放。如需静态链接三方库，按下方重编方法自行产出。

## 上板部署集

`scripts/build_libmica.sh` 会在 `out/artifacts/lib/` 生成完整部署集
（`libmica.so` + `libmetal.so.1` + `libopen_amp.so.1` + musl loader/libc），
上板时整目录拷到 `/data/lib`。musl 的 loader/libc 取自 OH 源码树的
aarch64 musl sysroot（板上用户态为 32 位，无现成 aarch64 loader，需随部署集携带）。

## 重编方法

将上游源码快照（libmetal 2022.10 / open-amp 2022.10.1）放到 `rk3568/mcs/third_party/{libmetal,open-amp}/` 并打 `prebuilt/patches/` 下的补丁，然后跑 `scripts/build_userlibs.sh`（需要 OpenHarmony 全源码树环境，见脚本头部说明）。
