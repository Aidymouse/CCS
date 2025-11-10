
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


	/** Add Entity **/
	Entity e = add_entity(&ecs);
	if (!e) {
		printf("Failed to add entity");
		return 1;
	}

	Entity e2 = add_entity(&ecs);

	/** Add Components **/
	Position *p = add_component(&ecs, e, C_Position);
	Position *p2 = add_component(&ecs, e2, C_Position);

	for (int i=0; i<10; i++) {
		p->x += 1;
	}

	/** Remove Components **/
	remove_component(&ecs, e, C_Position);

	for (int i=0; i<10; i++) {
	}

	/** Remove Entity **/
	remove_entity(&ecs, e);
	if (entity_exists(&ecs, e)) {
		printf("Failed to remove entity");
		return 1;
	}




	return 0;
}
