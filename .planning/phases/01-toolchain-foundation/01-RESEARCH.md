# Phase 1: Toolchain Foundation - Research

**Researched:** 2026-02-28
**Domain:** Zephyr RTOS / NXP RW61x embedded toolchain setup
**Confidence:** MEDIUM-HIGH

---

<user_constraints>
## User Constraints (from CONTEXT.md)

### Locked Decisions

#### 工程结构与 West 管理
- 使用 T-topology 方式管理 nxp_zsdk 依赖：项目根目录放 west.yml，引用 nxp_zsdk 作为外部 manifest
- 模块化分层目录结构：app/ 放应用代码，boards/ 放板级配置，drivers/ 放自定义驱动
- Kconfig 使用多文件 overlay 组合方式：拆分为 wifi.conf、ble.conf、debug.conf 等可组合片段
- nxp_zsdk 版本管理：跟踪 nxp-v4.3-branch 分支头，随时获取更新

#### 调试器与烧录
- 调试探针：使用 J-Link
- 烧录方式：通过 J-Link 直接烧录
- VS Code 集成：完整配置 launch.json + tasks.json，实现一键编译烧录调试

#### 硬件开发板
- 开发板：NXP 官方 RW612 EVK (FRDM-RW612 或 EVK-RW612)
- 串口日志：使用板载 USB 转串口
- Device Tree：使用 nxp_zsdk 自带的默认配置 + overlay 文件做小调整

#### 日志系统
- 默认日志级别：INFO，开发时可切换到 DEBUG
- 日志后端：串口后端 + Flash 存储后端
- 日志格式：带时间戳、模块名、级别的结构化格式

### Claude's Discretion
- CMakeLists.txt 的具体组织方式和构建目标配置
- west.yml 中具体的 module 引用和版本号
- VS Code 扩展推荐列表
- Flash 日志存储的具体分区大小和轮转策略
- 编译优化级别选择

### Deferred Ideas (OUT OF SCOPE)
None — discussion stayed within phase scope
</user_constraints>

---

<phase_requirements>
## Phase Requirements

| ID | Description | Research Support |
|----|-------------|-----------------|
| TOOL-01 | 基于 nxp_zsdk (nxp-v4.3-branch) 创建可编译的 RW61x 工程 | west init + west update workflow; nxp_zsdk README; board targets frdm_rw612 / rw612_evk |
| TOOL-02 | 配置 VS Code + west 开发环境并验证可用性 | VS Code tasks.json + launch.json patterns; cortex-debug extension; west build/flash/debug commands |
| TOOL-03 | 配置设备树（Device Tree）支持 RW61x 硬件 | DTS overlay pattern; chosen node for console; RW612 Flexcomm3 UART at 115200 8N1 |
| TOOL-04 | 验证编译、烧录、运行基本流程 | west build -b frdm_rw612; west flash --runner jlink; west debug --runner jlink |
| TOOL-05 | 集成 JTAG/SWD 硬件调试器支持（pyOCD 或 J-Link） | J-Link runner confirmed; west flash/debug --runner jlink; BOARD_FLASH_RUNNER cmake var |
| SYS-01 | 集成 Zephyr 日志系统（串口输出） | CONFIG_LOG=y; CONFIG_LOG_BACKEND_UART=y; LOG_MODULE_REGISTER; LOG_INF/DBG/WRN/ERR macros |
| DBG-01 | 配置串口日志输出 | Flexcomm3 at 115200 8N1 is default console; CONFIG_UART_CONSOLE=y; CONFIG_LOG_BACKEND_UART=y |
| DBG-02 | 验证 JTAG/SWD 断点调试 | west debug --runner jlink; cortex-debug VS Code extension; launch.json with jlink servertype |
</phase_requirements>

---

## Summary

Phase 1 establishes the complete build/debug/log environment for the RW61x on top of NXP's `nxp_zsdk` (nxp-v4.3-branch). The SDK is NXP's manifest repository wrapping upstream Zephyr v4.3 with NXP-specific board support and HAL modules. The workspace is initialized with `west init -m https://github.com/nxp-zephyr/nxp_zsdk --mr nxp-v4.3-branch` followed by `west update`. A critical early step is fetching the radio firmware blobs via `west blobs fetch hal_nxp` — without this, WiFi/BLE will not function in later phases.

