
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

QWIICMUX myMux;
VL53L1X **distanceSensor;

#define NUMBER_OF_SENSORS 8
#define TOTAL_SENSORS 8
#define MIDI_NOTES_COUNT 127
#define LIVE_SENSORS 8


/*
   Away in the Manger
*/
bool sensorActive[TOTAL_SENSORS] = {false}; // Tracks if a hand is currently covering the sensor
const byte notes[TOTAL_SENSORS] = {62, 64, 66, 67, 69, 71, 72, 74};
const byte ledPins[TOTAL_SENSORS] =  {10, 16, 4, 5, 6, 7, 8, 9};

int mapNoteToLed[MIDI_NOTES_COUNT] = {};
byte ledStates[MIDI_NOTES_COUNT] = {};

const byte song[] = {74, 74, 72, 71, 71, 69, 67, 67, 66, 64, 62, 62, 62, 64, 62, 62, 69, 66, 64, 62, 67, 71, 74, 74, 72, 71, 71, 69, 67, 67, 66, 64, 62, 62, 72, 71, 69, 71, 69, 67, 69, 64, 66, 67};

const int songLength = sizeof(song);

byte sensorStates[TOTAL_SENSORS] = {};
byte lastStates[TOTAL_SENSORS] = {};
int distance[NUMBER_OF_SENSORS];

int currentNote = 0;

// --- REPLACEMENT SETUP ---
void setup()
{
  Serial.begin(57600);
  Wire.begin();

  // 1. Initialize LEDs
  for (int i = 0; i < LIVE_SENSORS; i++) {
    pinMode(ledPins[i], OUTPUT);
    // AUTOMATIC MAPPING: Map the MIDI note to the specific LED Pin
    mapNoteToLed[notes[i]] = ledPins[i]; 
  }

  // 2. Initialize Mux
  if (myMux.begin() == false) {
    Serial.println("Mux not detected. Freezing...");
    while (1);
  }
  
  // 3. Initialize Sensors
  distanceSensor = new VL53L1X *[NUMBER_OF_SENSORS];
  for (int x = 0; x < NUMBER_OF_SENSORS; x++) {
    distanceSensor[x] = new VL53L1X();
    myMux.setPort(x);
    distanceSensor[x]->setTimeout(2000);
    if (distanceSensor[x]->init()) {
      distanceSensor[x]->setDistanceMode(VL53L1X::Long);
      distanceSensor[x]->setMeasurementTimingBudget(40000);
      distanceSensor[x]->startContinuous(25); 
    } else {
      Serial.print("Sensor "); Serial.print(x); Serial.println(" failed!");
    }
  }

  Serial.println("System Online.");
  resetSong(); // Prepare the first song
}

// --- NEW RESET FUNCTION (Replaces calling setup() recursively) ---
void resetSong() {
  resetLeds();
  currentNote = 0;
  promptNext();
}

// --- SAFE LED FUNCTIONS ---
void ledOn(int pin) {
  if (pin <= 0) return; // Safety check
  digitalWrite(pin, LOW);
  ledStates[pin] = LOW;
}

void ledOff(int pin) {
  if (pin <= 0) return; // Safety check
  digitalWrite(pin, HIGH);
  ledStates[pin] = HIGH;
}

void resetLeds() {
  Serial.println("Resetting LEDs...");
  for (int i = 0; i < TOTAL_SENSORS; i++) {
    digitalWrite(ledPins[i], LOW);
    delay(50);
    digitalWrite(ledPins[i], HIGH);
    delay(50);
  }
}

// --- NEW LOGIC HANDLER ---
// Checks if the triggered sensor matches the required song note
void checkNoteHit(byte sensorIndex) {
  byte playedNote = notes[sensorIndex];
  byte requiredNote = song[currentNote];

  // Only proceed if the USER hit the CORRECT note
  if (playedNote == requiredNote) {
    
    // 1. Play the note
    noteOn(1, playedNote, 100); // Velocity 100 is standard loud
    MidiUSB.flush();
    
    // 2. Visual Feedback & Logic
    // Turn OFF the light for the note we just hit
    ledOff(mapNoteToLed[requiredNote]); 
    
    // Move to next note
    currentNote++;
    
    // 3. Brief delay to prevent double-triggering on fast movements
    delay(30); 
    
    // 4. Turn on the light for the NEW next note
    if (currentNote < songLength) {
      promptNext();
    }
  }
}

