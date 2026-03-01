# Architecture Research

**Domain:** Zephyr RTOS Firmware for WiFi/BLE IoT Devices (NXP RW61x)
**Researched:** 2026-02-28
**Confidence:** HIGH

## Standard Architecture

### System Overview

```
┌─────────────────────────────────────────────────────────────────┐
│                     Application Layer                            │
│  ┌──────────────┐  ┌──────────────┐  ┌──────────────┐          │
│  │ WiFi Manager │  │ BLE Services │  │ App Logic    │          │
│  └──────┬───────┘  └──────┬───────┘  └──────┬───────┘          │
├─────────┴──────────────────┴──────────────────┴──────────────────┤
│                     Subsystems Layer                             │
│  ┌──────────┐  ┌──────────┐  ┌──────────┐  ┌──────────┐        │
│  │ Network  │  │Bluetooth │  │ Settings │  │  Logging │        │
│  │  Stack   │  │   Host   │  │ Storage  │  │  Shell   │        │
│  └────┬─────┘  └────┬─────┘  └────┬─────┘  └────┬─────┘        │
├───────┴──────────────┴──────────────┴──────────────┴─────────────┤
│                      Kernel Layer                                │
│  ┌──────────────────────────────────────────────────────────┐   │
│  │  Threads | Scheduling | Memory | Sync Primitives | IPC  │   │
│  └────────────────────────────┬─────────────────────────────┘   │
├────────────────────────────────┴──────────────────────────────────┤
│                   Hardware Abstraction Layer                     │
│  ┌──────────┐  ┌──────────┐  ┌──────────┐  ┌──────────┐        │
│  │  WiFi    │  │   BLE    │  │  GPIO    │  │  UART    │        │
│  │  Driver  │  │  Driver  │  │  Driver  │  │  Driver  │        │
│  └────┬─────┘  └────┬─────┘  └────┬─────┘  └────┬─────┘        │
├───────┴──────────────┴──────────────┴──────────────┴─────────────┤
│                      Hardware (RW61x)                            │
│  WiFi 6 Radio | BLE 5.3 Radio | Peripherals | Flash | RAM       │
└─────────────────────────────────────────────────────────────────┘
```

### Component Responsibilities

| Component | Responsibility | Typical Implementation |
|-----------|----------------|------------------------|
| **Application Layer** | Business logic, user features, state machines | Custom C code using Zephyr APIs |
| **Network Stack** | TCP/IP, UDP, sockets, WiFi L2 management | Zephyr net subsystem (IPv4/IPv6) |
| **Bluetooth Host** | BLE stack, GATT services, connections, advertising | Zephyr BT subsystem with HCI interface |
| **Kernel** | Thread scheduling, memory management, IPC | Zephyr kernel (preemptive/cooperative) |
| **Device Drivers** | Hardware abstraction for peripherals | Zephyr driver API + NXP HAL |
| **Settings Storage** | Persistent configuration (WiFi credentials, etc.) | NVS (Non-Volatile Storage) or FCB |
| **Logging/Shell** | Debug output, runtime diagnostics | Zephyr logging framework + shell |
| **MCUboot** | Secure bootloader, firmware updates | MCUboot with image signing |

## Recommended Project Structure

```
rw61x-zephyr-template/
├── CMakeLists.txt              # Build system entry point
├── prj.conf                    # Kconfig configuration
├── boards/
│   └── rw61x.overlay           # Board-specific devicetree overlay
├── src/
│   ├── main.c                  # Application entry point
│   ├── wifi/
│   │   ├── wifi_mgr.c          # WiFi connection management
│   │   └── wifi_mgr.h
│   ├── ble/
│   │   ├── ble_service.c       # BLE GATT services
│   │   └── ble_service.h
│   ├── app/
│   │   ├── app_logic.c         # Application-specific logic
│   │   └── app_config.c        # Configuration management
│   └── utils/
│       ├── logging.c           # Logging utilities
│       └── shell_cmds.c        # Custom shell commands
├── child_image/
│   └── mcuboot/
│       └── prj.conf            # MCUboot configuration
└── VERSION                     # Application version
```

### Structure Rationale

