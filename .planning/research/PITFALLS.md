# Pitfalls Research

**Domain:** Zephyr RTOS + NXP RW61x WiFi/BLE Firmware Development
**Researched:** 2026-02-28
**Confidence:** MEDIUM (Context7 + domain expertise, limited RW61x-specific documentation)

## Critical Pitfalls

### Pitfall 1: Missing or Incorrect Binary Blobs for WiFi Driver

**What goes wrong:**
WiFi driver compiles successfully but fails at runtime with cryptic initialization errors or silent failures. The device cannot scan for networks or connect to APs despite correct configuration.

**Why it happens:**
NXP WiFi drivers require proprietary firmware blobs that must be explicitly included in the build. Developers often miss the `CONFIG_BUILD_ONLY_NO_BLOBS` warning or don't realize the driver needs separate firmware files beyond the driver code itself. The build system allows compilation without blobs for CI purposes, creating a false sense of success.

**How to avoid:**
- Verify `hal_nxp` module includes firmware blobs for RW61x in `modules/hal_nxp/`
- Check CMakeLists.txt doesn't have `CONFIG_BUILD_ONLY_NO_BLOBS=y`
- Confirm firmware blob paths in device tree match actual file locations
- Add explicit build-time check that firmware files exist before linking

**Warning signs:**
- Build succeeds but WiFi initialization returns -ENODEV or -EIO
- `west build` shows warning about building without binary blobs
- WiFi driver probe fails silently in logs
- `dmesg` or logs show "firmware not found" errors

**Phase to address:**
Phase 1 (Toolchain Setup) — Verify blob availability before any WiFi development

---

### Pitfall 2: Thread Stack Overflow from WiFi/BLE Event Handlers

**What goes wrong:**
System crashes, hangs, or exhibits random behavior when WiFi connects or BLE events occur. Stack corruption causes hard-to-debug issues that appear intermittent or load-dependent.

**Why it happens:**
WiFi and BLE stacks invoke callbacks from their own threads with deep call stacks. Default stack sizes (512-1024 bytes) are insufficient for network protocol processing, especially when callbacks perform logging, JSON parsing, or trigger additional operations. Developers copy example code without adjusting stack sizes for real workloads.

**How to avoid:**
- Set WiFi management thread stack to minimum 4096 bytes (`CONFIG_NET_MGMT_EVENT_STACK_SIZE=4096`)
- Set BLE RX thread stack to minimum 2048 bytes (`CONFIG_BT_RX_STACK_SIZE=2048`)
- Enable stack sentinel checking during development (`CONFIG_STACK_SENTINEL=y`)
- Use `k_thread_stack_space_get()` to monitor actual usage
- Add `CONFIG_THREAD_STACK_INFO=y` for runtime analysis

**Warning signs:**
- Crashes occur only after WiFi connection established or during BLE operations
- Memory corruption detected by MPU or stack canaries
- System becomes unstable under network load
- Logs show stack overflow warnings before crash
- Different behavior between debug and release builds

**Phase to address:**
Phase 2 (WiFi Basic) and Phase 3 (BLE Basic) — Configure before implementing network features

---

### Pitfall 3: Device Tree WiFi/BLE Node Misconfiguration

**What goes wrong:**
Drivers fail to initialize, wrong GPIO pins are used for control signals, or WiFi/BLE hardware isn't detected. Errors like "device not ready" or "binding failed" appear at boot.

**Why it happens:**
RW61x requires specific device tree configuration for WiFi/BLE coexistence, shared antenna control, and power management. NXP's downstream device tree may differ from upstream Zephyr conventions. Developers copy device trees from different boards without understanding RW61x-specific requirements.

**How to avoid:**
- Start from NXP's official RW61x board device tree in nxp_zsdk
- Verify `compatible` strings match RW61x WiFi (`nxp,rw61x-wifi`) and BLE (`nxp,rw61x-bt`) drivers
- Confirm GPIO pins for WiFi enable, BLE enable, and antenna switching match hardware schematic
- Check clock configuration nodes for correct frequencies (WiFi needs specific clock rates)
- Validate device tree with `west build -t devicetree` before flashing

