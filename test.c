#include <stdio.h>
//#include <assert.h>

#define ECS_INCLUDE "components.h"

/** Add components below **/
#define COMPONENTS \
	Component(Position) \
	Component(Velocity) \

#define SYSTEMS \
	System(Move, 0b00000111)

#define ECS_IMPLEMENTATION
#include "ecs.h"

#define ecstest(condition, message) if (condition == false) { \
	printf("ECS TEST FAIL: "#message"\n");\
	return 1;\
} \
	

int main() {

	ECS ecs;

	init_ecs(&ecs);

	/** Add Entity **/
	Entity e = add_entity(&ecs);
	ecstest(e != 0, "Failed to add entity");
	ecstest(ecs.signatures[e-1] != 0, "Entity signature did not update");

	Entity e2 = add_entity(&ecs);

	/** Add Components **/
	Position *p = add_component(&ecs, e, C_Position);
	Velocity *v = add_component(&ecs, e, C_Velocity);
	Position *p2 = add_component(&ecs, e2, C_Position);

	ecstest(p!=0, "Position component returned bad pointer");
	ecstest(p->x==0, "Position does not init x properly");
	ecstest(p->y==0, "Position does not init y properly");

	printf("HALT\n");


	printf("HALT\n");

	/** Remove Components **/
	//remove_component(&ecs, e, C_Position);


	/** Basic system **/
	System move = ecs.systems[S_Move];
	for (int en=0; en<move.num_registered; en++) {
		Entity ent = move.registered_entities[en];
		Position *ps = get_component(&ecs, ent, C_Position);
		Velocity *vs = get_component(&ecs, ent, C_Velocity);

		ps->x += vs->x_speed;
		ps->y += vs->y_speed;
	}

	/** Remove Entity **/
	remove_entity(&ecs, e);
	if (entity_exists(&ecs, e)) {
		printf("Failed to remove entity");
		return 1;
	}




	return 0;
}
