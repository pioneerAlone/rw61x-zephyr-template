# Phase 1: Toolchain Foundation - Context

**Gathered:** 2026-02-28
**Status:** Ready for planning

<domain>
## Phase Boundary

建立基于 nxp_zsdk (nxp-v4.3-branch) 的 RW61x 完整开发环境，包括工程创建、编译、烧录、调试和日志输出。验证工具链端到端可用性。

</domain>

<decisions>
## Implementation Decisions

### 工程结构与 West 管理
- 使用 T-topology 方式管理 nxp_zsdk 依赖：项目根目录放 west.yml，引用 nxp_zsdk 作为外部 manifest
- 模块化分层目录结构：app/ 放应用代码，boards/ 放板级配置，drivers/ 放自定义驱动
- Kconfig 使用多文件 overlay 组合方式：拆分为 wifi.conf、ble.conf、debug.conf 等可组合片段
- nxp_zsdk 版本管理：跟踪 nxp-v4.3-branch 分支头，随时获取更新

### 调试器与烧录
- 调试探针：使用 J-Link
- 烧录方式：通过 J-Link 直接烧录
- VS Code 集成：完整配置 launch.json + tasks.json，实现一键编译烧录调试

### 硬件开发板
- 开发板：NXP 官方 RW612 EVK (FRDM-RW612 或 EVK-RW612)
- 串口日志：使用板载 USB 转串口
- Device Tree：使用 nxp_zsdk 自带的默认配置 + overlay 文件做小调整

### 日志系统
- 默认日志级别：INFO，开发时可切换到 DEBUG
- 日志后端：串口后端 + Flash 存储后端
- 日志格式：带时间戳、模块名、级别的结构化格式

### Claude's Discretion
- CMakeLists.txt 的具体组织方式和构建目标配置
- west.yml 中具体的 module 引用和版本号
- VS Code 扩展推荐列表
- Flash 日志存储的具体分区大小和轮转策略
- 编译优化级别选择

</decisions>

<specifics>
## Specific Ideas

- 工程应该能作为模版被快速复制到新项目中使用
- VS Code 配置应该开箱即用，新开发者 clone 后即可开始工作
- 日志系统需要支持按模块过滤，方便调试特定功能
- 编译输出应包含固件大小信息，方便跟踪资源使用

</specifics>

<deferred>
## Deferred Ideas

None — discussion stayed within phase scope

</deferred>

---

*Phase: 01-toolchain-foundation*
*Context gathered: 2026-02-28*