The project uses a T-topology west workspace: the application repo lives at the workspace root and contains its own `west.yml` that imports `nxp_zsdk` as the manifest source. This is the standard Zephyr pattern for downstream application repos. The RW612 board targets are `frdm_rw612` (FRDM-RW612 board) and `rw612_evk` (EVK-RW612). The Zephyr console is pre-assigned to Flexcomm3 at 115200 8N1 in the default board DTS — no overlay is needed for basic UART logging.

J-Link is the chosen debugger. Zephyr's west build system supports J-Link natively via `--runner jlink`. VS Code integration uses the `cortex-debug` extension with a `launch.json` pointing to the J-Link GDB server. The logging subsystem is straightforward: `CONFIG_LOG=y` + `CONFIG_LOG_BACKEND_UART=y` in `prj.conf`, then `LOG_MODULE_REGISTER` + `LOG_INF/DBG/WRN/ERR` macros in source.

**Primary recommendation:** Initialize the workspace with `west init` pointing to `nxp_zsdk nxp-v4.3-branch`, fetch blobs immediately, build `hello_world` for `frdm_rw612` as the first smoke test, then layer in logging and J-Link debug config.

---

## Standard Stack

### Core

| Component | Version | Purpose | Why Standard |
|-----------|---------|---------|--------------|
| nxp_zsdk | nxp-v4.3-branch | NXP manifest repo wrapping Zephyr v4.3 + NXP HAL | Official NXP Zephyr distribution for RW61x |
| west | bundled with Zephyr | Workspace manager, build/flash/debug orchestration | Zephyr's official meta-tool; required |
| Zephyr SDK (toolchain) | 0.16.x+ | ARM Cortex-M33 cross-compiler (arm-zephyr-eabi) | Official Zephyr toolchain; simplest setup |
| hal_nxp | pulled by west update | NXP HAL + RW61x radio firmware blobs | Required for RW61x peripheral drivers |
| CMake | 3.20.0+ | Build system | Zephyr requires CMake 3.20+ |

### Supporting

| Component | Version | Purpose | When to Use |
|-----------|---------|---------|-------------|
| cortex-debug (VS Code ext) | latest | GDB-based debug UI in VS Code | Required for VS Code J-Link debugging |
| J-Link Software Pack | latest | J-Link GDB server (JLinkGDBServerCL) | Required when using J-Link probe |
| Zephyr logging subsystem | built-in | Structured log output with levels/modules | Always — replaces raw printk for production code |
| Device Tree overlay (.overlay) | built-in | Board hardware customization | When default board DTS needs adjustment |

### Alternatives Considered

| Instead of | Could Use | Tradeoff |
|------------|-----------|----------|
| J-Link | pyOCD | pyOCD is open-source and macOS-friendly but has less mature RW61x support; J-Link is the locked decision |
| J-Link | OpenOCD | OpenOCD support for RW61x/Cortex-M33 is less tested; J-Link is more reliable for NXP targets |
| Zephyr SDK toolchain | Homebrew arm-none-eabi-gcc | Zephyr SDK is version-matched and tested; third-party toolchains can cause subtle build failures |
| cortex-debug | nRF Connect for VS Code | nRF Connect is Nordic-specific; cortex-debug is vendor-neutral and works with J-Link |

**Installation:**
```bash
# 1. Install Zephyr dependencies (macOS)
brew install cmake ninja gperf python3 ccache qemu dtc wget

# 2. Install west
pip3 install west

# 3. Initialize workspace (T-topology: run from workspace root, NOT inside app repo)
west init -m https://github.com/nxp-zephyr/nxp_zsdk --mr nxp-v4.3-branch
west update

# 4. Install Zephyr SDK (ARM toolchain)
# Download from https://github.com/zephyrproject-rtos/sdk-ng/releases
# Install to ~/zephyr-sdk-0.16.x

# 5. Fetch RW61x radio firmware blobs (CRITICAL - do this early)
west blobs fetch hal_nxp
```

---

## Architecture Patterns

### Recommended Project Structure

```
workspace/                        # west workspace root
├── west.yml                      # T-topology: app manifest importing nxp_zsdk
├── nxp_zsdk/                     # cloned by west init (manifest repo)
├── zephyr/                       # cloned by west update
├── modules/                      # cloned by west update (hal_nxp, etc.)
└── app/                          # your application repo (self: path: app)
    ├── CMakeLists.txt
    ├── prj.conf                  # base Kconfig (always loaded)
    ├── boards/
    │   └── frdm_rw612.overlay   # board-specific DTS adjustments
    ├── conf/
    │   ├── wifi.conf             # WiFi Kconfig fragment
    │   ├── ble.conf              # BLE Kconfig fragment
    │   └── debug.conf            # Debug/logging Kconfig fragment
    ├── src/
    │   └── main.c
    └── .vscode/
        ├── launch.json           # J-Link debug config
        ├── tasks.json            # west build/flash tasks
        └── extensions.json       # recommended extensions
```

