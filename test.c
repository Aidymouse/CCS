#include <stdio.h>

#include "components.h"

//#define CCS_OPT_NO_HELPER_TYPES
#define CCS_IMPLEMENTATION

#include "systems.h"
#include "components.h"

#include "ccs.h"

#define ccstest(condition, message) if (condition == false) { \
	printf("ECS TEST FAIL: "#message"\n");\
	return 1;\
} \
	

void component_tests() {
}

int main() {

	ECS ecs;
	ccs_init_ecs(&ecs);

	ccstest(ccs_entity_exists(&ecs, 1) == false, "Entity that does not exist exists");
	printf("Pass: Non-existant Entity Not Exists\n");

	/** Add Entity **/
	Entity e = ccs_add_entity(&ecs);
	ccstest(e != 0, "Failed to add entity");
	ccstest(ecs.signatures[e-1] != 0, "Entity signature did not update");
	printf("Pass: Add Entity\n");

	Entity e2 = ccs_add_entity(&ecs);

	// Add too many entities
	Entity e3 = ccs_add_entity(&ecs);
	ccstest(e3 == 0, "Entity added beyond range (or max entities > 2)")


	/** Add Components **/
	Position *p = ccs_add_component(&ecs, e, C_Position);
	Velocity *v = ccs_add_component(&ecs, e, C_Velocity);
	Position *p2 = ccs_add_component(&ecs, e2, C_Position);

	ccstest(p!=0, "Position component returned bad pointer");
	ccstest(p->x==0, "Position does not init x properly");
	ccstest(p->y==0, "Position does not init y properly");

	ccstest(ecs.components_Position.ent_to_comp_idx[e-1] == 0, "Entity mapped to wrong component idx");

	printf("Pass: Add Components\n");

	/** Update Components **/

	/** Remove Components **/
	ccs_remove_component(&ecs, e, C_Position);
	ccstest(ccs_get_component(&ecs, e, C_Position) == 0, "Component not removed");
	ccstest(ecs.components_Position.ent_to_comp_idx[e-1] == -1, "Entity to Component Index failed to be updated on Remove");


	/** Basic system **/
	System move = ecs.systems[S_Move];
	for (int en=0; en<move.num_registered; en++) {
		Entity ent = move.registered_entities[en];
		Position *ps = ccs_get_component(&ecs, ent, C_Position);
		Velocity *vs = ccs_get_component(&ecs, ent, C_Velocity);

		ps->x += vs->x_speed;
		ps->y += vs->y_speed;
	}

	// TODO: test cases ?
	//printf("Pass:  Entity");


	/** Remove Entity **/
	ccs_remove_entity(&ecs, e);
	ccstest(ccs_entity_exists(&ecs, e) == false, "Entity not removed properly");

	printf("Pass: Remove Entity\n");

	printf("Yippee! All passed.\n");


	return 0;
}
