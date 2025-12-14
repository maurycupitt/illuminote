
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
   Joy to the World
*/

const byte notes[TOTAL_SENSORS] = {62, 64, 66, 67, 69, 71, 73, 74};
const byte ledPins[TOTAL_SENSORS] =  {10, 16, 4, 5, 6, 7, 8, 9};

int mapNoteToLed[MIDI_NOTES_COUNT] = {};
byte ledStates[MIDI_NOTES_COUNT] = {};

const byte song[] = {66,64,62,64,66,67,69,71,73,74,73,71,69,71,73,74,73,71,69,71,73,74,69,67,66,66,64,62,64,66,67,69,71,73,74,73,71,69,71,73,74,73,71,69,71,73,74,69,67,66,66,64,62,64,66,67,69,74,73,71,71,69,74,73,71,69,71,73,74,69,67,66};
const int songLength = sizeof(song);

byte sensorStates[TOTAL_SENSORS] = {};
byte lastStates[TOTAL_SENSORS] = {};
int distance[NUMBER_OF_SENSORS];

int currentNote = 0;

void setup()
{
  for (int i = 0; i < LIVE_SENSORS; i++) {
    pinMode(ledPins[i], OUTPUT);
  }

  mapNoteToLed[62] = 10;
  mapNoteToLed[64] = 16;
  mapNoteToLed[66] = 4;
  mapNoteToLed[67] = 5;
  mapNoteToLed[69] = 6;
  mapNoteToLed[71] = 7;
  mapNoteToLed[73] = 8;
  mapNoteToLed[74] = 9;

  Serial.begin(57600);

  delay(100);

  Wire.begin();

  if (myMux.begin() == false) {
    Serial.println("Mux not detected. Freezing...");
    while (1);
  }
  Serial.println("Mux detected");

  //Create set of pointers to the class
  distanceSensor = new VL53L1X *[NUMBER_OF_SENSORS];

  //Assign pointers to instances of the class
  for (int x = 0; x < NUMBER_OF_SENSORS; x++) {
    distanceSensor[x] = new VL53L1X();
  }

  byte currentPortNumber = myMux.getPort();
  Serial.print("CurrentPort: ");
  Serial.println(currentPortNumber);

  //Initialize all the sensors
  bool initSuccess = true;

  for (byte x = 0; x < NUMBER_OF_SENSORS; x++)
  {
    myMux.setPort(x);
    distanceSensor[x]->setTimeout(2000);
    if (!distanceSensor[x]->init()) //Begin returns 0 on a good init
    {
      Serial.print("Sensor ");
      Serial.print(x);
      Serial.println(" did not begin! Check wiring");
      initSuccess = false;
    }
    else
    {
      //Configure each sensor
      distanceSensor[x]->setDistanceMode(VL53L1X::Long);
      distanceSensor[x]->setMeasurementTimingBudget(40000);
      distanceSensor[x]->startContinuous(25); //Write configuration bytes to initiate measurement
      Serial.print("Sensor ");
      Serial.print(x);
      Serial.println(" configured");
    }
  }

  if (initSuccess == false)
  {
    Serial.print("Freezing...");
    while (1);
  }
  Serial.println("Mux Shield online");
  delay(10);

  //  Serial.print("Starting Current Note: ");
  //  Serial.print(song[currentNote]);
  //  Serial.print(" - ");
  //  Serial.println(notePitch[currentNote]);
  resetLeds();
  promptNext();

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

void ledOn(int pin) {
  digitalWrite(pin, LOW);
  ledStates[pin] = LOW;
}

void ledOff(int pin) {
  digitalWrite(pin, HIGH);
  ledStates[pin] = HIGH;
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

void loop() {
  if (currentNote >= songLength) {
    delay(5000);
    currentNote = 0;
    setup();

  }

  for (byte x = 0; x < NUMBER_OF_SENSORS; x++) {
    myMux.setPort(x);
    distance[x] = distanceSensor[x]->read();

    if (distance[x] > 0 && distance[x] < 200) {
      handleSensorState(x);
    }
  }
  delay(50);
}
