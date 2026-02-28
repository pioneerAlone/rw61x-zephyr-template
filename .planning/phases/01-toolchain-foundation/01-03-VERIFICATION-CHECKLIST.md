# RW61x Hardware Verification Checklist

**Phase:** 01-toolchain-foundation
**Plan:** 03
**Date:** 2026-02-28

---

## Automated Checks ✅

All file structure checks passed:
- ✓ west.yml contains nxp-v4.3-branch
- ✓ app/CMakeLists.txt contains find_package(Zephyr)
- ✓ app/src/main.c contains LOG_MODULE_REGISTER
- ✓ app/prj.conf contains CONFIG_LOG=y
- ✓ app/boards/frdm_rw612.overlay exists
- ✓ app/.vscode/launch.json contains jlink
- ✓ app/.vscode/tasks.json contains west blobs fetch

---

## Developer Setup Commands

**IMPORTANT:** Run these commands from the workspace root (`/Users/wangbo/test/claude_proj`), NOT inside `app/`.

### Step 1: Initialize West Workspace (if not done)

```bash
cd /Users/wangbo/test/claude_proj
west init -m https://github.com/nxp-zephyr/nxp_zsdk --mr nxp-v4.3-branch
west update
```

**Expected:** Downloads nxp_zsdk and all Zephyr dependencies. Takes 5-10 minutes.

### Step 2: Fetch RW61x Radio Firmware Blobs (CRITICAL)

```bash
west blobs fetch hal_nxp
west blobs list hal_nxp
```

**Expected:** Downloads WiFi/BLE firmware blobs. The `list` command should show all blobs as present.

### Step 3: Install Zephyr Python Requirements

```bash
pip3 install -r zephyr/scripts/requirements.txt
```

**Expected:** Installs west, PyYAML, and other build dependencies.

### Step 4: Build Firmware

```bash
west build -b frdm_rw612 app/
```

**Expected:** Build completes without errors. Output shows:
```
Memory region         Used Size  Region Size  %age Used
           FLASH:       XXXXX B       XXX KB     X.XX%
            SRAM:       XXXXX B       XXX KB     X.XX%
```

### Step 5: Verify Build Artifacts

```bash
ls -lh build/zephyr/zephyr.elf
ls -lh build/zephyr/zephyr.bin
```

**Expected:** Both files exist with reasonable sizes (typically 50-200KB).

---

## Hardware Verification Steps

### Step 1: Connect Hardware

1. Connect FRDM-RW612 board to computer via USB
2. Ensure J-Link debugger is connected (on-board or external)
3. Verify board power LED is on

### Step 2: Flash Firmware

```bash
west flash --runner jlink
```

**Expected:** J-Link connects, erases flash, programs device, verifies. Output ends with "Verified OK".

**If device string fails:** Run `JLinkExe -device ?` and search for RW612. Update `"device"` in `app/.vscode/launch.json` to the correct string (e.g., `MIMXRW612`).

### Step 3: Verify UART Output

1. Open serial terminal (screen, minicom, or VS Code Serial Monitor)
2. Connect to board's USB-serial port at 115200 baud
3. Press reset button on board

**Expected output:**
```
[00:00:00.000,000] <inf> main: RW61x Zephyr template v1.0
[00:00:00.000,000] <inf> main: Board: frdm_rw612
[00:00:01.000,000] <inf> main: Heartbeat: 1
[00:00:02.000,000] <inf> main: Heartbeat: 2
```

### Step 4: Verify J-Link Debug in VS Code

1. Open `app/` folder in VS Code
2. Install recommended extensions when prompted (especially cortex-debug)
3. Press F5 (or Run → Start Debugging → "Debug (J-Link)")
4. **Expected:** Build runs, J-Link connects, execution stops at `main()`
5. Set a breakpoint on the `LOG_INF` line, press Continue
6. **Expected:** Breakpoint hits, you can inspect variables

---

## Troubleshooting

### J-Link Device String Issues

If you see "Could not connect to target device":
```bash
JLinkExe -device ?
# Search output for RW612 or MIMXRW612
```

Update `app/.vscode/launch.json`:
```json
"device": "MIMXRW612"  // or whatever JLinkExe reports
```

### Missing Firmware Blobs

If build fails with "firmware blob not found":
```bash
west blobs list hal_nxp
# If any show as missing:
west blobs fetch hal_nxp
```

### Build Errors

If `west build` fails:
1. Check that `west update` completed successfully
2. Verify Python requirements are installed
3. Try pristine build: `west build -b frdm_rw612 app/ -p`

---

## Verification Checklist

- [ ] `west build -b frdm_rw612 app/` completes without errors
- [ ] `build/zephyr/zephyr.elf` and `zephyr.bin` exist
- [ ] `west flash --runner jlink` successfully programs the device
- [ ] UART terminal shows structured log output with timestamps
- [ ] VS Code F5 debug session starts and stops at main() breakpoint
- [ ] Developer can set breakpoints and inspect variables

---

**When all checks pass, Phase 1 (Toolchain Foundation) is complete!**
