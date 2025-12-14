#ifndef O_COME_ALL_YE_FAITHFUL_H
#define O_COME_ALL_YE_FAITHFUL_H

// MIDI note to sensor/LED mapping
const byte notes[NUM_SENSORS] = {62, 64, 66, 67, 69, 71, 72, 74};

// LED pin assignments per sensor
const byte ledPins[NUM_SENSORS] = {10, 16, 4, 5, 6, 7, 8, 9};

// O Come All Ye Faithful
const byte song[] = {
  67, 67, 62, 67, 69, 62, 71, 69, 71, 72, 71, 67,
  66, 64, 66, 67, 69, 71, 66, 64, 62,
  62, 74, 72, 71, 72, 71, 69, 71, 67, 69, 66, 64, 62,
  67, 67, 66, 67, 69, 67, 62, 71, 71, 69, 71, 72, 71, 69,
  71, 72, 71, 69, 67, 66, 67, 72, 71, 69, 67, 67
};

const int songLength = sizeof(song);

#endif
