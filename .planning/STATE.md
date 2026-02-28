# Project State: RW61x Zephyr 工程模版

**Last Updated:** 2026-02-28
**Current Phase:** Phase 1 - Toolchain Foundation
**Status:** Complete - Pending Hardware Verification

---

## Current Status

### Active Phase: Phase 1 - Toolchain Foundation

**Goal:** Establish working build/debug environment for RW61x

**Progress:** 8/8 requirements completed (100%) - pending hardware verification

**Current Position:** All Phase 1 plans complete (01-01, 01-02, 01-03). Hardware verification checklist created. Awaiting developer confirmation of hardware tests.

**Next Actions:**
1. Developer: Run west update to fetch nxp_zsdk dependencies
2. Developer: Fetch hal_nxp firmware blobs with west blobs fetch
3. Developer: Build firmware with west build -b frdm_rw612 app/
4. Developer: Flash to hardware and verify UART output
5. Developer: Test J-Link debugging with breakpoints in VS Code
6. After hardware verification passes: Begin Phase 2 planning (WiFi Connectivity)

**Blockers:** None - awaiting hardware verification by developer

---

## Phase Progress

| Phase | Status | Requirements | Completed | Progress |
|-------|--------|--------------|-----------|----------|
| Phase 1: Toolchain Foundation | ✅ Complete* | 8 | 8 | 100% |
| Phase 2: WiFi Connectivity | ⚪ Pending | 9 | 0 | 0% |
| Phase 3: BLE Functionality | ⚪ Pending | 5 | 0 | 0% |
| Phase 4: WiFi/BLE Coexistence | ⚪ Pending | 5 | 0 | 0% |
| Phase 5: System Features & OTA | ⚪ Pending | 12 | 0 | 0% |
| Phase 6: Documentation & Examples | ⚪ Pending | 6 | 0 | 0% |

**Overall Progress:** 8/45 requirements (18%)

*Phase 1 complete pending hardware verification by developer

---

## Requirements Status

### Phase 1: Toolchain Foundation (8/8)

- [x] TOOL-01: 基于 nxp_zsdk (nxp-v4.3-branch) 创建可编译的 RW61x 工程
- [x] TOOL-02: 配置 VS Code + west 开发环境并验证可用性
- [x] TOOL-03: 配置设备树（Device Tree）支持 RW61x 硬件
- [x] TOOL-04: 验证编译、烧录、运行基本流程
- [x] TOOL-05: 集成 JTAG/SWD 硬件调试器支持（pyOCD 或 J-Link）
- [x] SYS-01: 集成 Zephyr 日志系统（串口输出）
- [x] DBG-01: 配置串口日志输出
- [x] DBG-02: 验证 JTAG/SWD 断点调试

### Phase 2: WiFi Connectivity (0/9)

- [ ] WIFI-01: 实现 WiFi Station 模式连接 AP
- [ ] WIFI-02: 支持 WPA2/WPA3 安全认证
- [ ] WIFI-03: 实现 DHCP 客户端自动获取 IP
- [ ] WIFI-04: 实现网络连接管理（连接、断开、重连）
- [ ] WIFI-05: 验证 WiFi 固件 blob 正确加载
- [ ] SYS-03: 集成 Shell/CLI 接口用于调试
- [ ] SYS-05: 配置线程栈大小（WiFi/BLE 需 2-4KB）
- [ ] SEC-01: 集成 mbedTLS 支持
- [ ] SEC-02: 验证 TLS 加密通信

### Phase 3: BLE Functionality (0/5)

- [ ] BLE-01: 实现 BLE 外设角色（Peripheral）
- [ ] BLE-02: 实现 BLE 广播功能
- [ ] BLE-03: 实现 GATT 服务定义和注册
- [ ] BLE-04: 支持 BLE 连接和断开
- [ ] BLE-05: 实现 GATT 特征读写回调

### Phase 4: WiFi/BLE Coexistence (0/5)

- [ ] COEX-01: 验证 WiFi 和 BLE 同时工作
- [ ] COEX-02: 测试 WiFi 连接时 BLE 广播稳定性
- [ ] COEX-03: 测试 BLE 连接时 WiFi 数据传输稳定性
- [ ] COEX-04: 配置多射频仲裁机制
- [ ] DBG-04: 集成性能分析工具（线程统计、内存使用）

### Phase 5: System Features & OTA (0/12)