**Warning signs:**
- Boot logs show "device binding failed" for WiFi or BLE
- `device_get_binding()` returns NULL for network interfaces
- GPIO control signals don't toggle (check with oscilloscope/logic analyzer)
- WiFi/BLE initialization hangs indefinitely
- Different behavior between boards with "same" configuration

**Phase to address:**
Phase 1 (Toolchain Setup) — Validate device tree before any driver work

---

### Pitfall 4: WiFi/BLE Coexistence Timing Conflicts

**What goes wrong:**
WiFi connection drops when BLE is active, BLE advertisements are missed during WiFi traffic, or throughput degrades severely when both radios operate simultaneously. System may crash under heavy dual-radio load.

**Why it happens:**
RW61x shares RF hardware between WiFi and BLE, requiring careful time-slicing and priority management. Without proper coexistence configuration, both stacks compete for radio access causing packet loss and timing violations. Default configurations often prioritize one radio over the other.

**How to avoid:**
- Enable coexistence support in Kconfig (`CONFIG_WIFI_NXP_RW61X_COEX=y` if available)
- Configure coexistence GPIO pins in device tree for hardware arbitration
- Set appropriate priority levels: WiFi connection > BLE connection > BLE advertising
- Limit BLE connection intervals to allow WiFi beacon reception (min 30ms intervals)
- Use WiFi power save mode (PSM) to create BLE transmission windows
- Test with simultaneous WiFi traffic and BLE connections, not separately

**Warning signs:**
- WiFi disconnects when BLE connection is established
- BLE connection parameters negotiation fails
- Packet loss increases dramatically with both radios active
- WiFi throughput drops below 50% when BLE is advertising
- System becomes unstable only when both WiFi and BLE are enabled

**Phase to address:**
Phase 4 (WiFi/BLE Coexistence) — Dedicated phase for integration testing

---

### Pitfall 5: Incorrect MCUboot Partition Layout for OTA

**What goes wrong:**
OTA updates fail with "insufficient space" errors, bootloader can't find update image, or device bricks after update attempt. Flash writes succeed but MCUboot refuses to swap images.

**Why it happens:**
MCUboot requires specific partition alignment and sizing that depends on flash page size and write alignment. RW61x flash characteristics may differ from reference boards. Developers copy partition layouts without adjusting for actual flash geometry or application size growth.

**How to avoid:**
- Align all partitions to flash erase block size (typically 4KB for RW61x)
- Size SLOT-0 and SLOT-1 identically (MCUboot requirement)
- Reserve scratch partition if using swap-based updates (size = largest sector)
- Account for image header overhead (MCUboot adds ~1KB per image)
- Leave 20-30% growth margin in partition sizes for future features
- Validate partition layout with `west build -t partition_manager_report`

**Warning signs:**
- MCUboot logs "invalid image header" or "image too large"
- OTA download succeeds but device doesn't reboot to new image
- Bootloader enters recovery mode after update attempt
- Flash write operations fail with alignment errors
- Update works on first attempt but fails on subsequent updates

**Phase to address:**
Phase 5 (OTA Updates) — Design partition layout before implementing OTA

---

### Pitfall 6: Power Management Breaking WiFi Connection Persistence

**What goes wrong:**
Device enters sleep mode and never wakes up, or WiFi connection is lost after sleep and cannot reconnect. System appears frozen or requires hard reset to recover.

**Why it happens:**
WiFi hardware requires specific power state transitions and wake-up sequencing. Zephyr's power management may put WiFi hardware into states it can't recover from without proper wake-up source configuration. Clock gating or voltage scaling can disrupt WiFi timing requirements.

**How to avoid:**
- Configure WiFi device as wake-up source in device tree (`wakeup-source;` property)
- Implement WiFi-specific PM hooks to save/restore connection state
- Use WiFi power save mode (PSM/TWT) instead of system-level deep sleep during connection
- Keep WiFi clocks active during light sleep (`CONFIG_PM_DEVICE_RUNTIME=y`)
- Test wake-up from all sleep states with active WiFi connection
- Monitor `NET_EVENT_L4_DISCONNECTED` and implement reconnection logic

