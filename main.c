
#include <stdio.h>

#define ECS_INCLUDE "components.h"

/** Add components below **/
#define COMPONENTS \
	Component(Position) \
	Component(Velocity) \

#define SYSTEMS \
	System(Move)

#define ECS_IMPLEMENTATION
#include "ecs.h"
	

int main() {

	ECS ecs;

	init_ecs(&ecs);	

	Entity e = add_entity(&ecs);

	Signature *s = get_signature_for_entity(&ecs, e);
	printf("Sig for %d: %d\n", e, *s);

	//remove_entity(&ecs, e);
	//s = get_signature_for_entity(&ecs, e);
	//printf("Sig for %d: %d\n", e, *s);

	Position *p = add_component(&ecs, 1, C_Position);
	Velocity *v = add_component(&ecs, 1, C_Velocity);
	//v->x_speed = 10;

	//printf("Position component %f, %f\n", p->x, p->y);
	//printf("Velocity component %f, %f\n", v->x_speed, v->y_speed);

	printf("%d%d%d%d%d\n",
		GetBit(*s, 4),
		GetBit(*s, 3),
		GetBit(*s, 2),
		GetBit(*s, 1),
		GetBit(*s, 0)
	);

	// Wow, pointers are scary.
	// This position can easily become outdated
	// Maybe there should be a set component function that takes a pointer to a struct to make sure we don't have ugly bugs
	// Alternatively, don't remove entities while using their components!


	return 0;
}
