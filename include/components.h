#ifndef _COMPONENTS_H_
#define _COMPONENTS_H_

#define CCS_COMPONENTS \
	Component(Position, 1) \
	Component(Velocity, 2) \

typedef struct Position {
	float x; 
	float y;
} Position;

typedef struct Velocity {
	float x_speed; 
	float y_speed;
} Velocity;

#endif