### Pattern 1: T-Topology west.yml (Application as Manifest)

**What:** The application repo contains `west.yml` at workspace root. It imports `nxp_zsdk` which in turn imports Zephyr and all NXP modules. The application repo is listed under `self:`.

**When to use:** Always — this is the locked decision and the standard Zephyr pattern for downstream application repos.

**Example:**
```yaml
# workspace/west.yml  (T-topology: app repo IS the manifest repo)
# Source: Zephyr docs workspaces.rst + nxp_zsdk README
manifest:
  remotes:
    - name: nxp-zephyr
      url-base: https://github.com/nxp-zephyr
  projects:
    - name: nxp_zsdk
      remote: nxp-zephyr
      revision: nxp-v4.3-branch
      import: true          # imports nxp_zsdk's west.yml (pulls zephyr, hal_nxp, etc.)
  self:
    path: app               # this repo lives at workspace/app/
```

### Pattern 2: Minimal CMakeLists.txt

**What:** The minimal CMake entry point that hooks into Zephyr's build system.

**When to use:** Every Zephyr application.

**Example:**
```cmake
# Source: https://docs.zephyrproject.org/latest/develop/application/index.html
cmake_minimum_required(VERSION 3.20.0)
find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(rw61x_template)
target_sources(app PRIVATE src/main.c)
```

### Pattern 3: Composable Kconfig Fragments

**What:** Split Kconfig into a base `prj.conf` plus feature-specific fragments. Pass fragments at build time with `EXTRA_CONF_FILE`.

**When to use:** When building with different feature combinations (WiFi only, BLE only, both, debug mode).

**Example:**
```bash
# Build with debug logging enabled
west build -b frdm_rw612 -- -DEXTRA_CONF_FILE="conf/debug.conf"

# Build with multiple fragments
west build -b frdm_rw612 -- -DEXTRA_CONF_FILE="conf/wifi.conf;conf/debug.conf"
```

```ini
# conf/debug.conf
CONFIG_LOG=y
CONFIG_LOG_DEFAULT_LEVEL=4
CONFIG_LOG_BACKEND_UART=y
CONFIG_LOG_PROCESS_THREAD_SLEEP_MS=10
```

### Pattern 4: Device Tree Overlay for Board Customization

**What:** A `.overlay` file placed in the app root (or `boards/` directory) that patches the board's default DTS without modifying the SDK.

**When to use:** When the default board DTS needs adjustment (e.g., changing console UART, enabling additional peripherals).

**Example:**
```dts
/* boards/frdm_rw612.overlay */
/* Source: Zephyr DTS overlay pattern from official docs */

/* RW612 default console is Flexcomm3 at 115200 8N1 — usually no change needed */
/* Override only if you need a different UART for console */
/ {
    chosen {
        zephyr,console = &flexcomm3;
        zephyr,shell-uart = &flexcomm3;
    };
};
```

### Pattern 5: Logging Module Registration

**What:** Per-module logging registration with configurable log level.

**When to use:** In every source file that emits log messages.

**Example:**
```c
/* Source: https://github.com/zephyrproject-rtos/zephyr (Context7) */
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(main, CONFIG_MAIN_LOG_LEVEL);

int main(void)
{
    LOG_INF("RW61x template started");
    LOG_DBG("Debug info: build %s", CONFIG_BOARD);
    return 0;
}
```

### Pattern 6: VS Code J-Link Debug Configuration

**What:** `launch.json` using `cortex-debug` extension with J-Link GDB server.

**When to use:** VS Code debugging with J-Link probe (locked decision).

**Example:**
```json
// .vscode/launch.json
// Source: cortex-debug extension docs + Zephyr community patterns
{
    "version": "0.2.0",
    "configurations": [
        {
            "name": "Debug (J-Link)",
            "type": "cortex-debug",
            "request": "launch",
            "servertype": "jlink",
            "device": "RW612",
            "interface": "swd",
            "svdFile": "${workspaceFolder}/../zephyr/dts/arm/nxp/rw612.svd",
            "executable": "${workspaceFolder}/build/zephyr/zephyr.elf",
            "preLaunchTask": "west build",
            "runToEntryPoint": "main",
            "showDevDebugOutput": "none"
        }
    ]
}
```