- **src/wifi/**: Isolates WiFi management (scanning, connecting, credentials) from application logic
- **src/ble/**: Encapsulates BLE services, advertising, and connection handling
- **src/app/**: Application-specific business logic, keeps domain logic separate from connectivity
- **src/utils/**: Shared utilities for logging, shell commands, and helper functions
- **boards/**: Hardware-specific devicetree overlays for pin configuration and peripheral setup
- **child_image/mcuboot/**: Bootloader configuration for OTA updates

## Architectural Patterns

### Pattern 1: Thread-per-Subsystem

**What:** Dedicate separate threads for WiFi management, BLE operations, and application logic

**When to use:** When subsystems have independent timing requirements and blocking operations

**Trade-offs:**
- Pros: Clean separation, independent scheduling, easier debugging
- Cons: Higher memory usage (stack per thread), context switching overhead

**Example:**
```c
#define WIFI_STACK_SIZE 2048
#define BLE_STACK_SIZE 2048
#define APP_STACK_SIZE 4096

K_THREAD_DEFINE(wifi_thread, WIFI_STACK_SIZE,
                wifi_manager_thread, NULL, NULL, NULL,
                K_PRIO_PREEMPT(7), 0, 0);

K_THREAD_DEFINE(ble_thread, BLE_STACK_SIZE,
                ble_service_thread, NULL, NULL, NULL,
                K_PRIO_PREEMPT(7), 0, 0);

K_THREAD_DEFINE(app_thread, APP_STACK_SIZE,
                app_logic_thread, NULL, NULL, NULL,
                K_PRIO_PREEMPT(8), 0, 0);
```

### Pattern 2: Event-Driven Network Management

**What:** Use Zephyr's net_mgmt event callbacks for WiFi state changes

**When to use:** For responding to network events (connected, disconnected, scan results)

**Trade-offs:**
- Pros: Asynchronous, non-blocking, integrates with Zephyr networking
- Cons: Callback context limitations, requires careful state management

**Example:**
```c
static struct net_mgmt_event_callback wifi_cb;

static void wifi_event_handler(struct net_mgmt_event_callback *cb,
                                uint32_t mgmt_event, struct net_if *iface) {
    switch (mgmt_event) {
    case NET_EVENT_WIFI_CONNECT_RESULT:
        // Handle connection result
        break;
    case NET_EVENT_WIFI_DISCONNECT_RESULT:
        // Handle disconnection
        break;
    }
}

void wifi_init(void) {
    net_mgmt_init_event_callback(&wifi_cb, wifi_event_handler,
                                  NET_EVENT_WIFI_CONNECT_RESULT |
                                  NET_EVENT_WIFI_DISCONNECT_RESULT);
    net_mgmt_add_event_callback(&wifi_cb);
}
```

### Pattern 3: Devicetree-Based Hardware Configuration

**What:** Use devicetree overlays for hardware pin configuration and peripheral setup

**When to use:** Always - this is the Zephyr way for hardware abstraction

**Trade-offs:**
- Pros: Hardware-independent code, compile-time validation, board portability
- Cons: Learning curve, requires understanding DTS syntax

**Example:**
```devicetree
&uart1 {
    status = "okay";
    current-speed = <115200>;
    pinctrl-0 = <&uart1_default>;
    pinctrl-names = "default";
};

&i2c0 {
    status = "okay";
    clock-frequency = <I2C_BITRATE_FAST>;

    sensor@48 {
        compatible = "ti,tmp112";
        reg = <0x48>;
    };
};
```

### Pattern 4: Kconfig-Driven Feature Selection

**What:** Use Kconfig for compile-time feature selection and configuration

**When to use:** For enabling/disabling subsystems, tuning stack sizes, selecting protocols

**Trade-offs:**
- Pros: Optimized binary size, compile-time validation, clear dependencies
- Cons: Requires rebuild for configuration changes

**Example:**
```kconfig
# prj.conf
CONFIG_WIFI=y
CONFIG_WIFI_NXP=y
CONFIG_NET_L2_WIFI_MGMT=y

CONFIG_BT=y
CONFIG_BT_PERIPHERAL=y
CONFIG_BT_DEVICE_NAME="RW61x-Device"

CONFIG_NETWORKING=y
CONFIG_NET_IPV4=y
CONFIG_NET_TCP=y
CONFIG_NET_SOCKETS=y

CONFIG_SETTINGS=y
CONFIG_SETTINGS_NVS=y
CONFIG_NVS=y
```

## Data Flow

### WiFi Connection Flow

```
[User/App Request]
    ↓
[WiFi Manager] → net_mgmt_wifi_connect() → [Network Stack]
    ↓                                            ↓
[Event Callback] ← NET_EVENT_WIFI_CONNECT ← [WiFi Driver]
    ↓                                            ↓
[DHCP Client] → IP Address Assignment ← [Network Interface]
    ↓
[Application Notified] → TCP/UDP Sockets Ready
```

### BLE GATT Service Flow

```
[BLE Init] → bt_enable() → [Bluetooth Host]
    ↓
[Define GATT Service] → BT_GATT_SERVICE_DEFINE()
    ↓
[Start Advertising] → bt_le_adv_start()
    ↓
[Central Connects] → connected_callback()
    ↓
[GATT Read/Write] → read_callback() / write_callback()
    ↓
[Notify/Indicate] → bt_gatt_notify() → [Central Device]
```

### Firmware Update Flow

```
[New Firmware] → [OTA Transport (BLE/WiFi)]
    ↓
[Image Manager] → img_mgmt_impl_write()
    ↓
[Flash Write] → flash_area_write() → [Slot 1 Partition]
    ↓
[Mark Pending] → boot_request_upgrade()
    ↓
[Reboot] → sys_reboot()
    ↓
[MCUboot] → Verify Signature → Swap Images → [Boot New Firmware]
```

### Key Data Flows

1. **WiFi to Application:** Network events propagate through net_mgmt callbacks to application threads via message queues or semaphores
2. **BLE to Application:** GATT callbacks execute in BT thread context, use work queues to defer processing to application thread
3. **Settings Persistence:** Application writes to settings subsystem, which commits to NVS flash storage asynchronously
4. **Logging:** All components log through unified logging framework, output to UART console or network backend

## Scaling Considerations

| Scale | Architecture Adjustments |
|-------|--------------------------|
| Prototype (single device) | Monolithic application, all features in main thread, simple state machine |
| Small deployment (10-100 devices) | Thread-per-subsystem, basic OTA, centralized logging, WiFi credentials in NVS |
| Production (1000+ devices) | Optimize memory (CONFIG_SIZE_OPTIMIZATIONS), implement watchdog, robust error handling, fleet management integration |

**Memory Optimization:**
- Use CONFIG_MINIMAL_LIBC for smaller footprint
- Tune thread stack sizes (use CONFIG_THREAD_STACK_INFO to measure actual usage)
- Disable unused subsystems (Bluetooth if WiFi-only, IPv6 if not needed)
- Use CONFIG_LOG_MODE_MINIMAL for production builds

**Power Optimization:**
- Enable CONFIG_PM and CONFIG_PM_DEVICE for power management
- Use WiFi power save modes (CONFIG_WIFI_NM_WPA_SUPPLICANT_WNM)
- Implement BLE connection interval tuning
- Use deep sleep between operations

## Common Anti-Patterns

### Anti-Pattern 1: Blocking in Callback Context

**What it looks like:** Calling k_sleep() or blocking operations in net_mgmt or BT callbacks

**Why it's wrong:** Callbacks execute in system work queue context, blocking stalls other system operations

**Do this instead:** Use work queues to defer processing
```c
static void wifi_event_handler(struct net_mgmt_event_callback *cb,
                                uint32_t mgmt_event, struct net_if *iface) {
    // BAD: k_sleep(K_SECONDS(1));

    // GOOD: Submit work to dedicated work queue
    k_work_submit(&wifi_reconnect_work);
}
```

### Anti-Pattern 2: Hardcoded Hardware Configuration

**What it looks like:** Direct register access or hardcoded pin numbers in C code

**Why it's wrong:** Breaks portability, bypasses Zephyr's hardware abstraction

**Do this instead:** Use devicetree and device API
```c
// BAD: #define LED_PIN 13
// GOOD:
#define LED_NODE DT_ALIAS(led0)
static const struct gpio_dt_spec led = GPIO_DT_SPEC_GET(LED_NODE, gpios);
```

### Anti-Pattern 3: Ignoring Return Values

**What it looks like:** Not checking return values from Zephyr APIs

**Why it's wrong:** Silent failures lead to undefined behavior and hard-to-debug issues

**Do this instead:** Always check and handle errors
```c
// BAD:
bt_le_adv_start(BT_LE_ADV_CONN, ad, ARRAY_SIZE(ad), NULL, 0);

// GOOD:
int err = bt_le_adv_start(BT_LE_ADV_CONN, ad, ARRAY_SIZE(ad), NULL, 0);
if (err) {
    LOG_ERR("Advertising failed to start (err %d)", err);
    return err;
}
```

### Anti-Pattern 4: Insufficient Stack Sizes

**What it looks like:** Thread stack overflows causing crashes or corruption

**Why it's wrong:** Zephyr threads have fixed stack sizes, overflow causes undefined behavior

**Do this instead:** Use CONFIG_THREAD_STACK_INFO to measure actual usage, add 25% margin
```c
// Enable in prj.conf:
// CONFIG_THREAD_STACK_INFO=y

// Check usage at runtime:
size_t unused;
k_thread_stack_space_get(&my_thread, &unused);
LOG_INF("Thread stack unused: %zu bytes", unused);
```

## Integration Points

### External Services

| Service | Integration Pattern | Notes |
|---------|---------------------|-------|
| Cloud MQTT | BSD sockets + MQTT library | Use net_app_get_default_ctx() for network interface |
| BLE Mobile App | GATT services + notifications | Define custom UUIDs, implement read/write callbacks |
| Local Network | mDNS/DNS-SD | Use CONFIG_MDNS_RESPONDER for service discovery |
| Time Sync | SNTP client | CONFIG_SNTP for network time synchronization |
| OTA Server | HTTP/HTTPS client | Use http_client API with TLS for secure updates |

### Internal Boundaries

| Boundary | Communication | Notes |
|----------|---------------|-------|
| App ↔ WiFi Manager | Message queues (k_msgq) | Async requests for connect/disconnect/scan |
| App ↔ BLE Service | Direct function calls + callbacks | BLE callbacks use work queues for app notification |
| Subsystems ↔ Settings | Settings API (settings_save_one) | Persistent storage for configuration |
| All ↔ Logging | LOG_* macros | Unified logging with module-specific tags |
| Threads ↔ Threads | Semaphores, mutexes, message queues | Use k_sem, k_mutex, k_msgq for synchronization |

## Build Order and Dependencies

### Phase 1: Foundation (Build First)
1. **Devicetree configuration** - Hardware pin mappings, peripheral definitions
2. **Kconfig setup** - Enable required subsystems (networking, Bluetooth, drivers)
3. **Basic application skeleton** - main.c with initialization sequence

### Phase 2: Core Connectivity
4. **WiFi driver integration** - Verify NXP WiFi driver builds and initializes
5. **BLE stack initialization** - Enable Bluetooth, verify HCI communication
6. **Network stack** - Configure IP stack, DHCP client

### Phase 3: Application Features
7. **WiFi management layer** - Connection state machine, credential storage
8. **BLE services** - GATT service definitions, characteristic handlers
9. **Settings storage** - NVS integration for persistent configuration

### Phase 4: Production Features
10. **MCUboot integration** - Bootloader, flash partitions, image signing
11. **Logging and diagnostics** - Shell commands, runtime debugging
12. **Power management** - Sleep modes, power optimization

### Dependency Notes
- WiFi and BLE can be developed in parallel (independent subsystems)
- Settings storage depends on flash driver being functional
- MCUboot requires understanding of flash partition layout (define early)
- Shell/logging can be added incrementally throughout development

## NXP RW61x Specific Considerations

### Hardware Capabilities
- **WiFi 6 (802.11ax)** - Requires CONFIG_WIFI_NXP with appropriate firmware blobs
- **BLE 5.3** - Dual-mode operation (WiFi + BLE concurrent)
- **Coexistence** - Hardware arbitration between WiFi and BLE radios
- **Flash Layout** - Typically 4MB external flash for code + data

### NXP SDK Integration
- **nxp_zsdk** provides RW61x-specific drivers and board definitions
- **HAL dependencies** - NXP HAL may be required for WiFi/BLE drivers
- **Binary blobs** - WiFi firmware likely requires proprietary blobs (check licensing)
- **Board definition** - Ensure rw61x board files exist in nxp_zsdk

### Known Limitations (to verify)
- WiFi driver maturity in Zephyr (may have limited feature support)
- BLE controller implementation (software vs hardware controller)
- Power management support (verify PM_DEVICE support for RW61x)
- Documentation completeness (NXP downstream may have sparse docs)

## Sources

**HIGH Confidence:**
- [Zephyr Project Official Documentation](https://docs.zephyrproject.org/latest/) - Architecture, kernel, subsystems
- [Zephyr GitHub Repository](https://github.com/zephyrproject-rtos/zephyr) - Source code, examples, devicetree bindings
- Context7 Zephyr Documentation (/zephyrproject-rtos/zephyr) - API examples, configuration patterns

**MEDIUM Confidence:**
- Zephyr Application Development Guide - Project structure, build system
- Zephyr Bluetooth Documentation - BLE stack architecture
- Zephyr Networking Documentation - WiFi management, TCP/IP stack

**Areas Requiring Validation:**
- NXP RW61x specific driver capabilities (WiFi 6 feature support, BLE 5.3 features)
- nxp_zsdk board definition completeness for RW61x
- WiFi/BLE coexistence implementation details
- Power management support maturity for RW61x platform

---
*Architecture research for: RW61x Zephyr RTOS Firmware Template*
*Researched: 2026-02-28*
