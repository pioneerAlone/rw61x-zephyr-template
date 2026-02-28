# Roadmap: RW61x Zephyr 工程模版

**Created:** 2026-02-28
**Target:** v1 Release
**Mode:** yolo (ship fast, validate through use)

## Overview

This roadmap delivers a validated RW61x Zephyr firmware template through 6 phases. Each phase builds on the previous, prioritizing toolchain validation before connectivity, then coexistence, and finally production features. The approach follows research findings: establish build/debug infrastructure first, validate WiFi/BLE independently, then test coexistence before adding OTA and security.

**Total v1 Requirements:** 45
**Phases:** 6
**Coverage:** 100%

---

## Phase 1: Toolchain Foundation

**Goal:** Establish working build/debug environment for RW61x

**Duration:** 1-2 days

**Success Criteria:**
- Developer can compile RW61x firmware from VS Code
- Firmware boots on hardware and prints "Hello World" via UART
- JTAG/SWD debugger can set breakpoints and inspect variables
- West workspace correctly manages nxp_zsdk dependencies

**Requirements Covered:**
- **TOOL-01**: 基于 nxp_zsdk (nxp-v4.3-branch) 创建可编译的 RW61x 工程
- **TOOL-02**: 配置 VS Code + west 开发环境并验证可用性
- **TOOL-03**: 配置设备树（Device Tree）支持 RW61x 硬件
- **TOOL-04**: 验证编译、烧录、运行基本流程
- **TOOL-05**: 集成 JTAG/SWD 硬件调试器支持（pyOCD 或 J-Link）
- **SYS-01**: 集成 Zephyr 日志系统（串口输出）
- **DBG-01**: 配置串口日志输出
- **DBG-02**: 验证 JTAG/SWD 断点调试

**Deliverables:**
- Working CMakeLists.txt and prj.conf
- VS Code launch.json for debugging
- Device tree overlay for RW61x board
- Basic logging output via UART

**Risks:**
- NXP SDK branch compatibility issues
- Hardware debugger driver problems on macOS
- Missing firmware blobs in hal_nxp module

---

## Phase 2: WiFi Connectivity

**Goal:** Achieve stable WiFi Station mode with network connectivity

**Duration:** 2-3 days

**Success Criteria:**
- Device connects to WPA2/WPA3 access point automatically
- DHCP client obtains IP address successfully
- Device can ping external hosts (e.g., 8.8.8.8)
- WiFi reconnects automatically after AP reboot

**Requirements Covered:**
- **WIFI-01**: 实现 WiFi Station 模式连接 AP
- **WIFI-02**: 支持 WPA2/WPA3 安全认证
- **WIFI-03**: 实现 DHCP 客户端自动获取 IP
- **WIFI-04**: 实现网络连接管理（连接、断开、重连）
- **WIFI-05**: 验证 WiFi 固件 blob 正确加载
- **SYS-03**: 集成 Shell/CLI 接口用于调试
- **SYS-05**: 配置线程栈大小（WiFi/BLE 需 2-4KB）
- **SEC-01**: 集成 mbedTLS 支持
- **SEC-02**: 验证 TLS 加密通信

**Deliverables:**
- WiFi connection manager implementation
- Shell commands for WiFi control (connect, disconnect, status)
- Network configuration via Kconfig
- TLS test client (HTTPS request)

**Risks:**
- WiFi firmware blob loading failures
- Thread stack overflow in network operations
- WPA3 support incomplete in nxp_zsdk

---

## Phase 3: BLE Functionality

**Goal:** Implement BLE peripheral with GATT services

**Duration:** 2-3 days

**Success Criteria:**
- Device advertises BLE services visible to smartphone apps
- Smartphone can connect and read/write GATT characteristics
- BLE connection remains stable for 5+ minutes
- Device handles connect/disconnect cycles gracefully

**Requirements Covered:**
- **BLE-01**: 实现 BLE 外设角色（Peripheral）
- **BLE-02**: 实现 BLE 广播功能
- **BLE-03**: 实现 GATT 服务定义和注册
- **BLE-04**: 支持 BLE 连接和断开
- **BLE-05**: 实现 GATT 特征读写回调

**Deliverables:**
- BLE peripheral initialization code
- Custom GATT service definition
- Advertising data configuration
- Connection event handlers

**Risks:**
- BLE stack configuration complexity
- GATT service UUID conflicts
- Connection parameter negotiation issues

---

## Phase 4: WiFi/BLE Coexistence

**Goal:** Validate simultaneous WiFi and BLE operation

**Duration:** 2-3 days

**Success Criteria:**
- WiFi maintains connection while BLE advertises
- BLE connection succeeds while WiFi transfers data
- No radio interference causes connection drops
- Throughput degradation stays within acceptable limits (<30%)

