#ifndef __wire_h__
#define __wire_h__

/*
arduino breakout board pen mapping
IO0  130
IO1  131
IO2  128
IO3  12
IO4  129
IO5  13
IO6  182
IO7  48
IO8  49
IO9  183
IO10 41
IO11 43
IO12 42
IO13 40
IO14 44
IO15 45
IO16 46
IO17 47
IO18 14
IO19 165
*/


#define WIRE_DIRECTION_IN  0
#define WIRE_DIRECTION_OUT 1

typedef struct _Wire_
{
	int pin;
	int direction;
} Wire;

void Wire_Wire(Wire* w);
int Wire_connect(Wire* w, int edisonPin, int direction);
void Wire_write(Wire* w, int level);

#endif
