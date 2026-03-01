# Stack Research

**Domain:** Zephyr RTOS embedded firmware development for NXP RW61x with WiFi/BLE
**Researched:** 2026-02-28
**Confidence:** MEDIUM

## Recommended Stack

### Core Technologies

| Technology | Version | Purpose | Why Recommended |
|------------|---------|---------|-----------------|
| Zephyr RTOS | 4.3.0 | Real-time operating system | Latest stable release (Nov 2025), full BLE 5.3 support, mature WiFi stack, active development |
| NXP ZSDK | nxp-v4.3-branch (v4.3.0) | NXP downstream Zephyr SDK | Official NXP-maintained fork with RW61x drivers, WiFi/BLE coexistence support, released Dec 2025 |
| West | 1.5.0 | Meta-tool for workspace management | Latest stable (Sep 2025), required for Zephyr development, handles multi-repo projects |
| CMake | 3.20.5+ | Build system | Zephyr's native build system, devicetree integration, Kconfig support |
| Zephyr SDK | 0.17.4+ | Cross-compilation toolchain | Official toolchain with ARM Cortex-M33 support, includes GDB, OpenOCD, QEMU |
| Python | 3.12+ | Build scripts and tools | Minimum required version for Zephyr 4.3.0, needed for west and build automation |

### Supporting Libraries

| Library | Version | Purpose | When to Use |
|---------|---------|---------|-------------|
| MCUboot | Latest (via Zephyr) | Secure bootloader | Required for OTA updates, secure boot, image signing/verification |
| MCUmgr | Built-in (Zephyr 4.3.0) | Device management framework | OTA updates, remote configuration, file system management over BLE/UART/UDP |
| mbedTLS | Built-in (Zephyr 4.3.0) | Cryptography library | WiFi WPA2/WPA3, TLS/DTLS, secure storage, PSA Crypto API support |
| NVS (Non-Volatile Storage) | Built-in (Zephyr 4.3.0) | Persistent configuration | WiFi credentials, BLE bonding data, application settings |
| Zephyr Logging | Built-in (Zephyr 4.3.0) | Structured logging system | Debugging, runtime diagnostics, deferred logging for performance |
| Zephyr Shell | Built-in (Zephyr 4.3.0) | Interactive CLI | Runtime debugging, WiFi/BLE control, configuration management |

### Development Tools

| Tool | Purpose | Notes |
|------|---------|-------|
| VS Code | Primary IDE | Official Zephyr support, C/C++ Extension Pack required, IntelliSense via compile_commands.json |
| pyOCD | JTAG/SWD debugging | macOS compatible, RTOS-aware, supports CMSIS-DAP probes |
| J-Link | JTAG/SWD debugging (alternative) | Commercial option, excellent macOS support, faster than OpenOCD |
| OpenOCD | JTAG/SWD debugging (alternative) | Open source, included in Zephyr SDK, RTOS-aware fork |
| Ninja | Build tool | Fast parallel builds, used by west build system |
| Git | Version control | Required for west workspace management |

## Installation

```bash
# Python environment setup
python3 -m venv ~/zephyrproject/.venv
source ~/zephyrproject/.venv/bin/activate

# Install west
pip3 install west==1.5.0

# Initialize workspace with NXP ZSDK
mkdir ~/zephyrproject
cd ~/zephyrproject
west init -m https://github.com/nxp-zephyr/nxp-zsdk --mr nxp-v4.3-branch
west update

# Install Python dependencies
west packages pip --install

# Install Zephyr SDK (macOS)
cd ~
wget https://github.com/zephyrproject-rtos/sdk-ng/releases/download/v0.17.4/zephyr-sdk-0.17.4_macos-aarch64.tar.xz
tar xvf zephyr-sdk-0.17.4_macos-aarch64.tar.xz
cd zephyr-sdk-0.17.4
./setup.sh

# Install debugging tools (optional)
pip3 install pyocd
# OR download J-Link from Segger website
```

## Alternatives Considered

| Recommended | Alternative | When to Use Alternative |
|-------------|-------------|-------------------------|
| Zephyr 4.3.0 | Zephyr 3.7.0 LTS | Long-term support needed (until 2029), stability over features |
| NXP ZSDK nxp-v4.3-branch | Upstream Zephyr | RW61x not yet in upstream, use NXP fork for RW61x support |
| pyOCD | J-Link | Budget allows commercial tools, need fastest debug performance |
| MCUboot | Custom bootloader | Avoid unless specific requirements, MCUboot is battle-tested |
| NVS | Flash Circular Buffer (FCB) | Legacy projects, NVS is newer and recommended |
| mbedTLS | TinyCrypt | mbedTLS has PSA Crypto API support, better WiFi integration |

## What NOT to Use

| Avoid | Why | Use Instead |
|-------|-----|-------------|
| Zephyr < 3.7.0 | Missing BLE 5.3 features, older WiFi stack, security vulnerabilities | Zephyr 4.3.0 or 3.7.0 LTS |
| MCUXpresso IDE | NXP's Eclipse-based IDE not optimized for Zephyr workflow | VS Code + west |
| Manual toolchain setup | Error-prone, version mismatches, missing Zephyr-specific patches | Zephyr SDK 0.17.4+ |
| Python < 3.12 | Not supported by Zephyr 4.3.0, build failures | Python 3.12+ |
| west < 1.0 | Missing critical features, compatibility issues | west 1.5.0 |
| Settings subsystem (deprecated path) | Documentation redirects, API changes | Use NVS directly for simple key-value storage |

## Stack Patterns by Variant

**If building WiFi-only device:**
- Use `CONFIG_WIFI_USAGE_MODE=STA` to reduce binary size
- Disable BLE stack with `CONFIG_BT=n`
- Consider WPA3-SAE for enhanced security