**Requirements Covered:**
- **COEX-01**: 验证 WiFi 和 BLE 同时工作
- **COEX-02**: 测试 WiFi 连接时 BLE 广播稳定性
- **COEX-03**: 测试 BLE 连接时 WiFi 数据传输稳定性
- **COEX-04**: 配置多射频仲裁机制
- **DBG-04**: 集成性能分析工具（线程统计、内存使用）

**Deliverables:**
- Coexistence test application
- Radio arbitration configuration
- Performance metrics logging
- Stability test results

**Risks:**
- Radio arbitration misconfiguration
- Priority inversion between WiFi/BLE threads
- Firmware blob version mismatches

---

## Phase 5: System Features & OTA

**Goal:** Add production-ready system features and OTA capability

**Duration:** 3-4 days

**Success Criteria:**
- Device stores WiFi credentials across reboots
- OTA update completes successfully and device boots new firmware
- Watchdog resets device after 30s hang
- Shell commands allow runtime configuration changes

**Requirements Covered:**
- **SYS-02**: 实现持久化配置存储（NVS）
- **SYS-04**: 实现看门狗定时器
- **OTA-01**: 集成 MCUboot 安全启动
- **OTA-02**: 配置双 bank flash 分区布局
- **OTA-03**: 实现 OTA 固件更新机制
- **OTA-04**: 验证 OTA 更新流程（下载、验证、重启）
- **SEC-03**: 配置 EdgeLock Secure Enclave（如果 SDK 支持）
- **DBG-03**: 实现网络日志输出（可选）

**Deliverables:**
- NVS storage implementation
- MCUboot partition layout
- OTA update client code
- Watchdog configuration
- Network logging service (optional)

**Risks:**
- Flash partition layout errors causing boot failures
- MCUboot signature verification issues
- NVS corruption during power loss
- EdgeLock Secure Enclave documentation gaps

---

## Phase 6: Documentation & Examples

**Goal:** Create comprehensive documentation for template users

**Duration:** 2-3 days

**Success Criteria:**
- New developer can set up environment in <1 hour following docs
- All example code compiles and runs without modifications
- Troubleshooting guide resolves 80% of common issues
- API documentation covers all public interfaces

**Requirements Covered:**
- **DOC-01**: 编写工具链安装和配置文档
- **DOC-02**: 编写 WiFi 连接示例和说明
- **DOC-03**: 编写 BLE 服务示例和说明
- **DOC-04**: 编写 OTA 更新操作指南
- **DOC-05**: 编写常见问题排查文档
- **DOC-06**: 提供完整的示例代码和注释

**Deliverables:**
- README.md with quick start guide
- docs/SETUP.md for environment configuration
- docs/WIFI.md with WiFi examples
- docs/BLE.md with BLE examples
- docs/OTA.md with update procedures
- docs/TROUBLESHOOTING.md with common issues
- Inline code comments and API documentation

**Risks:**
- Documentation becomes outdated quickly
- Examples too complex for beginners
- Missing edge cases in troubleshooting guide

---

## Phase Dependencies

```
Phase 1 (Toolchain)
    ↓
Phase 2 (WiFi) ←→ Phase 3 (BLE)
    ↓                 ↓
    └─────→ Phase 4 (Coexistence)
                ↓
         Phase 5 (System/OTA)
                ↓
         Phase 6 (Documentation)
```

**Notes:**
- Phase 2 and 3 can partially overlap (different developers)
- Phase 4 requires both Phase 2 and 3 complete
- Phase 6 should start documentation early but finalizes last

---

## Success Metrics

**Technical Validation:**
- [ ] Firmware compiles without warnings
- [ ] All 45 v1 requirements validated
- [ ] WiFi connects in <5 seconds
- [ ] BLE advertising starts in <1 second
- [ ] OTA update completes in <2 minutes
- [ ] No memory leaks after 24h runtime
- [ ] Watchdog triggers correctly on hang

**Usability Validation:**
- [ ] New developer completes setup in <1 hour
- [ ] Example code runs without modifications
- [ ] Documentation answers common questions
- [ ] Template used for 1+ real IoT projects

---

## Out of Scope (v2+)

Explicitly deferred to future releases:

- WiFi AP mode and concurrent AP+Station
- BLE Central role and multi-connection
- Advanced WiFi 6 features (TWT, OFDMA)
- Matter protocol support
- Deep sleep power optimization
- Multi-board support beyond RW61x
- Production performance tuning

---

*Roadmap created: 2026-02-28*
*Next: Update STATE.md and REQUIREMENTS.md traceability*
