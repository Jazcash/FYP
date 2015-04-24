#ifndef LINEMAZE_H_
#define LINEMAZE_H_

#include <pololu/Pololu3pi.h>
#include <avr/eeprom.h>
#include <avr/pgmspace.h>
#include <stdbool.h>
#include <math.h>

#include "utility.h"
#include "enums.h"
#include "position.h"
#include "cardinalDirection.h"
#include "node.h"
#include "NodeStack.h"

unsigned int calibrated_maximum_on[5] EEMEM;
unsigned int calibrated_minimum_on[5] EEMEM;

bool isForward, isLeft, isRight;
bool foundFinish;

cDirection currentDir;
position currentPos;
NodeStack myNodeStack;

const char bloop[] PROGMEM = ">g32>>c32";

const char levels[] PROGMEM = { 0b00000,0b00000,0b00000,0b00000,0b00000,0b00000,0b00000,0b11111,0b11111,0b11111,0b11111,0b11111,0b11111,0b11111 };

void saveCalibration();
void loadCalibration();
void turn(Turn t);
void stop();
void mapMaze();
void followLine();
void load_custom_characters();
void display_readings(const unsigned int *calibrated_values);
int8_t followSegment();  // returns the number of tiles travelled
void faceDir(Direction d);
void travelTo(int8_t x, int8_t y);

#endif /* LINEMAZE_H_ */