# RW61x Zephyr 工程模版

## What This Is

基于 NXP nxp_zsdk (nxp-v4.3-branch) 为 RW61x 芯片开发的 Zephyr RTOS 工程模版，支持 WiFi/BLE 双模无线功能。这是一个可复用的基础框架，用于验证 SDK 能力并为后续 IoT 设备固件项目提供起点。

## Core Value

验证 nxp_zsdk 对 RW61x 的完整支持程度，建立一套可靠的 Zephyr 开发工具链和工作流程，确保后续 IoT 项目能够快速启动。

## Requirements

### Validated

(None yet — ship to validate)

### Active

- [ ] 基于 nxp_zsdk (nxp-v4.3-branch) 创建可编译的 RW61x 工程
- [ ] 配置 VS Code + west 开发环境
- [ ] 实现 WiFi 基础功能（连接 AP、网络管理）
- [ ] 实现 BLE 基础功能（广播、连接、GATT 服务）
- [ ] 验证 WiFi/BLE 共存模式
- [ ] 集成通用固件功能（OTA、配置管理、日志系统）
- [ ] 支持多种调试方式（JTAG/SWD、串口日志、网络调试）
- [ ] 编写完整的开发文档和示例代码
- [ ] 验证完整工具链（编译、烧录、调试、部署）

### Out of Scope

- 特定 IoT 应用业务逻辑 — 这是通用模版，不针对具体产品
- MCUXpresso IDE 支持 — 专注于 VS Code + west 工具链
- 生产级性能优化 — 重点是功能验证和工具链建立
- 多板卡支持 — 仅针对 RW61x 平台

## Context

**技术背景：**
- 目标芯片：NXP RW61x（支持 WiFi 6 + BLE 5.3）
- SDK 来源：https://github.com/nxp-zephyr/nxp-zsdk/tree/nxp-v4.3-branch
- RTOS：Zephyr（NXP 下游适配版本）
- 开发环境：macOS (Darwin 23.4.0)

**不确定性：**
- nxp_zsdk 对 RW61x 的 WiFi/BLE 驱动完整性未知
- NXP 对 Zephyr 的硬件外设适配程度未知
- 文档和示例的完整性未知
- 需要通过实际验证来确认 SDK 能力

**目标应用场景：**
- IoT 设备固件开发（智能家居、传感器等联网设备）
- 需要 WiFi/BLE 双模通信能力
- 需要 Zephyr RTOS 的完整特性支持

## Constraints

- **SDK 版本**: nxp_zsdk nxp-v4.3-branch — NXP 官方维护的 Zephyr 下游分支
- **硬件平台**: RW61x 芯片 — 必须使用此平台进行验证
- **开发工具**: VS Code + west + CMake — 统一的开发环境
- **RTOS**: Zephyr — 不考虑其他 RTOS 方案
- **开发环境**: macOS — 工具链需在 macOS 上可用

## Key Decisions

| Decision | Rationale | Outcome |
|----------|-----------|---------|
| 使用 nxp_zsdk 而非上游 Zephyr | NXP 官方维护，包含 RW61x 专有驱动和优化 | — Pending |
| VS Code + west 工具链 | 跨平台、社区支持好、与 Zephyr 生态集成 | — Pending |
| 优先验证工具链完整性 | 工具链是后续开发的基础，必须先确保可用 | — Pending |
| 包含完整文档和示例 | 为后续项目提供参考，降低学习成本 | — Pending |

---
*Last updated: 2026-02-28 after initialization*