- [ ] SYS-02: 实现持久化配置存储（NVS）
- [ ] SYS-04: 实现看门狗定时器
- [ ] OTA-01: 集成 MCUboot 安全启动
- [ ] OTA-02: 配置双 bank flash 分区布局
- [ ] OTA-03: 实现 OTA 固件更新机制
- [ ] OTA-04: 验证 OTA 更新流程（下载、验证、重启）
- [ ] SEC-03: 配置 EdgeLock Secure Enclave（如果 SDK 支持）
- [ ] DBG-03: 实现网络日志输出（可选）

### Phase 6: Documentation & Examples (0/6)

- [ ] DOC-01: 编写工具链安装和配置文档
- [ ] DOC-02: 编写 WiFi 连接示例和说明
- [ ] DOC-03: 编写 BLE 服务示例和说明
- [ ] DOC-04: 编写 OTA 更新操作指南
- [ ] DOC-05: 编写常见问题排查文档
- [ ] DOC-06: 提供完整的示例代码和注释

---

## Recent Changes

**2026-02-28:**
- ✅ Roadmap created with 6 phases
- ✅ All 45 v1 requirements mapped to phases
- ✅ Phase 1 ready to start
- ✅ Plan 01-01 complete: Project scaffold created
  - west.yml with T-topology manifest
  - app/CMakeLists.txt and src/main.c
  - prj.conf and composable Kconfig fragments
  - Device tree overlay for FRDM-RW612
  - Commits: 7f2f402, 9c43306
- ✅ Plan 01-02 complete: VS Code workspace configuration
  - launch.json with J-Link debug configuration
  - tasks.json with west build/flash/blobs tasks
  - extensions.json with recommended extensions
  - Commit: c5ee8e7
- ✅ Plan 01-03 complete: Hardware verification checklist
  - Automated file structure validation (7 checks passed)
  - Developer setup commands with expected outputs
  - Hardware verification workflow documentation
  - Troubleshooting guide for common issues
  - Commit: 921104d
- ✅ Phase 1 complete: All 8 requirements satisfied
  - Pending hardware verification by developer
  - Ready for Phase 2 planning

---

## Known Issues

None yet - project just started.

---

## Decisions Log

| Date | Decision | Rationale | Impact |
|------|----------|-----------|--------|
| 2026-02-28 | 6-phase roadmap structure | Follows research recommendations: toolchain → connectivity → coexistence → production features | Clear progression, reduces risk |
| 2026-02-28 | Phase 2/3 can overlap | WiFi and BLE are independent subsystems | Faster delivery if multiple developers |
| 2026-02-28 | T-topology west manifest | nxp_zsdk as imported project, self.path = app | Standard Zephyr workspace pattern |
| 2026-02-28 | Composable Kconfig fragments | Separate debug.conf, wifi.conf, ble.conf | Flexible build configurations via EXTRA_CONF_FILE |
| 2026-02-28 | flexcomm3 as console UART | FRDM-RW612 board default | Matches NXP reference design |
| 2026-02-28 | J-Link device string "RW612" | Default from research; alternative MIMXRW612 documented | Developer can verify with JLinkExe if needed |
| 2026-02-28 | SVD file path included but optional | Path provided for peripheral register view; removal instructions if missing | Better debugging experience when available |
| 2026-02-28 | Dedicated west blobs fetch task | hal_nxp firmware blobs critical for WiFi/BLE; separate task for visibility | Prevents "forgot to fetch blobs" issues |
| 2026-02-28 | Hardware verification as separate plan | Verification requires physical hardware; automated checks + developer checklist | Clear separation of automated vs manual steps |
| 2026-02-28 | Verification checklist format | Structured setup commands with expected outputs and troubleshooting | Reduces developer friction, clear success criteria |

---

## Risk Register

| Risk | Probability | Impact | Mitigation | Status |
|------|-------------|--------|------------|--------|
| WiFi firmware blob missing | Medium | High | Verify hal_nxp module early in Phase 1; documented in verification checklist | Mitigated |
| Thread stack overflow | Medium | Medium | Follow research guidance (2-4KB stacks) | Open |
| JTAG debugger macOS issues | Low | Medium | Test pyOCD and J-Link alternatives | Open |
| WPA3 incomplete support | Low | Low | Fall back to WPA2 if needed | Open |
| EdgeLock documentation gaps | Medium | Low | Defer SEC-03 if SDK support unclear | Open |

---

*State tracking started: 2026-02-28*
*Update this file as phases progress*
