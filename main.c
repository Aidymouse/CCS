
#include <stdio.h>

#define ECS_INCLUDE "structs.h"

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

	Signature s = get_signature_for_entity(&ecs, e);

	printf("Sig for %d: %d\n", e, s);


	remove_entity(&ecs, e);
	s = get_signature_for_entity(&ecs, e);

	printf("Sig for %d: %d\n", e, s);

	return 0;
}
