#ifndef THE_FIRST_NOEL_H
#define THE_FIRST_NOEL_H

// MIDI note to sensor/LED mapping (uses C#5 instead of C5)
const byte notes[NUM_SENSORS] = {62, 64, 66, 67, 69, 71, 73, 74};

// LED pin assignments per sensor
const byte ledPins[NUM_SENSORS] = {10, 16, 4, 5, 6, 7, 8, 9};

// The First Noel
const byte song[] = {
  66, 64, 62, 64, 66, 67, 69, 71, 73, 74, 73, 71, 69, 71, 73, 74, 73, 71,
  69, 71, 73, 74, 69, 67, 66, 66, 64, 62, 64, 66, 67, 69, 71, 73, 74, 73, 71,
  69, 71, 73, 74, 73, 71, 69, 71, 73, 74, 69, 67, 66, 66, 64, 62, 64, 66, 67,
  69, 74, 73, 71, 71, 69, 74, 73, 71, 69, 71, 73, 74, 69, 67, 66
};

const int songLength = sizeof(song);

#endif