**Warning signs:**
- Device doesn't respond to network traffic after idle period
- WiFi connection shows as "connected" but no data flows
- System wakes from sleep but WiFi initialization fails
- Increased power consumption indicates sleep mode not entered
- Logs show "device not ready" after wake-up

**Phase to address:**
Phase 6 (Power Management) — Test with WiFi active, not just idle states

---

### Pitfall 7: BLE GATT Service Registration Timing Issues

**What goes wrong:**
BLE services appear missing to clients, characteristic reads/writes fail, or notifications don't work despite correct implementation. Services work intermittently or only after reconnection.

**Why it happens:**
GATT services must be registered before Bluetooth is enabled, but developers often register them in initialization code that runs after `bt_enable()`. Service registration during active connections can cause database inconsistencies. Notification/indication setup requires proper CCC (Client Characteristic Configuration) handling that's easy to misconfigure.

**How to avoid:**
- Register all GATT services before calling `bt_enable()`
- Use `BT_GATT_SERVICE_DEFINE()` macro for static registration (preferred)
- For dynamic services, register in `bt_ready()` callback before advertising
- Verify CCC descriptors are automatically added for notifiable characteristics
- Check `bt_gatt_notify()` return value — it fails if client hasn't subscribed
- Use `bt_gatt_indicate()` for critical data requiring acknowledgment

**Warning signs:**
- BLE service discovery shows incomplete service list
- Characteristic UUIDs appear but read/write operations fail
- Notifications work after reconnection but not on first connection
- `bt_gatt_notify()` returns -EINVAL or -ENOTCONN
- Different behavior between iOS and Android clients

**Phase to address:**
Phase 3 (BLE Basic) — Establish registration pattern before adding services

---

### Pitfall 8: Insufficient Heap for Network Buffers

**What goes wrong:**
Network operations fail with -ENOMEM errors, WiFi connection drops under load, or system runs out of memory during data transfer. Symptoms worsen with concurrent WiFi and BLE activity.

**Why it happens:**
Zephyr's network stack allocates buffers from heap for packet processing. Default heap size (often 16-32KB) is insufficient for WiFi packet buffers, TLS sessions, and BLE L2CAP channels. Each WiFi packet needs ~1.5KB, TLS adds ~16KB, and BLE connections need ~2KB per link.

**How to avoid:**
- Set minimum heap size to 64KB for WiFi+BLE (`CONFIG_HEAP_MEM_POOL_SIZE=65536`)
- Increase to 128KB if using TLS/DTLS (`CONFIG_MBEDTLS_HEAP_SIZE=65536`)
- Configure network buffer pools explicitly:
  - `CONFIG_NET_BUF_RX_COUNT=16` (WiFi RX buffers)
  - `CONFIG_NET_BUF_TX_COUNT=16` (WiFi TX buffers)
  - `CONFIG_BT_L2CAP_TX_BUF_COUNT=8` (BLE buffers)
- Monitor heap usage with `kernel heap` shell command
- Enable heap statistics (`CONFIG_SYS_HEAP_RUNTIME_STATS=y`)

**Warning signs:**
- Intermittent -ENOMEM errors in logs
- WiFi connection succeeds but data transfer fails
- System becomes unstable under network load
- Memory allocation failures during TLS handshake
- BLE connection establishment fails randomly

**Phase to address:**
Phase 2 (WiFi Basic) — Configure before network testing begins

---

### Pitfall 9: West Manifest and Module Version Mismatches

**What goes wrong:**
Build fails with missing symbols, API incompatibilities, or linker errors. Code that works in examples fails in your project. Subtle runtime bugs appear from ABI mismatches.

**Why it happens:**
NXP's nxp_zsdk uses specific versions of Zephyr modules (hal_nxp, mbedtls, etc.) that may conflict with other dependencies. Using `west update` without pinning versions can pull incompatible module versions. Mixing upstream Zephyr examples with nxp_zsdk creates version skew.