```json
// .vscode/tasks.json
{
    "version": "2.0.0",
    "tasks": [
        {
            "label": "west build",
            "type": "shell",
            "command": "west build -b frdm_rw612",
            "group": { "kind": "build", "isDefault": true },
            "problemMatcher": ["$gcc"]
        },
        {
            "label": "west flash",
            "type": "shell",
            "command": "west flash --runner jlink",
            "dependsOn": "west build"
        }
    ]
}
```

### Anti-Patterns to Avoid

- **Running `west init` inside the app repo directory:** T-topology requires `west init` at the workspace root, not inside the app. The app repo is a subdirectory of the workspace.
- **Modifying nxp_zsdk files directly:** All customizations go in overlay files and Kconfig fragments in the app repo. SDK files will be overwritten on `west update`.
- **Using `printk` for production logging:** Use the Zephyr logging subsystem (`LOG_INF`, etc.) — it supports levels, module filtering, and multiple backends.
- **Forgetting `west blobs fetch hal_nxp`:** The RW61x radio firmware blobs are not in the git repo. Without them, WiFi/BLE will silently fail in later phases.
- **Hardcoding `ZEPHYR_BASE` in CMakeLists.txt:** Use `$ENV{ZEPHYR_BASE}` or rely on `west build` to set it automatically.

---

## Don't Hand-Roll

| Problem | Don't Build | Use Instead | Why |
|---------|-------------|-------------|-----|
| Structured logging with levels | Custom printf wrapper | Zephyr logging subsystem (`CONFIG_LOG`) | Built-in level filtering, multiple backends, timestamps, module names, deferred logging |
| Flash log storage | Custom flash write routines | Zephyr logging flash backend (`CONFIG_LOG_BACKEND_FS` or `CONFIG_LOG_BACKEND_FLASH`) | Handles wear leveling, circular buffer, crash-safe writes |
| UART console output | Custom UART driver | `CONFIG_UART_CONSOLE=y` + Flexcomm3 default | Board DTS already configures Flexcomm3 as console UART |
| Build system integration | Custom Makefile | `west build` + CMake `find_package(Zephyr)` | west handles toolchain, board config, DTS compilation, Kconfig merge |
| Debug probe flashing | Custom flash scripts | `west flash --runner jlink` | west abstracts runner selection; handles ELF → flash translation |

**Key insight:** Zephyr's subsystems (logging, console, build) are mature and handle the hard parts. The value is in configuration, not custom implementation.

---

## Common Pitfalls

### Pitfall 1: west init Location (T-Topology)

**What goes wrong:** Developer runs `west init` inside the app repo, creating a nested workspace. West commands fail or produce unexpected behavior.

**Why it happens:** T-topology requires the workspace root to be the parent of the app repo. The `west.yml` at workspace root imports nxp_zsdk; the app repo is listed as `self: path: app`.

**How to avoid:** Create an empty workspace directory, run `west init` there (or use `west init -m <url>` which creates the workspace), then place the app repo at `workspace/app/`.

**Warning signs:** `west build` can't find `ZEPHYR_BASE`; `west list` shows unexpected project paths.

### Pitfall 2: Missing hal_nxp Firmware Blobs

**What goes wrong:** WiFi and BLE initialization fails at runtime with no obvious error. Build succeeds but radio subsystems don't start.

**Why it happens:** RW61x radio firmware blobs are binary files not stored in git. They must be fetched separately after `west update`.

**How to avoid:** Run `west blobs fetch hal_nxp` immediately after `west update`. Add this to onboarding documentation and VS Code tasks.

**Warning signs:** WiFi/BLE init returns error codes; `west blobs list hal_nxp` shows blobs as missing.

### Pitfall 3: Board Target Name Confusion

**What goes wrong:** `west build -b rw612_evk` fails or produces wrong binary for FRDM-RW612 board.

**Why it happens:** nxp_zsdk has multiple RW612 board targets: `frdm_rw612` (FRDM-RW612), `rw612_evk` (EVK-RW612), `rw612/rd_rw612_bga` (RD reference design). They have different DTS and pin assignments.

**How to avoid:** Confirm physical board model. For FRDM-RW612 use `frdm_rw612`. For EVK-RW612 use `rw612_evk`. Document the board target in the project README.

**Warning signs:** UART output missing; peripherals not responding; wrong pin assignments.

### Pitfall 4: Kconfig Fragment Not Applied

