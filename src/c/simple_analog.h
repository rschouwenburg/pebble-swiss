#pragma once

#include "pebble.h"

//#define SCREENSHOT 1

static const struct GPathInfo BT_ICON_PATH = {
  6,
    (GPoint []) {
      {1, 5},
      {8, 12},
      {4, 16},
      {4, 1},
      {8, 5},
      {1, 12}
    }
};

static const GRect TICK_SPRITE_24_DEF[] = {
  { {0, 0}, {22, 19} },    // 12
  { {0, 0}, {7, 19} },     // 1
  { {10, 0}, {12, 19} },   // 2
  { {25, 0}, {12, 19} },   // 3
  { {40, 0}, {13, 19} },   // 4
  { {56, 0}, {12, 19} },   // 5
  { {71, 0}, {12, 19} },   // 6
  { {86, 0}, {13, 19} },   // 7
  { {101, 0}, {12, 19} },   // 8
  { {117, 0}, {12, 19} },  // 9
  { {133, 0}, {22, 19} },  // 10
  { {160, 0}, {18, 19} }   // 11
};