**How to avoid:**
- Use nxp_zsdk's west.yml as authoritative manifest
- Pin all module revisions in west.yml (don't use `revision: main`)
- Run `west update` after any manifest changes
- Verify module versions with `west list` before building
- Don't manually clone modules — let west manage them
- Document exact SDK version in project README (commit hash or tag)

**Warning signs:**
- Build errors about undefined references to WiFi/BLE functions
- Linker complains about multiple definitions
- Code compiles but crashes at runtime with invalid function pointers
- Examples from NXP work but your code doesn't with same configuration
- Different build results on different machines

**Phase to address:**
Phase 1 (Toolchain Setup) — Lock versions before any development

---

### Pitfall 10: Logging Overhead Causing Real-Time Violations

**What goes wrong:**
WiFi packets are dropped, BLE connection intervals are missed, or system becomes sluggish. Timing-sensitive operations fail intermittently. Disabling logs "fixes" the problem.

**Why it happens:**
Zephyr's default logging mode (deferred) still has overhead from formatting and buffer management. WiFi/BLE drivers log extensively at INFO level. Logging from ISR context or high-priority threads can block critical operations. UART output at 115200 baud is too slow for high-frequency logs.

**How to avoid:**
- Use minimal logging mode for production (`CONFIG_LOG_MODE_MINIMAL=y`)
- Set driver log levels to WARNING in production (INFO only for debug)
- Disable logging in ISR context (`CONFIG_LOG_PRINTK=n`)
- Increase UART baud rate to 921600 or use RTT for logging
- Use async logging with sufficient buffer size (`CONFIG_LOG_BUFFER_SIZE=4096`)
- Profile with logging disabled to identify timing-critical paths

**Warning signs:**
- System performance improves dramatically with logs disabled
- Timing issues disappear at lower log levels
- UART output shows buffer overflow warnings
- WiFi throughput increases when logging is reduced
- BLE connection parameters negotiation fails with verbose logging

**Phase to address:**
Phase 1 (Toolchain Setup) — Configure logging strategy early

---

## Technical Debt Patterns

Shortcuts that seem reasonable but create long-term problems.

| Shortcut | Immediate Benefit | Long-term Cost | When Acceptable |
|----------|-------------------|----------------|-----------------|
| Using single large thread for WiFi+BLE | Simpler code, less memory | Race conditions, poor responsiveness, hard to debug | Never — RTOS exists for separation |
| Hardcoding WiFi credentials in firmware | Fast testing, no provisioning UI | Security risk, inflexible deployment, requires reflash | Development only, never production |
| Skipping MCUboot signature verification | Faster boot, simpler build | No security, vulnerable to malicious updates | Early prototyping only, enable before field testing |
| Disabling stack guards for memory savings | ~100 bytes per thread saved | Silent corruption, impossible debugging | Never — stack bugs are catastrophic |
| Using polling instead of interrupts for WiFi | Simpler driver integration | High CPU usage, poor power efficiency, missed packets | Never for battery-powered devices |
| Copying entire device tree from reference board | Quick start | Wrong pin assignments, missing features, hardware conflicts | Initial bringup only, customize immediately |
| Setting all thread priorities to same level | Avoids priority inversion | No real-time guarantees, unpredictable behavior | Never — defeats RTOS purpose |
| Using sprintf instead of snprintf | Slightly faster | Buffer overflows, security vulnerabilities | Never — always use bounded functions |

## Integration Gotchas

Common mistakes when connecting to external services.

| Integration | Common Mistake | Correct Approach |
|-------------|----------------|------------------|
| MQTT over WiFi | Not handling reconnection after WiFi drop | Implement connection state machine with exponential backoff |
| TLS certificate validation | Disabling verification for "testing" | Use proper CA certificates, test with self-signed CA |
| NTP time sync | Assuming time is available immediately after WiFi connect | Wait for NTP sync event before time-dependent operations |
| Cloud IoT platforms | Sending data before connection confirmed | Wait for CONNACK/connection callback before publishing |
| BLE mobile app pairing | Not persisting bonding information | Store bonds in flash, restore on boot |
| WiFi provisioning via BLE | Blocking BLE thread during WiFi connection | Use separate thread or async WiFi connection |
| HTTP REST APIs | Not handling partial receives | Use content-length header, loop until complete |
| Firmware update servers | No resume capability for interrupted downloads | Implement range requests, checksum validation |

## Performance Traps

Patterns that work at small scale but fail as usage grows.

| Trap | Symptoms | Prevention | When It Breaks |
|------|----------|------------|----------------|
| Synchronous WiFi operations in main thread | UI freezes, missed BLE events | Use async WiFi APIs, separate network thread | First WiFi connection attempt |
| Allocating large buffers on stack | Stack overflow, corruption | Use heap or static allocation for >1KB buffers | When buffer size exceeds ~512 bytes |
| Frequent flash writes for state persistence | Flash wear-out, slow performance | Batch writes, use wear-leveling, cache in RAM | After ~10K write cycles |
| Linear search through BLE connection list | Slow connection handling | Use hash table or array indexed by conn handle | >3 simultaneous connections |
| Copying packet data multiple times | High CPU usage, low throughput | Use zero-copy buffers, pass pointers | WiFi throughput >1 Mbps |
| Blocking on mutex in ISR context | System hangs, watchdog resets | Use lock-free queues, defer work to thread | First interrupt occurrence |
| String formatting in hot path | High CPU usage, jitter | Pre-format strings, use binary protocols | >10 messages/second |
| Not using DMA for UART logging | CPU bottleneck, timing issues | Enable DMA for UART TX | Log rate >1000 lines/second |

## Security Mistakes

Domain-specific security issues beyond general web security.

| Mistake | Risk | Prevention |
|---------|------|------------|
| Storing WiFi passwords in plaintext flash | Credentials extractable via JTAG/SWD | Encrypt credentials, use secure storage if available |
| No secure boot chain verification | Malicious firmware installation | Enable MCUboot signature verification, fuse public key |
| BLE pairing without MITM protection | Man-in-the-middle attacks | Use passkey entry or numeric comparison pairing |
| Accepting any BLE connection | Unauthorized device access | Implement whitelist or bonding requirement |
| No rollback protection for OTA | Downgrade to vulnerable firmware | Use MCUboot version monotonic counter |
| Debug interfaces enabled in production | Device cloning, credential theft | Disable JTAG/SWD via fuses in production builds |
| Predictable BLE MAC addresses | Device tracking, privacy violation | Use random resolvable addresses |
| No certificate pinning for cloud connections | MITM via rogue CA | Pin server certificate or public key |
| Unencrypted firmware update transport | Firmware tampering in transit | Use HTTPS for downloads, verify signatures |
| Hardcoded encryption keys | Single compromise affects all devices | Use per-device keys, secure provisioning |

## UX Pitfalls

Common user experience mistakes in this domain.

| Pitfall | User Impact | Better Approach |
|---------|-------------|-----------------|
| No feedback during WiFi connection | User doesn't know if device is working | Blink LED, show progress via BLE characteristic |
| Silent failures in provisioning | User stuck, doesn't know what went wrong | Return error codes via BLE, log to persistent storage |
| No indication of firmware update progress | User unplugs device thinking it's frozen | Update progress characteristic, LED patterns |
| Requiring factory reset for WiFi changes | Lost configuration, frustration | Provide WiFi reconfiguration via BLE |
| No way to verify device is online | User can't troubleshoot connectivity | Expose connection status via BLE or LED |
| Long boot time without indication | Appears broken, user returns device | Show "booting" indication, optimize boot time |
| BLE advertising stops after timeout | Device becomes invisible, unusable | Keep advertising or provide button to restart |
| No local control when WiFi is down | Device useless without internet | Implement local BLE control as fallback |

## "Looks Done But Isn't" Checklist

Things that appear complete but are missing critical pieces.

- [ ] **WiFi Connection:** Often missing reconnection logic after AP reboot — verify automatic reconnection after 5-minute disconnect
- [ ] **BLE Services:** Often missing CCC persistence across reboots — verify notifications work after device reboot without re-pairing
- [ ] **OTA Updates:** Often missing rollback on boot failure — verify MCUboot reverts to old image if new one crashes
- [ ] **Power Management:** Often missing wake-up source configuration — verify device wakes from deep sleep on button press or timer
- [ ] **Error Handling:** Often missing recovery from WiFi driver errors — verify system recovers from WiFi hardware reset
- [ ] **Flash Storage:** Often missing wear-leveling — verify settings survive 10K+ write cycles
- [ ] **TLS Connections:** Often missing certificate expiration handling — verify behavior when server cert expires
- [ ] **BLE Pairing:** Often missing bond storage — verify paired devices reconnect after reboot
- [ ] **Logging:** Often missing log level control at runtime — verify can change log levels without reflashing
- [ ] **Watchdog:** Often missing watchdog feeding in all code paths — verify system recovers from thread deadlock
- [ ] **Time Sync:** Often missing timezone handling — verify timestamps are correct across DST changes
- [ ] **Network Buffers:** Often missing buffer exhaustion handling — verify graceful degradation when out of buffers

## Recovery Strategies

When pitfalls occur despite prevention, how to recover.

| Pitfall | Recovery Cost | Recovery Steps |
|---------|---------------|----------------|
| Missing WiFi firmware blobs | LOW | Add hal_nxp module to west.yml, run west update, rebuild |
| Stack overflow | MEDIUM | Increase stack sizes in prj.conf, rebuild, reflash |
| Wrong device tree | MEDIUM | Copy correct DTS from nxp_zsdk, adjust pins, rebuild |
| Coexistence conflicts | HIGH | Redesign radio scheduling, may require firmware architecture changes |
| Wrong partition layout | HIGH | Reflash bootloader and app with new layout, loses all data |
| Power management issues | MEDIUM | Disable PM temporarily, implement proper PM hooks incrementally |
| GATT registration timing | LOW | Move service registration before bt_enable(), rebuild |
| Insufficient heap | LOW | Increase heap size in prj.conf, rebuild |
| Module version mismatch | MEDIUM | Reset west workspace, checkout correct versions, rebuild all |
| Logging overhead | LOW | Reduce log levels in prj.conf, rebuild |

## Pitfall-to-Phase Mapping

How roadmap phases should address these pitfalls.

| Pitfall | Prevention Phase | Verification |
|---------|------------------|--------------|
| Missing WiFi firmware blobs | Phase 1: Toolchain Setup | Build succeeds and WiFi driver probes successfully |
| Thread stack overflow | Phase 2: WiFi Basic | Stack sentinel checks pass under load testing |
| Device tree misconfiguration | Phase 1: Toolchain Setup | All devices show "ready" in boot logs |
| WiFi/BLE coexistence conflicts | Phase 4: WiFi/BLE Coexistence | Simultaneous WiFi+BLE operation for 1 hour without drops |
| Wrong MCUboot partition layout | Phase 5: OTA Updates | Successful OTA update and rollback test |
| Power management breaking WiFi | Phase 6: Power Management | Wake from sleep and resume WiFi within 2 seconds |
| BLE GATT registration timing | Phase 3: BLE Basic | Service discovery succeeds on first connection |
| Insufficient heap | Phase 2: WiFi Basic | No memory allocation failures during stress test |
| West module version mismatch | Phase 1: Toolchain Setup | Clean build succeeds on fresh checkout |
| Logging overhead | Phase 1: Toolchain Setup | WiFi throughput same with/without logging |

## Sources

- Zephyr Project Official Documentation (Context7: /zephyrproject-rtos/zephyr)
- NXP RW61x SDK Documentation (nxp_zsdk repository)
- Zephyr WiFi Driver Implementation (CMakeLists.txt, driver source analysis)
- Zephyr Bluetooth Stack Documentation (GATT API reference)
- MCUboot Documentation (partition layout, OTA update flow)
- Zephyr Power Management Documentation (device PM, system PM)
- Domain expertise: Embedded WiFi/BLE firmware development patterns
- Common issues from Zephyr GitHub issues and mailing list discussions

---
*Pitfalls research for: Zephyr RTOS + NXP RW61x WiFi/BLE Firmware*
*Researched: 2026-02-28*
