# Changelog

## 2025-12-14

### Restructured Arduino Sketch

- Renamed sketch from `away_in_a_manger_2025/` to `illuminote/illuminote.ino`
- Created `illuminote/songs/` folder for song header files
- Moved song data to `songs/away_in_a_manger.h`
- To change songs, edit the `#include` directive in `illuminote.ino`

### Code Quality Improvements

- **Removed dead code** - Deleted unused `handleSensorState()` function (~60 lines) and its associated variables (`sensorStates`, `lastStates`)
- **Consolidated constants** - Replaced `NUMBER_OF_SENSORS`, `TOTAL_SENSORS`, `LIVE_SENSORS` with single `NUM_SENSORS`
- **Fixed out-of-bounds access** - Wrapped `song[currentNote - 1]` access in `promptNext()` inside a `currentNote > 0` check
- **Replaced dynamic allocation** - Changed `VL53L1X **distanceSensor` (heap-allocated) to `VL53L1X distanceSensors[NUM_SENSORS]` (static allocation)
- **Removed duplicate flush** - Removed redundant `MidiUSB.flush()` call in `checkNoteHit()` since `noteOn()` already calls it
