#ifndef JOY_TO_THE_WORLD_H
#define JOY_TO_THE_WORLD_H

// MIDI note to sensor/LED mapping (uses C#5 instead of C5)
const byte notes[NUM_SENSORS] = {62, 64, 66, 67, 69, 71, 73, 74};

// LED pin assignments per sensor
const byte ledPins[NUM_SENSORS] = {10, 16, 4, 5, 6, 7, 8, 9};

// Joy to the World
const byte song[] = {
  74, 73, 71, 69, 67, 66, 64, 62,
  69, 71, 71, 73, 73, 74,
  74, 74, 73, 71, 69, 69, 67, 66,
  74, 74, 73, 71, 69, 69, 67, 66,
  66, 66, 66, 66, 66, 66, 67, 69, 67, 66, 64,
  64, 64, 64, 64, 66, 67, 66, 64, 62,
  74, 71, 69, 67, 66, 67, 66, 64, 62
};

const int songLength = sizeof(song);

#endif
