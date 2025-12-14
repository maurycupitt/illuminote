#ifndef SONGS_H
#define SONGS_H

#include "songs/away_in_a_manger.h"
#include "songs/hark_the_herald.h"
#include "songs/joy_to_the_world.h"
#include "songs/o_come_all_ye_faithful.h"
#include "songs/the_first_noel.h"
#include "songs/up_on_the_housetop.h"
#include "songs/we_wish_you_a_merry_christmas.h"

struct Song {
  const char* name;
  const byte* data;
  const byte* notes;
  int length;
};

// Songs in alphabetical order (mapped to LEDs 0-7)
const Song SONGS[] = {
  {"Away in a Manger",      SONG_AWAY_IN_A_MANGER,      SONG_AWAY_IN_A_MANGER_NOTES,      SONG_AWAY_IN_A_MANGER_LENGTH},
  {"Hark the Herald",       SONG_HARK_THE_HERALD,       SONG_HARK_THE_HERALD_NOTES,       SONG_HARK_THE_HERALD_LENGTH},
  {"Joy to the World",      SONG_JOY_TO_THE_WORLD,      SONG_JOY_TO_THE_WORLD_NOTES,      SONG_JOY_TO_THE_WORLD_LENGTH},
  {"O Come All Ye Faithful", SONG_O_COME_ALL_YE_FAITHFUL, SONG_O_COME_ALL_YE_FAITHFUL_NOTES, SONG_O_COME_ALL_YE_FAITHFUL_LENGTH},
  {"The First Noel",        SONG_THE_FIRST_NOEL,        SONG_THE_FIRST_NOEL_NOTES,        SONG_THE_FIRST_NOEL_LENGTH},
  {"Up on the Housetop",    SONG_UP_ON_THE_HOUSETOP,    SONG_UP_ON_THE_HOUSETOP_NOTES,    SONG_UP_ON_THE_HOUSETOP_LENGTH},
  {"We Wish You a Merry Christmas", SONG_WE_WISH_YOU_A_MERRY_CHRISTMAS, SONG_WE_WISH_YOU_A_MERRY_CHRISTMAS_NOTES, SONG_WE_WISH_YOU_A_MERRY_CHRISTMAS_LENGTH}
};

const int NUM_SONGS = sizeof(SONGS) / sizeof(SONGS[0]);

#endif
