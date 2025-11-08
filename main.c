
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



	//remove_entity(&ecs, e);
	//s = get_signature_for_entity(&ecs, e);
	//printf("Sig for %d: %d\n", e, *s);

	/** Add Entity **/
	Entity e = add_entity(&ecs);
	if (!e) {
		printf("Failed to add entity");
		return 1;
	}

	/** Add Components **/
	Position *p = add_component(&ecs, e, C_Position);


	/** Remove Entity **/
	remove_entity(&ecs, e);
	if (entity_exists(&ecs, e)) {
		printf("Failed to remove entity");
		return 1;
	}




	return 0;
}
