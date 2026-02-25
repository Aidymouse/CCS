
/** Components **/
#define CCS_COMPONENTS \
	Component(Position, 1) \
	Component(Collider, 2) \

typedef struct Position {
	float x;
	float y;
} Position;

typedef struct Collider {
	float width;
	float height;
	float offset_x;
	float offset_y;
} Collider;

#define CCS_SYSTEMS \
	System(Draw, CB_Position) \

#define ECS_IMPLEMENTATION

#include "ecs.h"
#include "raylib.h"

int main() {

	ECS ecs;
	ccs_init_ecs(&ecs);


	Entity e = ccs_add_entity(&ecs);

	Position *p = ccs_add_component(&ecs, e, C_Position);
	p->x = 10;

	InitWindow(800, 450, "The Evil Men");


    while (!WindowShouldClose())
    {
        BeginDrawing();
            ClearBackground(RAYWHITE);

		/** Basic draw system **/
	    System draw_sys = ecs.systems[S_Draw];
		
	    for (int en=0; en<draw_sys.num_registered; en++) {
		Entity ent = draw_sys.registered_entities[en];
		Position *ps = ccs_get_component(&ecs, ent, C_Position);

		DrawRectangle(ps->x, ps->y, 50, 50, BLUE);
	    }
        EndDrawing();
    }

    CloseWindow();

    return 0;

}
