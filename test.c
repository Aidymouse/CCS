#include <stdio.h>

#include "components.h"

//#define CCS_OPT_NO_HELPER_TYPES
#define CCS_IMPLEMENTATION

#define MAX_CCS_ENTITIES 2 // Custom define for tests

/** Test Components **/
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

/** Test Systems **/
#define CCS_SYSTEMS \
	System(Move, CB_Position | CB_Velocity) \

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

	printf("Test Start | Max Entities: %d, Max Components: %d, Max Systems: %d\n", MAX_CCS_ENTITIES, MAX_CCS_COMPONENTS, MAX_CCS_SYSTEMS);

	ccstest(ccs_entity_exists(&ecs, 1) == false, "Entity that does not exist exists");
	printf("Test | Non-existent Entity Not Exists - PASS\n");

	/** Add Entity **/
	Entity e = ccs_add_entity(&ecs);
	ccstest(e != -1, "Failed to add entity");
	ccstest(ccs_get_signature_for_entity(&ecs, e) != 0, "Entity signature not updated");

	Entity e2 = ccs_add_entity(&ecs);

	// Add too many entities
	Entity e3 = ccs_add_entity(&ecs);
	ccstest(e3 == -1, "Entity added beyond range (or max entities > 2)")

	printf("Test | Add Entities - PASS\n");

	/** Add Components **/
	Position *p = ccs_add_component(&ecs, e, C_Position);
	Velocity *v = ccs_add_component(&ecs, e, C_Velocity);
	Position *p2 = ccs_add_component(&ecs, e2, C_Position);

	ccstest(p!=0, "Position component returned bad pointer");
	ccstest(p->x==0, "Position does not init x properly");
	ccstest(p->y==0, "Position does not init y properly");

	ccstest(ecs.components_Position.ent_to_comp_idx[e] == 0, "Entity mapped to wrong component idx");
	ccstest(ccs_get_component(&ecs, e, C_Position) != 0, "Entity failed to get component");

	printf("Test | Add Components - PASS\n");

	/** Update Components **/

	/** Remove Components **/
	ccs_remove_component(&ecs, 0, C_Position);
	ccstest(ccs_get_component(&ecs, e, C_Position) == 0, "Component not removed");
	ccstest(ecs.components_Position.ent_to_comp_idx[e] == -1, "Entity to Component Index failed to be updated on Remove");
	ccstest(ecs.components_Position.comp_idx_to_ent[0] != 0, "Component Idx to Entity should not be 0");

	printf("Test | Remove Components - PASS\n");

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

	printf("Test | Remove Entity - PASS\n");

	printf("\n\nYippee! All passed.\n");

	return 0;
}
