#include "Vec2.h"
#include "Timer.h"

typedef enum ManBehaviour {
	// Men start on the edge of the screen and move toward the middle
	MANB_APPROACH,
	// Men close enough to the center orb will attack.
	MANB_ATTACK,
} ManBehaviour;

typedef struct ManStateApproach {
	Vec2 destination;
} ManStateApproach;

typedef struct ManStateAttack {
	Timer attack_timer;
} ManStateAttack;

typedef struct ManAI {
	ManBehaviour behaviour;
	union {
		ManStateApproach approach;
		ManStateAttack attack;
	} state;
} ManAI;
