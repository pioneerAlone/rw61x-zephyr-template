# RW61x Zephyr Template

基于 NXP nxp_zsdk (nxp-v4.3-branch) 的 RW61x Zephyr RTOS 工程模版，支持 WiFi 6 + BLE 5.3 双模无线功能。

## 项目结构

```
rw61x-zephyr-template/       ← workspace root
├── app/                      ← 应用代码（本仓库核心）
│   ├── west.yml              ← west manifest（T-topology）
│   ├── CMakeLists.txt
│   ├── prj.conf              ← 基础 Kconfig
│   ├── src/main.c
│   ├── boards/
│   │   └── frdm_rw612.overlay
│   ├── conf/                 ← 可组合 Kconfig 片段
│   │   ├── debug.conf
│   │   ├── wifi.conf
│   │   └── ble.conf
│   └── .vscode/              ← VS Code 配置
│       ├── launch.json       ← J-Link 调试
│       ├── tasks.json        ← west build/flash 任务
│       ├── extensions.json   ← 推荐扩展
│       └── settings.json     ← Python venv 路径
├── zephyr/                   ← west update 拉取（不提交）
├── modules/                  ← west update 拉取（不提交）
└── build/                    ← 构建产物（不提交）
```

本项目采用 Zephyr **T-topology** 架构：`app/` 是 manifest repo，通过 `app/west.yml` 声明 nxp_zsdk 依赖，`west update` 自动拉取 Zephyr 及所有模块。

## 快速开始

### 前置条件

- macOS / Linux
- Python 3.10+
- [Zephyr SDK](https://docs.zephyrproject.org/latest/develop/toolchains/zephyr_sdk.html)
- CMake 3.20+
- J-Link 调试器（烧录/调试用）

### 1. 克隆并初始化 workspace

```bash
git clone https://github.com/pioneerAlone/rw61x-zephyr-template.git
cd rw61x-zephyr-template
west init -l app
west update
```

### 2. 创建 Python 虚拟环境

```bash
python3 -m venv .venv
source .venv/bin/activate
pip install -r zephyr/scripts/requirements.txt
```

### 3. 下载射频固件 blobs

```bash
west blobs fetch hal_nxp
```

RW61x 的 WiFi/BLE 由独立协处理器驱动，需要 NXP 提供的预编译固件。

### 4. 编译

```bash
west build -b frdm_rw612 app/
```

使用 Kconfig 片段启用额外功能：

```bash
# 启用调试日志
west build -b frdm_rw612 app/ -- -DEXTRA_CONF_FILE=conf/debug.conf

# 启用 WiFi
west build -b frdm_rw612 app/ -- -DEXTRA_CONF_FILE=conf/wifi.conf

# 组合多个片段
west build -b frdm_rw612 app/ -- -DEXTRA_CONF_FILE="conf/debug.conf;conf/wifi.conf"
```

### 5. 烧录

```bash
west flash --runner jlink
```

### 6. 查看串口日志

连接板子的 USB-Serial 端口，波特率 115200：

```bash
screen /dev/tty.usbmodem* 115200
```

预期输出：

```
[00:00:00.000,000] <inf> main: RW61x Zephyr template v1.0
[00:00:00.000,000] <inf> main: Board: frdm_rw612
[00:00:05.000,000] <inf> main: Heartbeat
```

## VS Code 调试

1. 用 VS Code 打开 `app/` 目录
2. 安装推荐扩展（cortex-debug 等）
3. 按 F5 启动 J-Link 调试，程序会停在 `main()` 入口

## 硬件

- 目标板：[FRDM-RW612](https://www.nxp.com/design/design-center/development-boards-and-designs/general-purpose-mcus/frdm-development-board-for-rw612-wireless-mcu:FRDM-RW612)
- 芯片：NXP RW612（Cortex-M33, WiFi 6 + BLE 5.3）
- 调试器：板载或外接 J-Link

## License

[Apache-2.0](LICENSE)
