
#include <stdio.h>
#include "raylib.h"
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

	

int main() {

	ECS ecs;

	init_ecs(&ecs);


	InitWindow(800, 800, "ECS");
	SetTargetFPS(60);

	Entity e = add_entity(&ecs);
	add_component(&ecs, e, C_Position);
	Velocity *v = add_component(&ecs, e, C_Velocity);
	v->x_speed = 0.5;
	v->y_speed = 0.5;


	while (!WindowShouldClose()) {

		/** Basic system **/
		System move = ecs.systems[S_Move];
		for (int en=0; en<move.num_registered; en++) {
			Entity ent = move.registered_entities[en];
			Position *ps = get_component(&ecs, ent, C_Position);
			Velocity *vs = get_component(&ecs, ent, C_Velocity);

			ps->x += vs->x_speed;
			ps->y += vs->y_speed;
		}

		BeginDrawing();
		
		Position *p = get_component(&ecs, e, C_Position);

		DrawRectangle(p->x, p->y, 50, 50, RED);

		EndDrawing();
	}

	return 0;
}
