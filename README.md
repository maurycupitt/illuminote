# Illuminote

A light-guided piano that teaches users to play Christmas carols without reading music. LEDs indicate which note to play next, proximity sensors detect hand position, and MIDI output triggers instrument sounds in Ableton Live.

## How It Works

1. A song is stored as an array of MIDI note numbers in the Arduino sketch
2. An LED lights up indicating which key/sensor to touch next
3. When the user places their hand over the correct sensor, the system:
   - Sends a MIDI note-on message to Ableton
   - Turns off the current LED
   - Lights up the LED for the next note
4. Ableton receives the MIDI and plays the corresponding instrument sound
5. When the song completes, it automatically restarts after 5 seconds

## Hardware Components

| Component | Description |
|-----------|-------------|
| SparkFun Qwiic Pro Micro USB-C (ATmega32U4) | Microcontroller with native USB MIDI support |
| VL53L1X Time-of-Flight Sensors (x8) | Proximity sensors detecting hand position (50-200mm range) |
| SparkFun Qwiic Mux 8-Channel TCA9548A (x2) | I2C multiplexers to address multiple identical sensors |
| 16-Channel Relay Board | 5V relay interface with optocoupler isolation (LM2576) for LED control |
| 12V LED Recessed Lights (x8) | 3W warm white (3000K, 300 lumens) visual indicators |

### Pin Mapping

| LED Pin | MIDI Note | Note Name |
|---------|-----------|-----------|
| 10 | 62 | D4 |
| 16 | 64 | E4 |
| 4 | 66 | F#4 |
| 5 | 67 | G4 |
| 6 | 69 | A4 |
| 7 | 71 | B4 |
| 8 | 72 | C5 |
| 9 | 74 | D5 |

## Software Components

### Arduino Sketch

The main codebase is located in `away_in_a_manger_2025/away_in_a_manger_2025.ino`.

**Key Libraries:**
- `MIDIUSB` - USB MIDI communication
- `VL53L1X` - Time-of-flight sensor driver
- `SparkFun_I2C_Mux_Arduino_Library` - I2C multiplexer control

**Core Functions:**
- `checkNoteHit()` - Validates if the correct sensor was triggered
- `promptNext()` - Lights the LED for the next note in the song
- `resetSong()` - Resets to the beginning when song completes
- `noteOn()`/`noteOff()` - MIDI message transmission

### Ableton Live Projects

| File | Description |
|------|-------------|
| `2025-organ.als` | Current organ instrument setup |
| `2020-organ.als` | Previous year's organ configuration |
| `silentnight.als` | Silent Night specific project |

## Song Library

Songs are stored in the `midi-songs/` directory with multiple format stages:

| Format | Purpose |
|--------|---------|
| `.mid` | Original MIDI file |
| `.csv` | Parsed MIDI data (via midicsv) |
| `.txt` | Extracted note sequence (one per line) |

### Available Songs

- Away in a Manger
- Joy to the World
- O Come All Ye Faithful
- The First Noel
- Hark the Herald Angels Sing
- Up on the Housetop
- We Wish You a Merry Christmas

### Converting MIDI to Note Array

To extract note numbers from a MIDI file for use in the Arduino sketch:

```bash
midicsv song.mid | awk -F, '/Note_on_c/ && $6 > 0 {print $5}' | paste -sd, -
```

This outputs a comma-separated list of MIDI note numbers ready to paste into the `song[]` array.

## Directory Structure

```
illuminote/
├── away_in_a_manger_2025/     # Current Arduino sketch
│   └── away_in_a_manger_2025.ino
├── midi-songs/                 # Song files in various formats
│   ├── *.mid                   # Original MIDI
│   ├── *.csv                   # Parsed MIDI data
│   └── *.txt                   # Note sequences
├── lyrics_slides/              # Lyric presentation slides
│   └── 2020-silent-night.pptx
├── Samples/                    # Audio samples for Ableton
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