void promptNext() {
  // Turn on light for next Note
  //    Serial.println("||||||||||||||||||||||||||||");
  //    Serial.print("Next Note LED Pin On: ");
  //    Serial.print(mapNoteToLed[song[currentNote]]);
  //    Serial.print(" - Note ");
  //    Serial.print(currentNote);
  //    Serial.print(": ");
  //    Serial.println(song[currentNote]);
  //    Serial.println("||||||||||||||||||||||||||||");

  ledOn(mapNoteToLed[song[currentNote]]);

  if (currentNote > 0 && song[currentNote - 1] != song[currentNote]) {
    ledOff(mapNoteToLed[song[currentNote - 1]]);
  }

  // If the next note is the same as the previous put a slight delay in
  if (song[currentNote - 1] == song[currentNote]) {
    ledOff(mapNoteToLed[song[currentNote - 1]]);
    delay(450);
    ledOn(mapNoteToLed[song[currentNote]]);
  }
  //  Serial.println("|||||| Exiting promptNext()");
}

void handleSensorState(byte sensorPin) {
  //  Serial.print("In handleSensorState: ");
  //  Serial.print(sensorPin);
  //  Serial.print(" Distance: ");
  //  Serial.println(distance);
  sensorStates[sensorPin] = 0;

  // Serial.println("-----------");
  // Serial.print(" State: ");
  // Serial.print(sensorStates[sensorPin]);
  // Serial.print(" snsorPin: ");
  // Serial.print(sensorPin);
  // Serial.print(" - Note ");
  // Serial.print(currentNote);
  // Serial.print(" - notes[sensorPin] ");
  // Serial.print(notes[sensorPin]);
  // Serial.print(": ");
  // Serial.print(song[currentNote]);
  // Serial.println("-----------");


  // NOTE OFF
  //  if (currentNote != 0 && sensorStates[sensorPin] && !lastStates[sensorPin]) {
  ////    Serial.print(" Note Off - Sensor: ");
  ////    Serial.print(sensorPin);
  ////    Serial.print(" - Note ");
  ////    Serial.print(currentNote);
  ////    Serial.print(" - notes[sensorPin] ");
  ////    Serial.print(notes[sensorPin]);
  ////    Serial.print(": ");
  ////    Serial.println(song[currentNote]);
  //    noteOff(1, notes[sensorPin], 90);
  //    MidiUSB.flush();
  //    delay(10);
  //
  //  }

  // NOTE ON
  //      if (!sensorStates[sensorPin] && lastStates[sensorPin]) {
  if (song[currentNote] == notes[sensorPin] && !sensorStates[sensorPin] && lastStates[sensorPin]) {
    //    Serial.print(" Sensor: ");
    //    Serial.print(sensorPin);
    //    Serial.print(" State: ");
    //    Serial.print(sensorStates[sensorPin]);
    //    Serial.print(" Note: ");
    //    Serial.println(notes[sensorPin]);

    noteOn(1, notes[sensorPin], 47);
    MidiUSB.flush();
    delay(30);
    currentNote++;

    promptNext();

  }
  lastStates[sensorPin] = !sensorStates[sensorPin];

}

// --- REPLACEMENT LOOP ---
void loop() {
  // Check if song is over
  if (currentNote >= songLength) {
    delay(5000); // Wait 5 seconds
    resetSong(); // Restart
    return;
  }

  // Scan all sensors
  for (byte x = 0; x < NUMBER_OF_SENSORS; x++) {
    myMux.setPort(x);
    
    // Get Distance
    int dist = distanceSensor[x]->read();
    
    // Determine if hand is present (Threshold: 50mm to 200mm)
    bool handPresent = (dist > 50 && dist < 200);

    // STATE MACHINE LOGIC
    // We only trigger if the hand wasn't there before ( !sensorActive ), but IS there now ( handPresent )
    if (handPresent && !sensorActive[x]) {
       sensorActive[x] = true; // Mark as covered
       checkNoteHit(x);        // Fire logic
    }
    // Reset state if hand leaves
    else if (!handPresent) {
       sensorActive[x] = false;
    }
  }
}
