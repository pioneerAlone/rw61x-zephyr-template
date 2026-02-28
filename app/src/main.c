/*
 * main.c — RW61x Zephyr Template Application
 *
 * Minimal application demonstrating:
 * - Zephyr kernel initialization
 * - Logging subsystem usage (LOG_INF, LOG_DBG)
 * - Board configuration via CONFIG_BOARD
 *
 * This is a template starting point. Extend with WiFi, BLE, or other features.
 */

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

// Register this file as a logging module with INFO level
LOG_MODULE_REGISTER(main, LOG_LEVEL_INF);

int main(void)
{
    LOG_INF("RW61x Zephyr template v1.0");
    LOG_INF("Board: %s", CONFIG_BOARD);
    LOG_DBG("Debug logging enabled (if LOG_DEFAULT_LEVEL >= 4)");

    // Main application loop
    while (1) {
        k_sleep(K_SECONDS(5));
        LOG_INF("Heartbeat");
    }

    return 0;
}
