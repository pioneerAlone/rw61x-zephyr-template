# Requirements: RW61x Zephyr 工程模版

**Defined:** 2026-02-28
**Core Value:** 验证 nxp_zsdk 对 RW61x 的完整支持程度，建立一套可靠的 Zephyr 开发工具链和工作流程，确保后续 IoT 项目能够快速启动。

## v1 Requirements

Requirements for initial release. Each maps to roadmap phases.

### 工具链与环境 (Toolchain)

- [ ] **TOOL-01**: 基于 nxp_zsdk (nxp-v4.3-branch) 创建可编译的 RW61x 工程
- [ ] **TOOL-02**: 配置 VS Code + west 开发环境并验证可用性
- [ ] **TOOL-03**: 配置设备树（Device Tree）支持 RW61x 硬件
- [ ] **TOOL-04**: 验证编译、烧录、运行基本流程
- [ ] **TOOL-05**: 集成 JTAG/SWD 硬件调试器支持（pyOCD 或 J-Link）

### WiFi 功能 (WiFi)

- [ ] **WIFI-01**: 实现 WiFi Station 模式连接 AP
- [ ] **WIFI-02**: 支持 WPA2/WPA3 安全认证
- [ ] **WIFI-03**: 实现 DHCP 客户端自动获取 IP
- [ ] **WIFI-04**: 实现网络连接管理（连接、断开、重连）
- [ ] **WIFI-05**: 验证 WiFi 固件 blob 正确加载

### BLE 功能 (BLE)

- [ ] **BLE-01**: 实现 BLE 外设角色（Peripheral）
- [ ] **BLE-02**: 实现 BLE 广播功能
- [ ] **BLE-03**: 实现 GATT 服务定义和注册
- [ ] **BLE-04**: 支持 BLE 连接和断开
- [ ] **BLE-05**: 实现 GATT 特征读写回调

### WiFi/BLE 共存 (Coexistence)

- [ ] **COEX-01**: 验证 WiFi 和 BLE 同时工作
- [ ] **COEX-02**: 测试 WiFi 连接时 BLE 广播稳定性
- [ ] **COEX-03**: 测试 BLE 连接时 WiFi 数据传输稳定性
- [ ] **COEX-04**: 配置多射频仲裁机制

### 系统功能 (System)

- [ ] **SYS-01**: 集成 Zephyr 日志系统（串口输出）
- [ ] **SYS-02**: 实现持久化配置存储（NVS）
- [ ] **SYS-03**: 集成 Shell/CLI 接口用于调试
- [ ] **SYS-04**: 实现看门狗定时器
- [ ] **SYS-05**: 配置线程栈大小（WiFi/BLE 需 2-4KB）

### OTA 更新 (OTA)

- [ ] **OTA-01**: 集成 MCUboot 安全启动
- [ ] **OTA-02**: 配置双 bank flash 分区布局
- [ ] **OTA-03**: 实现 OTA 固件更新机制
- [ ] **OTA-04**: 验证 OTA 更新流程（下载、验证、重启）

### 安全功能 (Security)

- [ ] **SEC-01**: 集成 mbedTLS 支持
- [ ] **SEC-02**: 验证 TLS 加密通信
- [ ] **SEC-03**: 配置 EdgeLock Secure Enclave（如果 SDK 支持）

### 调试与诊断 (Debug)

- [ ] **DBG-01**: 配置串口日志输出
- [ ] **DBG-02**: 验证 JTAG/SWD 断点调试
- [ ] **DBG-03**: 实现网络日志输出（可选）
- [ ] **DBG-04**: 集成性能分析工具（线程统计、内存使用）

### 文档与示例 (Documentation)

- [ ] **DOC-01**: 编写工具链安装和配置文档
- [ ] **DOC-02**: 编写 WiFi 连接示例和说明
- [ ] **DOC-03**: 编写 BLE 服务示例和说明
- [ ] **DOC-04**: 编写 OTA 更新操作指南
- [ ] **DOC-05**: 编写常见问题排查文档
- [ ] **DOC-06**: 提供完整的示例代码和注释

## v2 Requirements

Deferred to future release. Tracked but not in current roadmap.

### 高级 WiFi 功能

- **WIFI-06**: WiFi AP 模式支持
- **WIFI-07**: WiFi AP + Station 并发模式
- **WIFI-08**: WiFi 6 高级特性（TWT、OFDMA）

### 高级 BLE 功能

- **BLE-06**: BLE 中心角色（Central）
- **BLE-07**: BLE 长距离模式
- **BLE-08**: BLE 多连接支持

### 高级系统功能

