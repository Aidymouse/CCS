
#define SWAPBACK_ARRAY_IMPLEMENTATION
#include "SwapbackArray.h"

#define COLGRID_IMPLEMENTATION
#include "ColGrid.h"

#define COLLISIONS_IMPLEMENTATION
#include "Collisions.h"

#include "CCS_Setup.h"
#define CCS_IMPLEMENTATION
#include "ccs.h"

#define VEC2_IMPLEMENTATION
#include "Vec2.h"
#include "raylib.h"

#define SCREENWIDTH 700
#define SCREENHEIGHT 700

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
	int y = 20;
	for (int i=0; i<200; i++) {

		Entity e = ccs_add_entity(&ecs);

		Position *p = ccs_add_component(&ecs, e, C_Position);
		p->x = x;
		p->y = y;
		x += 100;
		if (x > SCREENWIDTH-100) {
			x = 50;
			y += 15;
			if ((y / 15) % 2 == 1) {
				x = 100;
			}
		}

		Visible *v = ccs_add_component(&ecs, e, C_Visible);
		v->offset_x = -4;
		v->offset_y = -4;
		v->color = BLUE;

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

	//ccs_remove_component(&ecs, 0, C_Position);

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

					cg_remove_index(&grid, c);
					ccs_remove_entity(&ecs, ent);
					break;
					//ccs_remove_component(&ecs, ent, C_Position);
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
			Visible *vs = ccs_get_component(&ecs, ent, C_Visible);

			DrawRectangle(ps->x + vs->offset_x, ps->y + vs->offset_y, 8, 8, vs->color);
			vs->offset_x += 1;
		}
		EndDrawing();

		// Debug grid
		/*
		for (int row=0; row<15; row++) {
			for (int col=0; col<15; col++) {
				DrawRectangleLines(row*CELL_SIZE, col*CELL_SIZE, CELL_SIZE, CELL_SIZE, RED);
			}
		}
		*/

	}

	cg_free_colgrid(&grid);

	CloseWindow();

	return 0;

}
