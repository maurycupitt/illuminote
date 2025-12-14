#include <Wire.h>
#include <MIDIUSB.h>
#include <SparkFun_I2C_Mux_Arduino_Library.h>
#include <VL53L1X.h>

void noteOn(byte channel, byte pitch, byte velocity) {
  midiEventPacket_t noteOn = {0x09, 0x90 | channel, pitch, velocity};
  MidiUSB.sendMIDI(noteOn);
  MidiUSB.flush();
}

void noteOff(byte channel, byte pitch, byte velocity) {
  midiEventPacket_t noteOff = {0x08, 0x80 | channel, pitch, velocity};
  MidiUSB.sendMIDI(noteOff);
  MidiUSB.flush();
}

void controlChange(byte channel, byte control, byte value) {
  midiEventPacket_t event = {0x0B, 0xB0 | channel, control, value};
  MidiUSB.sendMIDI(event);
  MidiUSB.flush();
}

#define NUM_SENSORS 10
#define MIDI_NOTES_COUNT 127

#include "songs.h"

// LED pin assignments per sensor
const byte ledPins[NUM_SENSORS] = {10, 16, 4, 5, 6, 7, 8, 9, 14, 15};

QWIICMUX myMux;
QWIICMUX myMux2;
VL53L1X distanceSensors[NUM_SENSORS];

bool sensorActive[NUM_SENSORS] = {false};
int mapNoteToLed[MIDI_NOTES_COUNT] = {};
byte ledStates[MIDI_NOTES_COUNT] = {};

int currentNote = 0;
int currentSongIndex = -1;
bool selectingMode = true;

void selectMuxPort(int sensorIndex) {
  if (sensorIndex < 8) {
    myMux.setPort(sensorIndex);
  } else {
    myMux2.setPort(sensorIndex - 8);
  }
}

void setup() {
  Serial.begin(57600);
  Wire.begin();

  for (int i = 0; i < NUM_SENSORS; i++) {
    pinMode(ledPins[i], OUTPUT);
    digitalWrite(ledPins[i], HIGH);
  }

  if (myMux.begin() == false) {
    Serial.println("Mux 1 not detected. Freezing...");
    while (1);
  }
  
  if (myMux2.begin(0x71) == false) {
    Serial.println("Mux 2 not detected. Freezing...");
    while (1);
  }
  
  for (int x = 0; x < NUM_SENSORS; x++) {
    selectMuxPort(x);
    distanceSensors[x].setTimeout(2000);
    if (distanceSensors[x].init()) {
      distanceSensors[x].setDistanceMode(VL53L1X::Long);
      distanceSensors[x].setMeasurementTimingBudget(40000);
      distanceSensors[x].startContinuous(25); 
    } else {
      Serial.print("Sensor "); Serial.print(x); Serial.println(" failed!");
    }
  }

  Serial.println("System Online.");
  enterSongSelection();
}

void buildNoteMapping() {
  for (int i = 0; i < MIDI_NOTES_COUNT; i++) {
    mapNoteToLed[i] = 0;
  }
  const byte* songNotes = SONGS[currentSongIndex].notes;
  for (int i = 0; i < NUM_SENSORS; i++) {
    mapNoteToLed[songNotes[i]] = ledPins[i];
  }
}

void enterSongSelection() {
  selectingMode = true;
  currentSongIndex = -1;
  currentNote = 0;
  
  allLedsOff();
  delay(300);
  
  for (int i = 0; i < NUM_SONGS && i < NUM_SENSORS; i++) {
    ledOn(ledPins[i]);
  }
  
  Serial.println("Select a song:");
  for (int i = 0; i < NUM_SONGS; i++) {
    Serial.print("  LED "); Serial.print(i + 1); Serial.print(": ");
    Serial.println(SONGS[i].name);
  }
}

void selectSong(int songIndex) {
  if (songIndex < 0 || songIndex >= NUM_SONGS) return;
  
  currentSongIndex = songIndex;
  selectingMode = false;
  currentNote = 0;
  
  buildNoteMapping();
  
  Serial.print("Playing: ");
  Serial.println(SONGS[currentSongIndex].name);
  
  allLedsOff();
  delay(500);
  resetLeds();
  promptNext();
}

void resetSong() {
  allLedsOff();
  currentNote = 0;
  promptNext();
}

void ledOn(int pin) {
  if (pin <= 0) return;
  digitalWrite(pin, LOW);
  ledStates[pin] = LOW;
}

void ledOff(int pin) {
  if (pin <= 0) return;
  digitalWrite(pin, HIGH);
  ledStates[pin] = HIGH;
}

void allLedsOff() {
  for (int i = 0; i < NUM_SENSORS; i++) {
    digitalWrite(ledPins[i], HIGH);
  }
}

void resetLeds() {
  Serial.println("Resetting LEDs...");
  for (int i = 0; i < NUM_SENSORS; i++) {
    digitalWrite(ledPins[i], LOW);
    delay(50);
    digitalWrite(ledPins[i], HIGH);
    delay(50);
  }
}

byte getSongNote(int index) {
  return pgm_read_byte(&SONGS[currentSongIndex].data[index]);
}

int getSongLength() {
  return SONGS[currentSongIndex].length;
}

const byte* getCurrentNotes() {
  return SONGS[currentSongIndex].notes;
}

void checkNoteHit(byte sensorIndex) {
  const byte* songNotes = getCurrentNotes();
  byte playedNote = songNotes[sensorIndex];
  byte requiredNote = getSongNote(currentNote);

  if (playedNote == requiredNote) {
    noteOn(1, playedNote, 100);
    ledOff(mapNoteToLed[requiredNote]); 
    currentNote++;
    delay(30); 
    
    if (currentNote < getSongLength()) {
      promptNext();
    }
  }
}

void promptNext() {
  byte currentSongNote = getSongNote(currentNote);
  ledOn(mapNoteToLed[currentSongNote]);

  if (currentNote > 0) {
    byte prevNote = getSongNote(currentNote - 1);
    if (prevNote != currentSongNote) {
      ledOff(mapNoteToLed[prevNote]);
    } else {
      ledOff(mapNoteToLed[prevNote]);
      delay(450);
      ledOn(mapNoteToLed[currentSongNote]);
    }
  }
}

void loop() {
  if (selectingMode) {
    for (byte x = 0; x < NUM_SENSORS; x++) {
      selectMuxPort(x);
      int dist = distanceSensors[x].read();
      bool handPresent = (dist > 50 && dist < 200);

      if (handPresent && !sensorActive[x]) {
        sensorActive[x] = true;
        if (x < NUM_SONGS) {
          selectSong(x);
          return;
        }
      }
      else if (!handPresent) {
        sensorActive[x] = false;
      }
    }
  }
  else {
    if (currentNote >= getSongLength()) {
      Serial.println("Song complete!");
      delay(3000);
      enterSongSelection();
      return;
    }

    for (byte x = 0; x < NUM_SENSORS; x++) {
      selectMuxPort(x);
      int dist = distanceSensors[x].read();
      bool handPresent = (dist > 50 && dist < 200);

      if (handPresent && !sensorActive[x]) {
        sensorActive[x] = true;
        checkNoteHit(x);
      }
      else if (!handPresent) {
        sensorActive[x] = false;
      }
    }
  }
}
