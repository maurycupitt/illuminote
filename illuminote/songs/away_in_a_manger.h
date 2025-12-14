#ifndef AWAY_IN_A_MANGER_H
#define AWAY_IN_A_MANGER_H

// MIDI note to sensor/LED mapping
const byte notes[NUM_SENSORS] = {62, 64, 66, 67, 69, 71, 72, 74};

// LED pin assignments per sensor
const byte ledPins[NUM_SENSORS] = {10, 16, 4, 5, 6, 7, 8, 9};

// Away in a Manger
const byte song[] = {
  74, 74, 72, 71, 71, 69, 67, 67, 66, 64, 62, 62,
  62, 64, 62, 62, 69, 66, 64, 62, 67, 71,
  74, 74, 72, 71, 71, 69, 67, 67, 66, 64, 62, 62,
  72, 71, 69, 71, 69, 67, 69, 64, 66, 67
};

const int songLength = sizeof(song);

#endif