- **SYS-06**: 电源管理（低功耗模式）
- **SYS-07**: 深度睡眠和快速唤醒
- **SYS-08**: 网络协议栈（MQTT、CoAP、HTTP）

### Matter 协议

- **MATT-01**: Matter over WiFi 支持
- **MATT-02**: Matter over Thread 支持
- **MATT-03**: Matter 设备认证

## Out of Scope

Explicitly excluded. Documented to prevent scope creep.

| Feature | Reason |
|---------|--------|
| MCUXpresso IDE 支持 | 专注于 VS Code + west 工具链，避免多工具链维护成本 |
| 特定 IoT 应用业务逻辑 | 这是通用模版，不针对具体产品功能 |
| 生产级性能优化 | 重点是功能验证和工具链建立，性能优化留给具体项目 |
| 多板卡支持 | 仅针对 RW61x 平台，避免复杂度 |
| 实时视频流 | RW61x 硬件不适合，缺少视频加速，RAM 受限 |
| Linux 兼容性 | Cortex-M33 无 MMU，Zephyr RTOS 是正确选择 |
| 自定义加密算法 | 使用标准 TLS/DTLS 和硬件加密，避免安全风险 |

## Traceability

Which phases cover which requirements. Updated during roadmap creation.

| Requirement | Phase | Status |
|-------------|-------|--------|
| TOOL-01 | Phase 1: Toolchain Foundation | Pending |
| TOOL-02 | Phase 1: Toolchain Foundation | Pending |
| TOOL-03 | Phase 1: Toolchain Foundation | Pending |
| TOOL-04 | Phase 1: Toolchain Foundation | Pending |
| TOOL-05 | Phase 1: Toolchain Foundation | Pending |
| SYS-01 | Phase 1: Toolchain Foundation | Pending |
| DBG-01 | Phase 1: Toolchain Foundation | Pending |
| DBG-02 | Phase 1: Toolchain Foundation | Pending |
| WIFI-01 | Phase 2: WiFi Connectivity | Pending |
| WIFI-02 | Phase 2: WiFi Connectivity | Pending |
| WIFI-03 | Phase 2: WiFi Connectivity | Pending |
| WIFI-04 | Phase 2: WiFi Connectivity | Pending |
| WIFI-05 | Phase 2: WiFi Connectivity | Pending |
| SYS-03 | Phase 2: WiFi Connectivity | Pending |
| SYS-05 | Phase 2: WiFi Connectivity | Pending |
| SEC-01 | Phase 2: WiFi Connectivity | Pending |
| SEC-02 | Phase 2: WiFi Connectivity | Pending |
| BLE-01 | Phase 3: BLE Functionality | Pending |
| BLE-02 | Phase 3: BLE Functionality | Pending |
| BLE-03 | Phase 3: BLE Functionality | Pending |
| BLE-04 | Phase 3: BLE Functionality | Pending |
| BLE-05 | Phase 3: BLE Functionality | Pending |
| COEX-01 | Phase 4: WiFi/BLE Coexistence | Pending |
| COEX-02 | Phase 4: WiFi/BLE Coexistence | Pending |
| COEX-03 | Phase 4: WiFi/BLE Coexistence | Pending |
| COEX-04 | Phase 4: WiFi/BLE Coexistence | Pending |
| DBG-04 | Phase 4: WiFi/BLE Coexistence | Pending |
| SYS-02 | Phase 5: System Features & OTA | Pending |
| SYS-04 | Phase 5: System Features & OTA | Pending |
| OTA-01 | Phase 5: System Features & OTA | Pending |
| OTA-02 | Phase 5: System Features & OTA | Pending |
| OTA-03 | Phase 5: System Features & OTA | Pending |
| OTA-04 | Phase 5: System Features & OTA | Pending |
| SEC-03 | Phase 5: System Features & OTA | Pending |
| DBG-03 | Phase 5: System Features & OTA | Pending |
| DOC-01 | Phase 6: Documentation & Examples | Pending |
| DOC-02 | Phase 6: Documentation & Examples | Pending |
| DOC-03 | Phase 6: Documentation & Examples | Pending |
| DOC-04 | Phase 6: Documentation & Examples | Pending |
| DOC-05 | Phase 6: Documentation & Examples | Pending |
| DOC-06 | Phase 6: Documentation & Examples | Pending |

**Coverage:**
- v1 requirements: 45 total
- Mapped to phases: 45 ✅
- Unmapped: 0

---
*Requirements defined: 2026-02-28*
*Last updated: 2026-02-28 after roadmap creation (100% coverage achieved)*
