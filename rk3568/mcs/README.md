# RK3568 混合部署（AMP）— 架构仓（device/soc/rockchip/rk3568/mcs）

RK3568 混合部署方案的**架构层**（与具体板卡无关的部分）：

- `mcs_km/`：内核让渡框架（out-of-tree 内核模块 `mcs_km.ko`）——CPU 隔离、保留内存映射、IPI 转发、`/dev/mcs` 设备
- `libmica/`：主核侧 AMP 通信库源码（remoteproc 裸机 pedestal + rpmsg 服务封装，含 OH 适配 patch），由 `scripts/build_libmica.sh` 现场编译
- `libmica-compat/`：OH musl 兼容 shim（编译进 libmica.so，另单独产出 libmica_compat.a）
- `prebuilt/`：open-amp / libmetal 预编译**动态库**与头文件（OH aarch64 musl，OH clang 构建；源码待三方开源引入评审通过后进 third_party，适配补丁在 `prebuilt/patches/`）
- `scripts/`：`rebuild_ohrt.sh`（构建 mcs_km.ko）、`build_libmica.sh`（源码编译 libmica + 生成上板 so 部署集）、`build_userlibs.sh`（重编三方库，评审后启用）、`check_gpl.sh`（三方许可检查）

两仓结构与快速开始见板级仓 `device/board/hihope/rk3568_amp/README.md` 与 `USER_GUIDE.md`。

## 构建顺序

```bash
cd <OH 树>/device/soc/rockchip/rk3568/mcs/scripts
./rebuild_ohrt.sh     # mcs_km.ko
./build_libmica.sh    # libmica.a/.so（源码编译）→ ../out/staging/lib；上板 so 部署集 → ../out/artifacts/lib
```

`prebuilt/lib` 只交付 libmetal / open-amp 的 SONAME 动态库（`libmetal.so.1`、`libopen_amp.so.1`，链接用 `-l:` 语法精确指定），不含静态库与 musl loader/libc（后者由 `build_libmica.sh` 从 OH musl sysroot 生成进部署集）；libmica 一律从本仓源码编译，不在 prebuilt 中存放其二进制。

## prebuilt 二进制说明

见 `prebuilt/README.md`（文件清单、来源、重编方法）。
