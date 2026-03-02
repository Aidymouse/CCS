
#include "raylib.h"

#define MAX_CCS_ENTITIES 300

/** Components **/
#define CCS_COMPONENTS \
	Component(Position, 1) \
	Component(Velocity, 2) \
	Component(Collider, 3) \
	Component(Visible, 4) \
	
	//Component(ManAI, 5) \

typedef struct Position {
	float x;
	float y;
	float z;
} Position;

// All colliders are squares these days
typedef CollisionGridIndex Collider;

typedef struct Velocity {
	float x;
	float y;
} Velocity;

typedef struct Visible {
	char offset_x;
	char offset_y;
	Color color;
} Visible;

typedef struct ManAI {
	int behaviour;
} ManAI;

/** SYSTEMS **/
#define CCS_SYSTEMS \
	System(Draw, CB_Position | CB_Visible) \
	System(Move, CB_Position | CB_Velocity) \
