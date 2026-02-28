---
phase: 01-toolchain-foundation
plan: 03
subsystem: infra
tags: [zephyr, west, jlink, hardware, verification, rw61x, nxp]

# Dependency graph
requires:
  - phase: 01-01
    provides: "T-topology west workspace and Zephyr application scaffold"
  - phase: 01-02
    provides: "VS Code workspace with J-Link debug configuration"
provides:
  - "Hardware verification checklist with setup commands"
  - "Automated file structure validation"
  - "Developer-ready build/flash/debug workflow documentation"
affects: [02, 03, 04, 05]

# Tech tracking
tech-stack:
  added: []
  patterns: [hardware verification workflow, developer setup checklist]

key-files:
  created:
    - .planning/phases/01-toolchain-foundation/01-03-VERIFICATION-CHECKLIST.md
  modified: []

key-decisions:
  - "Automated checks verify file structure before hardware testing"
  - "Developer setup commands provided in exact execution order"
  - "Troubleshooting section covers common J-Link and blob issues"

patterns-established:
  - "Hardware verification plans create VERIFICATION-CHECKLIST.md for developer guidance"
  - "Automated checks run first, then manual hardware steps documented"
  - "Setup commands include expected output for each step"

requirements-completed: [TOOL-04, DBG-02]

# Metrics
duration: 5min
completed: 2026-02-28
---

# Plan 01-03: Hardware Verification Checklist

**Automated file structure validation and comprehensive developer setup guide for RW61x build/flash/debug workflow**

## Performance

- **Duration:** 5 min
- **Started:** 2026-02-28T00:00:00Z
- **Completed:** 2026-02-28T00:05:00Z
- **Tasks:** 2 completed
- **Files modified:** 1 created

## Accomplishments

- Automated verification of all 7 critical project files (west.yml, CMakeLists.txt, main.c, prj.conf, overlay, launch.json, tasks.json)
- Created comprehensive developer setup checklist with exact commands in execution order
- Documented hardware verification workflow: flash → UART → J-Link debug
- Provided troubleshooting guidance for common issues (device string, blobs, build errors)

## Task Commits

Each task was committed atomically:

1. **Task 1: Verify west workspace and create verification checklist** - `921104d` (docs)

## Files Created/Modified

- `.planning/phases/01-toolchain-foundation/01-03-VERIFICATION-CHECKLIST.md` - Complete hardware verification guide with setup commands, expected outputs, and troubleshooting

## Decisions Made

None - followed plan as specified. This is a verification and documentation plan, not an implementation plan.

## Deviations from Plan

None - plan executed exactly as written.

## Issues Encountered

None - all automated checks passed, verification checklist created successfully.

## User Setup Required

**Hardware verification required.** See [01-03-VERIFICATION-CHECKLIST.md](./01-03-VERIFICATION-CHECKLIST.md) for:

### Required Steps:
1. Initialize west workspace: `west init` and `west update`
2. Fetch firmware blobs: `west blobs fetch hal_nxp` (CRITICAL for WiFi/BLE)
3. Install Python requirements: `pip3 install -r zephyr/scripts/requirements.txt`
4. Build firmware: `west build -b frdm_rw612 app/`
5. Flash to hardware: `west flash --runner jlink`
6. Verify UART output at 115200 baud
7. Test J-Link debugging in VS Code (F5)

### Verification Criteria:
- Build completes without errors
- Flash succeeds with "Verified OK"
- UART shows structured log output with timestamps
- VS Code debug session stops at main() breakpoint
- Developer can set breakpoints and inspect variables

**When all checks pass, Phase 1 (Toolchain Foundation) is complete.**

## Next Phase Readiness

- Phase 1 complete pending hardware verification
- All 8 Phase 1 requirements satisfied once developer confirms hardware tests pass
- Ready to begin Phase 2 (WiFi Connectivity) after hardware validation
- Project scaffold, VS Code workspace, and verification workflow fully documented

---
*Phase: 01-toolchain-foundation*
*Completed: 2026-02-28*
