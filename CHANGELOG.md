# Changelog

## 2025-12-14

### Added 3 New Songs

- **Hark the Herald Angels Sing** - 78 notes, 9 unique notes
- **Up on the Housetop** - 68 notes (uses only 6 unique notes: 67, 69, 71, 72, 74, 76)
- **We Wish You a Merry Christmas** - 56 notes

Now 7 songs total in the selector.

### Song Selection Feature

- Added interactive song chooser at startup and between songs
- LEDs 1-4 light up to indicate available songs (alphabetically)
- Player selects a song by hovering over the corresponding sensor
- After song completes, returns to song selection mode
- Songs stored in PROGMEM to save RAM
- Created `songs.h` registry with all songs and metadata

### Restructured Arduino Sketch

- Renamed sketch from `away_in_a_manger_2025/` to `illuminote/illuminote.ino`
- Created `illuminote/songs/` folder for song header files
- All 4 songs now included: Away in a Manger, Joy to the World, O Come All Ye Faithful, The First Noel

### Code Quality Improvements

- **Removed dead code** - Deleted unused `handleSensorState()` function (~60 lines) and its associated variables (`sensorStates`, `lastStates`)
- **Consolidated constants** - Replaced `NUMBER_OF_SENSORS`, `TOTAL_SENSORS`, `LIVE_SENSORS` with single `NUM_SENSORS`
- **Fixed out-of-bounds access** - Wrapped `song[currentNote - 1]` access in `promptNext()` inside a `currentNote > 0` check
- **Replaced dynamic allocation** - Changed `VL53L1X **distanceSensor` (heap-allocated) to `VL53L1X distanceSensors[NUM_SENSORS]` (static allocation)
- **Removed duplicate flush** - Removed redundant `MidiUSB.flush()` call in `checkNoteHit()` since `noteOn()` already calls it
