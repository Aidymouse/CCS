
#define SWAPBACK_ARRAY_IMPLEMENTATION
#include "SwapbackArray.h"
#define TIMER_IMPLEMENTATION
#include "Timer.h"
#define RANDOM_IMPLEMENTATION
#include "Random.h"
#define VEC2_IMPLEMENTATION
#include "Vec2.h"

#define COLGRID_IMPLEMENTATION
#include "ColGrid.h"

#define COLLISIONS_IMPLEMENTATION
#include "Collisions.h"

#include "CCS_Setup.h"
#define CCS_IMPLEMENTATION
#include "ccs.h"

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

CCS_Entity spawn_guy(CCS *ecs, CollisionGrid *grid) {
		Entity e = ccs_add_entity(ecs);

		Position *p = ccs_add_component(ecs, e, C_Position);
		Visible *v = ccs_add_component(ecs, e, C_Visible);
		Velocity *vel = ccs_add_component(ecs, e, C_Velocity);
		ManAI *mai = ccs_add_component(ecs, e, C_ManAI);
		Collider *collider = ccs_add_component(ecs, e, C_Collider);

		// TODO: random pos around edge of screen
		int side = rand_int(1, 4);
		if (side == 1) { // Top
			p->x = rand_int(0, SCREENWIDTH);
			p->y = 0;
		} else if (side == 2) { // Left
			p->x = 0;
			p->y = rand_int(0, SCREENHEIGHT);
		} else if (side == 3) { // Bottom
			p->x = rand_int(0, SCREENWIDTH);
			p->y = SCREENHEIGHT;
		} else {
			p->x = SCREENWIDTH;
			p->y = rand_int(0, SCREENHEIGHT);
		}

		v->offset_x = -4;
		v->offset_y = -4;
		v->color = BLUE;

		mai->behaviour = MANB_APPROACH;
		mai->state.approach.destination = (Vec2){ SCREENWIDTH/2, SCREENHEIGHT/2 };

		collider->id = e;
		collider->shape.x = p->x;
		collider->shape.y = p->y;
		collider->shape.type = COL_RECT;
		collider->shape.offset_x = -4;
		collider->shape.offset_y = -4;
		collider->shape.data.rect.width = 8;
		collider->shape.data.rect.height = 8;
		// lol
		collider->inhabited[0] = -1;
		collider->inhabited[1] = -1;
		collider->inhabited[2] = -1;
		collider->inhabited[3] = -1;
		cg_update_index(grid, collider);

		vel->x = 0;
		vel->y = 0;

		return e;
}

int main() {


	/** Collision Grid **/
	CollisionGrid grid;
	cg_init_colgrid(&grid);

	/** **/
	ECS ecs;
	ccs_init_ecs(&ecs);

	Timer man_spawn_timer = {
		.active = 1,
		.time_remaining = 0.5,
		.default_time = 0.5,
	};

	InitWindow(SCREENWIDTH, SCREENHEIGHT, "The Evil Men");
	SetTargetFPS(60);

	printf_colgrid(&grid);

	//ccs_remove_component(&ecs, 0, C_Position);

	while (!WindowShouldClose())
	{
		/** UPDATE **/
		float dt = GetFrameTime();

		timer_tick(&man_spawn_timer, dt);
		if (man_spawn_timer.time_remaining <= 0) {
			timer_reset(&man_spawn_timer);
			spawn_guy(&ecs, &grid);
		}

		/* Men */
		System manAI_sys = ecs.systems[S_ManDecision];
		for (int en=0; en<manAI_sys.num_registered; en++) {
			CCS_Entity man_ent = manAI_sys.registered_entities[en];

			ManAI *ai = ccs_get_component(&ecs, man_ent, C_ManAI);

			if (ai->behaviour == MANB_APPROACH) {
				Position *p = ccs_get_component(&ecs, man_ent, C_Position);
				Vec2 to_dest = Vec2Sub(ai->state.approach.destination, (Vec2){p->x, p->y});
				to_dest = Vec2Normalize(to_dest);

				Velocity *v = ccs_get_component(&ecs, man_ent, C_Velocity);
				v->x = to_dest.x;
				v->y = to_dest.y;

				Collider *col = ccs_get_component(&ecs, man_ent, C_Collider);
				// TODO
				if (point_in_circle_shapes((CollisionS)) {
					v->x = 0;
					v->y = 0;
				}
			}

		}

		/* Move System */
		System move_sys = ecs.systems[S_Move];
		for (int en=0; en<move_sys.num_registered; en++) {
			CCS_Entity move_ent = move_sys.registered_entities[en];
			Position *p = ccs_get_component(&ecs, move_ent, C_Position);
			Velocity *v = ccs_get_component(&ecs, move_ent, C_Velocity);
			Collider *c = ccs_get_component(&ecs, move_ent, C_Collider);

			p->x += v->x;
			p->y += v->y;

			if (c) {
				c->shape.x = p->x;
				c->shape.y = p->y;
				cg_update_index(&grid, c);
			}
		}
	
		/* Controls */
		if (IsMouseButtonPressed(0)) {
			Vector2 mouse = GetMousePosition();
			CollisionGridCell clicked_cell = cg_get_cell_at_pos(&grid, mouse.x, mouse.y);

			for (int in=0; in<clicked_cell.num_inhabitants; in++) {
				Entity ent = clicked_cell.inhabitants[in];
				Collider *c = ccs_get_component(&ecs, ent, C_Collider);
				CollisionRect r = c->shape.data.rect;
				CollisionShape mouse_point = {
					.type = COL_POINT,
					.offset_x = 0,
					.offset_y = 0,
							.x = mouse.x,
							.y = mouse.y,
					.data = {
					}
				};
				if (point_in_square_shapes(mouse_point, c->shape)) {

					printf("Clicked entity %d\n", ent);

					cg_remove_index(&grid, c);
					ccs_remove_entity(&ecs, ent);

					printf_colgrid(&grid);

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
		}
		EndDrawing();

		// Debug grid
		for (int row=0; row<CELLS_DOWN; row++) {
			for (int col=0; col<CELLS_ACROSS; col++) {
				DrawRectangleLines(col*CELL_SIZE, row*CELL_SIZE, CELL_SIZE, CELL_SIZE, RED);
			}
		}

	}

	cg_free_colgrid(&grid);

	CloseWindow();

	return 0;

}
