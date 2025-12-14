# Illuminote

A light-guided piano that teaches users to play Christmas carols without reading music. LEDs indicate which note to play next, proximity sensors detect hand position, and MIDI output triggers instrument sounds in Ableton Live.

## How It Works

### Song Selection

At startup (and after each song ends), the system enters song selection mode:

1. LEDs 1-7 light up, each representing an available song (alphabetically ordered)
2. The player hovers their hand over a sensor to select that song
3. The selected song begins playing

### Playing a Song

1. An LED lights up indicating which key/sensor to touch next
2. When the user places their hand over the correct sensor, the system:
   - Sends a MIDI note-on message to Ableton
   - Turns off the current LED
   - Lights up the LED for the next note
3. Ableton receives the MIDI and plays the corresponding instrument sound
4. When the song completes, it returns to song selection after 3 seconds

## Hardware Components

| Component | Description |
|-----------|-------------|
| SparkFun Qwiic Pro Micro USB-C (ATmega32U4) | Microcontroller with native USB MIDI support |
| VL53L1X Time-of-Flight Sensors (x10) | Proximity sensors detecting hand position (50-200mm range) |
| SparkFun Qwiic Mux 8-Channel TCA9548A (x2) | I2C multiplexers - Mux 1 at 0x70 (sensors 0-7), Mux 2 at 0x71 (sensors 8-9) |
| 16-Channel Relay Board | 5V relay interface with optocoupler isolation (LM2576) for LED control |
| 12V LED Recessed Lights (x10) | 3W warm white (3000K, 300 lumens) visual indicators |

### Pin Mapping

| Sensor | LED Pin | MIDI Note | Note Name |
|--------|---------|-----------|-----------|
| 0 | 10 | 62 | D4 |
| 1 | 16 | 64 | E4 |
| 2 | 4 | 66 | F#4 |
| 3 | 5 | 67 | G4 |
| 4 | 6 | 69 | A4 |
| 5 | 7 | 71 | B4 |
| 6 | 8 | 72 | C5 |
| 7 | 9 | 74 | D5 |
| 8 | 14 | 76 | E5 |
| 9 | 15 | 77 | F5 |

## Software Components

### Arduino Sketch

The main sketch is located in `illuminote/illuminote.ino`. Songs are stored as header files in `illuminote/songs/` and registered in `illuminote/songs.h`.

**Key Libraries:**
- `MIDIUSB` - USB MIDI communication
- `VL53L1X` - Time-of-flight sensor driver
- `SparkFun_I2C_Mux_Arduino_Library` - I2C multiplexer control

**Core Functions:**
- `enterSongSelection()` - Enters song selection mode, lighting LEDs for available songs
- `selectSong()` - Loads the selected song and begins playback
- `selectMuxPort()` - Routes sensor index to correct mux (0-7 → mux1, 8-9 → mux2)
- `checkNoteHit()` - Validates if the correct sensor was triggered
- `promptNext()` - Lights the LED for the next note in the song
- `noteOn()`/`noteOff()` - MIDI message transmission

### Ableton Live Projects

| File | Description |
|------|-------------|
| `2025-organ.als` | Current organ instrument setup |

## Song Library

Songs are stored in the `midi-songs/` directory with multiple format stages:

| Format | Purpose |
|--------|---------|
| `.mid` | Original MIDI file |
| `.csv` | Parsed MIDI data (via midicsv) |
| `.txt` | Extracted note sequence (one per line) |

### Available Songs

| LED | Song | Header File | Notes |
|-----|------|-------------|-------|
| 1 | Away in a Manger | `away_in_a_manger.h` | 44 notes, 8 unique |
| 2 | Hark the Herald Angels Sing | `hark_the_herald.h` | 78 notes, 9 unique |
| 3 | Joy to the World | `joy_to_the_world.h` | 59 notes, 8 unique |
| 4 | O Come All Ye Faithful | `o_come_all_ye_faithful.h` | 60 notes, 8 unique |
| 5 | The First Noel | `the_first_noel.h` | 71 notes, 8 unique |
| 6 | Up on the Housetop | `up_on_the_housetop.h` | 68 notes, 6 unique |
| 7 | We Wish You a Merry Christmas | `we_wish_you_a_merry_christmas.h` | 56 notes, 8 unique |

### Converting MIDI to Note Array

To extract note numbers from a MIDI file for use in the Arduino sketch:

```bash
midicsv song.mid | awk -F, '/Note_on_c/ && $6 > 0 {print $5}' | paste -sd, -
```

This outputs a comma-separated list of MIDI note numbers ready to paste into the `song[]` array.

## Directory Structure

```
illuminote/
├── illuminote/                 # Arduino sketch
│   ├── illuminote.ino          # Main sketch
│   ├── songs.h                 # Song registry
│   └── songs/                  # Song header files
│       ├── away_in_a_manger.h
│       ├── hark_the_herald.h
│       ├── joy_to_the_world.h
│       ├── o_come_all_ye_faithful.h
│       ├── the_first_noel.h
│       ├── up_on_the_housetop.h
│       └── we_wish_you_a_merry_christmas.h
├── midi-songs/                 # Song files in various formats
│   ├── *.mid                   # Original MIDI
│   ├── *.csv                   # Parsed MIDI data
│   └── *.txt                   # Note sequences
├── lyrics_slides/              # Lyric presentation slides
│   └── 2020-silent-night.pptx
├── *.als                       # Ableton Live project files
└── README.md
```

## Future Features

### Automatic Lyric Slide Advancement

The `lyrics_slides/` folder contains PowerPoint presentations with song lyrics. A planned feature will automatically advance slides as the song progresses, displaying the current lyric phrase in sync with the notes being played.

**Potential Implementation Approaches:**
- Arduino Keyboard library to send keystroke commands alongside MIDI
- MIDI Control Change messages to trigger slide software (Qlab, custom script)
- Serial-to-AppleScript bridge on macOS for PowerPoint control
- Integration with Ableton's video/clip launching capabilities

This would enable a complete sing-along experience where users see both which note to play and the corresponding lyrics to sing.