**What goes wrong:** Features configured in `wifi.conf` or `debug.conf` are not active in the build.

**Why it happens:** Kconfig fragments must be explicitly passed via `EXTRA_CONF_FILE` or listed in `CMakeLists.txt`. They are not auto-discovered.

**How to avoid:** Either pass via `west build -- -DEXTRA_CONF_FILE=conf/debug.conf` or add to `CMakeLists.txt`:
```cmake
set(EXTRA_CONF_FILE "conf/debug.conf")
```

**Warning signs:** `west build` succeeds but expected CONFIG options are not set; check `build/zephyr/.config` to verify.

### Pitfall 5: J-Link Device Name for RW612

**What goes wrong:** `west flash --runner jlink` or cortex-debug fails with "device not found" or "unsupported device".

**Why it happens:** J-Link requires the correct device name string for RW612. The exact string may differ between J-Link software versions.

**How to avoid:** Use `JLinkExe -device ?` to list supported devices and find the correct RW612 identifier. Common values: `RW612`, `MIMXRW612`. Set in `launch.json` `"device"` field and in `west flash` via `--dev-id` or board.cmake.

**Warning signs:** J-Link GDB server starts but fails to connect; "Could not find core" errors.

### Pitfall 6: macOS Zephyr SDK Path

**What goes wrong:** `west build` fails with "toolchain not found" on macOS.

**Why it happens:** Zephyr SDK must be installed and `ZEPHYR_SDK_INSTALL_DIR` or `ZEPHYR_TOOLCHAIN_VARIANT` must be set, or the SDK must be in a standard location (`~/zephyr-sdk-*`).

**How to avoid:** Install Zephyr SDK to `~/zephyr-sdk-0.16.x`. Run `cmake -P ~/zephyr-sdk-0.16.x/setup.cmake` to register it. Alternatively set `ZEPHYR_SDK_INSTALL_DIR` in shell profile.

**Warning signs:** CMake error "Unable to find Zephyr toolchain"; `west build` exits immediately.

---

## Code Examples

Verified patterns from official sources:

### Minimal Application (main.c)
```c
/* Source: Zephyr official docs + Context7 /zephyrproject-rtos/zephyr */
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(main, LOG_LEVEL_INF);

int main(void)
{
    LOG_INF("RW61x Zephyr template v1.0");
    LOG_INF("Board: %s", CONFIG_BOARD);
    return 0;
}
```

### Base prj.conf
```ini
# Source: Zephyr logging docs + Context7 /zephyrproject-rtos/zephyr
# Base configuration — always loaded
CONFIG_CONSOLE=y
CONFIG_UART_CONSOLE=y
CONFIG_LOG=y
CONFIG_LOG_DEFAULT_LEVEL=3
CONFIG_LOG_BACKEND_UART=y
CONFIG_LOG_PROCESS_THREAD_SLEEP_MS=10
```

### conf/debug.conf (Kconfig fragment)
```ini
# Debug fragment — loaded with -DEXTRA_CONF_FILE=conf/debug.conf
CONFIG_LOG_DEFAULT_LEVEL=4
CONFIG_LOG_BACKEND_UART=y
CONFIG_ASSERT=y
CONFIG_DEBUG=y
```

### west build commands
```bash
# Source: nxp_zsdk README + Zephyr docs
# Basic build
west build -b frdm_rw612 app/

# Build with debug fragment
west build -b frdm_rw612 app/ -- -DEXTRA_CONF_FILE=conf/debug.conf

# Flash via J-Link
west flash --runner jlink

# Debug via J-Link (opens GDB server)
west debug --runner jlink

# Fetch firmware blobs (run once after west update)
west blobs fetch hal_nxp
```

### west.yml (T-topology, application as manifest)
```yaml
# Source: Zephyr workspaces.rst (Context7) + nxp_zsdk README
manifest:
  remotes:
    - name: nxp-zephyr
      url-base: https://github.com/nxp-zephyr
  projects:
    - name: nxp_zsdk
      remote: nxp-zephyr
      revision: nxp-v4.3-branch
      import: true
  self:
    path: app
```

---

## State of the Art

