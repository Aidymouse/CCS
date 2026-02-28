
#define SWAPBACK_ARRAY_IMPLEMENTATION
#include "SwapbackArray.h"

#define COLGRID_IMPLEMENTATION
#include "ColGrid.h"


#include "Collisions.h"

/** Components **/
#define CCS_COMPONENTS \
	Component(Position, 1) \
	Component(Velocity, 2) \
	Component(Collider, 3) \
	Component(ManAI, 4) \

typedef struct Position {
	float x;
	float y;
	float z;
} Position;

// All colliders are squares these days
typedef CollisionGridIndex Collider;

typedef struct Velocity {
	float x;
	float y;
	float z;
} Velocity;

typedef struct ManAI {
	int behaviour;
} ManAI;

#define CCS_SYSTEMS \
	System(Draw, CB_Position) \
	System(Move, CB_Position | CB_Velocity) \
	System(ManDecider, CB_ManAI | CB_Position) \

#define CCS_IMPLEMENTATION

#include "ccs.h"
#define VEC2_IMPLEMENTATION
#include "Vec2.h"
#include "raylib.h"
#define COLLISIONS_IMPLEMENTATION
#include "Collisions.h"


#define SCREENWIDTH 800
#define SCREENHEIGHT 450

void printf_colcell(CollisionGridCell cell) {
	printf("Cell %d: (id: %d, inhabitants: %d) ", cell.id, cell.id, cell.num_inhabitants);
	printf("[");
	for (int i=0; i<cell.num_inhabitants; i++) {
		printf("%d,", cell.inhabitants[i]);
	}
	printf("]\n");
}

void printf_colgrid(CollisionGrid *cg) {
	for (int i=0; i<cg->num_cells; i++) {
		CollisionGridCell cell = cg->cells[i];
		printf_colcell(cell);
	}
}


int main() {

	/** Collision Grid **/
	CollisionGrid grid;
	cg_init_colgrid(&grid);

	/** **/
	ECS ecs;
	ccs_init_ecs(&ecs);

	int x = 100;
	int y = 15;
	for (int i=0; i<200; i++) {

		Entity e = ccs_add_entity(&ecs);

		Position *p = ccs_add_component(&ecs, e, C_Position);
		p->x = x;
		p->y = y;
		x += 100;
		if (x > SCREENWIDTH) {
			x = 50;
			y += 15;
			if ((y / 15) % 2 == 1) {
				x = 100;
			}
		}

		Collider *collider = ccs_add_component(&ecs, e, C_Collider);
		collider->id = e;
		collider->pos_x = p->x;
		collider->pos_y = p->y;
		collider->shape.type = COL_RECT;
		collider->shape.data.rect.x = p->x - 4;
		collider->shape.data.rect.y = p->y - 4;
		collider->shape.data.rect.width = 8;
		collider->shape.data.rect.height = 8;
		collider->inhabited[0] = -1;
		cg_update_index(&grid, collider, p->x - 4, p->y - 4, 8, 8);
	}


	InitWindow(SCREENWIDTH, SCREENHEIGHT, "The Evil Men");
	SetTargetFPS(60);

	printf_colgrid(&grid);

	ccs_remove_component(&ecs, 1, C_Position);

	while (!WindowShouldClose())
	{
		/* UPDATE */
		if (IsMouseButtonPressed(0)) {
			Vector2 mouse = GetMousePosition();
			CollisionGridCell clicked_cell = cg_get_cell_id_at_pos(&grid, mouse.x, mouse.y);

			for (int in=0; in<clicked_cell.num_inhabitants; in++) {
				Entity ent = clicked_cell.inhabitants[in];
				Collider *c = ccs_get_component(&ecs, ent, C_Collider);
				CollisionRect r = c->shape.data.rect;
				if (point_in_square(mouse.x, mouse.y, r.x, r.y, r.width, r.height)) {

					printf("Clicked entity %d\n", ent);

					//cg_remove_index(&grid, c);
					//in--;
					//ccs_remove_entity(&ecs, ent);
				}
			}

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

		// Debug grid
		for (int row=0; row<15; row++) {
			for (int col=0; col<15; col++) {
				DrawRectangleLines(row*CELL_SIZE, col*CELL_SIZE, CELL_SIZE, CELL_SIZE, RED);
			}
		}

	}

	cg_free_colgrid(&grid);

	CloseWindow();

	return 0;

}
