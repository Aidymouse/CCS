
/** Components **/
#define CCS_COMPONENTS \
	Component(Position, 1) \
	Component(Collider, 2) \
	Component(Velocity, 3) \

typedef struct Position {
	float x;
	float y;
	float z;
} Position;

typedef struct Collider {
	float width;
	float height;
	float offset_x;
	float offset_y;
} Collider;

typedef struct Velocity {
	float x;
	float y;
	float z;
} Velocity;

#define CCS_SYSTEMS \
	System(Draw, CB_Position) \
	System(Move, CB_Position | CB_Velocity) \

#define CCS_IMPLEMENTATION

#include "ecs.h"
#include "raylib.h"

#define SCREENWIDTH 800
#define SCREENHEIGHT 450

int main() {

	ECS ecs;
	ccs_init_ecs(&ecs);


	Entity e = ccs_add_entity(&ecs);

	Position *p = ccs_add_component(&ecs, e, C_Position);
	p->x = SCREENWIDTH/2;
	p->y = SCREENHEIGHT/2;
	Velocity *v = ccs_add_component(&ecs, e, C_Velocity);
	v->z = 10;

	InitWindow(SCREENWIDTH, SCREENHEIGHT, "The Evil Men");
	SetTargetFPS(60);

	while (!WindowShouldClose())
	{
		/* UPDATE */
		System move_sys = ecs.systems[S_Move];
		for (int en=0; en<move_sys.num_registered; en++) {
			Entity ent = move_sys.registered_entities[en];
			Position *ps = ccs_get_component(&ecs, ent, C_Position);
			Velocity *ve = ccs_get_component(&ecs, ent, C_Velocity);

			ps->x += ve->x * GetFrameTime();
			ps->y += ve->y * GetFrameTime();
			ps->z += ve->z * GetFrameTime();
		}

		/* DRAW */
		BeginDrawing();
		ClearBackground(RAYWHITE);

		/** Basic draw system **/
		System draw_sys = ecs.systems[S_Draw];

		for (int en=0; en<draw_sys.num_registered; en++) {
			Entity ent = draw_sys.registered_entities[en];
			Position *ps = ccs_get_component(&ecs, ent, C_Position);

			if (ps->z > 0) {
				DrawEllipse(ps->x, ps->y, 4, 2, GRAY);
			}
			DrawRectangle(ps->x-4, ps->y - ps->z - 8, 8, 8, BLUE);
		}
		EndDrawing();

	}

	CloseWindow();

	return 0;

}