**If building BLE-only device:**
- Disable WiFi drivers to save flash/RAM
- Use BLE mesh if multi-hop networking needed
- Consider BLE Long Range for extended coverage

**If building WiFi/BLE coexistence device (RW61x typical use case):**
- Use NXP ZSDK coexistence samples as reference (`samples/wireless/coex/`)
- Enable both `CONFIG_WIFI_NXP=y` and `CONFIG_BT_NXP=y`
- Test coexistence scenarios (simultaneous WiFi + BLE operation)
- Use MCUmgr over BLE for OTA while WiFi handles data traffic

**If OTA updates required:**
- Enable MCUboot bootloader
- Configure dual-image flash partitions
- Use MCUmgr with appropriate transport (BLE/UART/UDP)
- Implement image signing for production

**If low power operation critical:**
- Use Zephyr power management subsystem
- Configure WiFi power save modes
- Use BLE connection intervals appropriately
- Consider RW61x low-power modes

## Version Compatibility

| Package A | Compatible With | Notes |
|-----------|-----------------|-------|
| Zephyr 4.3.0 | Zephyr SDK 0.17.4+ | Recommended pairing, tested together |
| NXP ZSDK nxp-v4.3-branch | Zephyr 4.3.0 base | NXP fork based on Zephyr 4.3.0 |
| west 1.5.0 | Zephyr 4.3.0 | Latest west compatible with all Zephyr 4.x |
| Python 3.12+ | Zephyr 4.3.0 | Minimum 3.12 required |
| MCUboot | Zephyr 4.3.0 | Included as Zephyr module, version-matched |
| VS Code C/C++ Extension | Any recent | Use compile_commands.json for IntelliSense |

## RW61x-Specific Configuration

**Hardware Capabilities:**
- ARM Cortex-M33 @ 260 MHz with TrustZone-M
- 1.2 MB on-chip SRAM
- WiFi 6 (802.11ax) dual-band 2.4/5 GHz, up to +21 dBm TX power
- Bluetooth 5.4 with 2 Mbps, Long Range, up to +15 dBm TX power
- 802.15.4 / Thread / Matter support
- EdgeLock Secure Enclave for secure boot and crypto

**Required Kconfig Options:**
```
CONFIG_SOC_SERIES_RW6XX=y
CONFIG_BOARD_RW612=y  # or specific RW61x variant
CONFIG_WIFI_NXP=y
CONFIG_BT_NXP=y
CONFIG_ARM_TRUSTZONE_M=y  # if using TrustZone
```

**WiFi Stack Configuration:**
```
CONFIG_WIFI=y
CONFIG_WIFI_NXP=y
CONFIG_NET_L2_ETHERNET=y
CONFIG_NET_IPV4=y
CONFIG_NET_IPV6=y
CONFIG_NET_TCP=y
CONFIG_NET_UDP=y
CONFIG_NET_DHCPV4=y
CONFIG_WIFI_MGMT_EXT=y
```

**BLE Stack Configuration:**
```
CONFIG_BT=y
CONFIG_BT_NXP=y
CONFIG_BT_PERIPHERAL=y
CONFIG_BT_CENTRAL=y
CONFIG_BT_GATT_CLIENT=y
CONFIG_BT_GATT_DYNAMIC_DB=y
CONFIG_BT_SETTINGS=y  # for bonding persistence
```

## Confidence Assessment

| Component | Confidence | Reason |
|-----------|-----------|--------|
| Core RTOS (Zephyr 4.3.0) | HIGH | Official docs, Context7 verification, recent release (Nov 2025) |
| NXP ZSDK | MEDIUM | Official NXP repo verified, v4.3.0 released Dec 2025, limited public documentation |
| WiFi Stack | MEDIUM | Zephyr docs confirm NXP WiFi driver exists, CONFIG_WIFI_NXP verified in source |
| BLE Stack | HIGH | Zephyr BLE 5.3+ support well-documented, mature implementation |
| Development Tools | HIGH | Official Zephyr documentation, verified versions from PyPI and GitHub |
| RW61x Support | MEDIUM | Hardware specs from NXP website, driver existence confirmed, limited sample documentation |

## Sources

- Zephyr Project Documentation (https://docs.zephyrproject.org/latest/) — Core architecture, APIs, configuration
- NXP ZSDK Repository (https://github.com/nxp-zephyr/nxp-zsdk/tree/nxp-v4.3-branch) — RW61x support verification
- Context7: /zephyrproject-rtos/zephyr — WiFi/BLE driver configuration, build system
- NXP RW612 Product Page (https://www.nxp.com/products/wireless-connectivity/) — Hardware specifications
- West PyPI (https://pypi.org/project/west/) — Version 1.5.0 verification
- Zephyr Releases (https://docs.zephyrproject.org/latest/releases/) — Version 4.3.0 and LTS information

## Open Questions for Phase-Specific Research

1. **RW61x Board Definition**: Exact board name in NXP ZSDK (e.g., `rw612_evk`, `rw610_evk`) needs verification from boards directory
2. **WiFi/BLE Coexistence Samples**: Detailed analysis of `samples/wireless/coex/` needed to understand coexistence patterns
3. **Flash Partitioning**: RW61x-specific flash layout for MCUboot dual-image setup needs investigation
4. **Power Management**: RW61x low-power mode integration with Zephyr PM subsystem requires testing
5. **Debugging Setup**: Specific debug probe recommendations for RW61x (J-Link vs pyOCD performance)
6. **OTA Transport**: Best practice for OTA on RW61x (BLE vs WiFi UDP) based on use case
7. **Thread/Matter Support**: If 802.15.4 needed, additional stack configuration research required

---
*Stack research for: RW61x Zephyr RTOS firmware with WiFi/BLE*
*Researched: 2026-02-28*
