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

#define NUM_SENSORS 8
#define MIDI_NOTES_COUNT 127

// Select song by including the appropriate header file
// #include "songs/away_in_a_manger.h"
// #include "songs/joy_to_the_world.h"
#include "songs/o_come_all_ye_faithful.h"


QWIICMUX myMux;
VL53L1X distanceSensors[NUM_SENSORS];

bool sensorActive[NUM_SENSORS] = {false};
int mapNoteToLed[MIDI_NOTES_COUNT] = {};
byte ledStates[MIDI_NOTES_COUNT] = {};

int currentNote = 0;

void setup() {
  Serial.begin(57600);
  Wire.begin();

  // 1. Initialize LEDs
  for (int i = 0; i < NUM_SENSORS; i++) {
    pinMode(ledPins[i], OUTPUT);
    mapNoteToLed[notes[i]] = ledPins[i]; 
  }

  // 2. Initialize Mux
  if (myMux.begin() == false) {
    Serial.println("Mux not detected. Freezing...");
    while (1);
  }
  
  // 3. Initialize Sensors
  for (int x = 0; x < NUM_SENSORS; x++) {
    myMux.setPort(x);
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
  resetSong();
}

void resetSong() {
  resetLeds();
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

void resetLeds() {
  Serial.println("Resetting LEDs...");
  for (int i = 0; i < NUM_SENSORS; i++) {
    digitalWrite(ledPins[i], LOW);
    delay(50);
    digitalWrite(ledPins[i], HIGH);
    delay(50);
  }
}

void checkNoteHit(byte sensorIndex) {
  byte playedNote = notes[sensorIndex];
  byte requiredNote = song[currentNote];

  if (playedNote == requiredNote) {
    noteOn(1, playedNote, 100);
    ledOff(mapNoteToLed[requiredNote]); 
    currentNote++;
    delay(30); 
    
    if (currentNote < songLength) {
      promptNext();
    }
  }
}

void promptNext() {
  ledOn(mapNoteToLed[song[currentNote]]);

  if (currentNote > 0) {
    if (song[currentNote - 1] != song[currentNote]) {
      ledOff(mapNoteToLed[song[currentNote - 1]]);
    } else {
      ledOff(mapNoteToLed[song[currentNote - 1]]);
      delay(450);
      ledOn(mapNoteToLed[song[currentNote]]);
    }
  }
}

void loop() {
  if (currentNote >= songLength) {
    delay(5000);
    resetSong();
    return;
  }

  for (byte x = 0; x < NUM_SENSORS; x++) {
    myMux.setPort(x);
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