| Old Approach | Current Approach | When Changed | Impact |
|--------------|------------------|--------------|--------|
| `void main()` | `int main(void)` | Zephyr 3.1+ | Compiler warnings on old signature; use `int` |
| `#include <kernel.h>` | `#include <zephyr/kernel.h>` | Zephyr 3.0 | Old path still works but deprecated; use namespaced path |
| `west init` with Zephyr directly | `west init` with nxp_zsdk manifest | NXP SDK adoption | nxp_zsdk pulls correct Zephyr version + NXP patches |
| MCUXpresso IDE | VS Code + west | Ongoing | Project explicitly excludes MCUXpresso (out of scope) |
| pyOCD as default runner | J-Link as runner | Per project decision | J-Link is locked decision; use `--runner jlink` |

**Deprecated/outdated:**
- `CONFIG_UART_CONSOLE_ON_DEV_NAME`: Replaced by DTS `chosen` node `zephyr,console` property
- `zephyr_library()` in CMakeLists.txt: Use `target_sources(app PRIVATE ...)` for application code

---

## Open Questions

1. **Exact J-Link device string for RW612**
   - What we know: J-Link supports NXP Cortex-M33 targets; `west flash --runner jlink` is the command
   - What's unclear: The exact `--device` string (e.g., `RW612` vs `MIMXRW612`) needed in `launch.json` and board.cmake
   - Recommendation: Run `JLinkExe -device ?` on the target machine to confirm; check nxp_zsdk `boards/nxp/frdm_rw612/board.cmake` for the default runner config

2. **Flash logging backend partition for RW612**
   - What we know: User wants serial + flash log backends; Zephyr has `CONFIG_LOG_BACKEND_FS`
   - What's unclear: RW612 flash partition layout in nxp_zsdk; whether a dedicated log partition is pre-defined
   - Recommendation: Defer flash log backend to after basic UART logging is verified; check `boards/nxp/frdm_rw612/frdm_rw612.dts` for flash partition map

3. **SVD file availability for RW612 in cortex-debug**
   - What we know: cortex-debug uses SVD files for peripheral register view
   - What's unclear: Whether nxp_zsdk ships an RW612 SVD file or if it must be downloaded from NXP separately
   - Recommendation: Check `zephyr/dts/arm/nxp/` for SVD; if absent, download from NXP product page

4. **nxp_zsdk west.yml module list**
   - What we know: `import: true` on nxp_zsdk pulls all its declared modules including hal_nxp, zephyr, etc.
   - What's unclear: Full list of modules pulled; whether any conflict with standard Zephyr modules
   - Recommendation: After `west update`, run `west list` to see all projects; verify hal_nxp is present before attempting blob fetch

---

## Sources

### Primary (HIGH confidence)
- `/zephyrproject-rtos/zephyr` (Context7) — logging subsystem, west workspace topologies, CMakeLists.txt patterns, DTS overlay patterns, J-Link runner commands
- `https://github.com/nxp-zephyr/nxp_zsdk/blob/nxp-v4.3-branch/README.md` — west init command, board targets, blob fetch command, supported boards
- `https://docs.zephyrproject.org/latest/boards/nxp/frdm_rw612/doc/index.html` — FRDM-RW612 board target, Flexcomm3 console at 115200 8N1, west build/flash/debug commands
- `https://docs.zephyrproject.org/latest/boards/nxp/rw612/doc/index.html` — RW612 SoC overview, rw612/rd_rw612_bga board target

### Secondary (MEDIUM confidence)
- `https://docs.zephyrproject.org/latest/develop/application/index.html` — minimal CMakeLists.txt, prj.conf structure (verified via WebSearch result content)
- `https://docs.zephyrproject.org/latest/services/logging/index.html` — CONFIG_LOG options, log level values (verified via WebSearch result content)

### Tertiary (LOW confidence)
- NXP Community threads on J-Link + RW612 debugging — community reports of J-Link config friction; exact device string unverified
- cortex-debug launch.json pattern — based on training knowledge + community patterns; needs validation against actual J-Link + RW612 setup

---

## Metadata

**Confidence breakdown:**
- Standard stack: HIGH — nxp_zsdk README and Zephyr official docs confirm all core components
- Architecture: HIGH — west workspace topology patterns verified via Context7 Zephyr docs; T-topology west.yml pattern is well-documented
- Logging config: HIGH — CONFIG_LOG options verified via Context7 and official Zephyr logging docs
- J-Link integration: MEDIUM — west --runner jlink confirmed; exact RW612 device string for launch.json is LOW confidence
- DTS overlay: MEDIUM — general overlay pattern is HIGH confidence; RW612-specific Flexcomm3 details from board docs are MEDIUM

**Research date:** 2026-02-28
**Valid until:** 2026-03-28 (nxp-v4.3-branch is a moving branch; re-verify if significant time passes)
