/** Collision Grid **/

#ifndef _COLGRID_H_
#define _COLGRID_H_

#include <stdlib.h>
#include "Collisions.h"

#include "SwapbackArray.h"

#define MAX_INHABITANTS 50
#define GRID_WIDTH 800
#define GRID_HEIGHT 450
#define CELL_SIZE 100
#define CELLS_ACROSS GRID_WIDTH / CELL_SIZE
#define CELLS_DOWN GRID_HEIGHT / CELL_SIZE
#define NUM_CELLS CELLS_ACROSS * CELLS_DOWN

typedef struct CollisionGridCell {
	int id;
	int num_inhabitants; 
	int *inhabitants;
} CollisionGridCell;

typedef struct CollisionGrid {
	CollisionGridCell cells[NUM_CELLS];
	int num_cells;
} CollisionGrid;

typedef struct CollisionGridIndex {
	int id;
	float pos_x;
	float pos_y;
	CollisionShape shape;
	/** The inhabited cells. If every object is smaller than the grid size, the max this can be is 4 **/
	// TODO: this is limited to 1 right now
	int inhabited[4];
} CollisionGridIndex;

// Helper

/** Declarations **/
void cg_init_colgrid(CollisionGrid *cg);
void cg_free_colgrid(CollisionGrid *cg);
void cg_init_cell(CollisionGridCell *cell, int id, int init_inhabitants);
void cg_update_index(CollisionGrid *cg, CollisionGridIndex *cg_index, float pos_x, float pos_y, float width, float height);
void cg_remove_index(CollisionGrid *cg, CollisionGridIndex *cg_index);
CollisionGridCell cg_get_cell_id_at_pos(CollisionGrid *cg, float pos_x, float pos_y);

#ifdef COLGRID_IMPLEMENTATION

void cg_init_colgrid(CollisionGrid *cg) {

	for (int cell_id=0; cell_id < NUM_CELLS; cell_id++) {
		cg_init_cell(&cg->cells[cell_id], cell_id, 50);
	}

	cg->num_cells = NUM_CELLS;

}

void cg_free_colgrid(CollisionGrid *cg) {
	for (int cell_id=0; cell_id < NUM_CELLS; cell_id++) {
		free(cg->cells[cell_id].inhabitants);
	}
}

/** Initializes a grid cell **/
// TODO: return bool for success or not
void cg_init_cell(CollisionGridCell *cell, int id, int init_inhabitants) {
	int *inhabitants = malloc(sizeof(int) * init_inhabitants);

	if (inhabitants == 0) {
		return;
	}

	cell->id = id;
	cell->num_inhabitants = 0;
	cell->inhabitants = inhabitants;

}

/** Takes an index and updates it and the colgrid based on a new position **/
void cg_update_index(CollisionGrid *cg, CollisionGridIndex *cg_index, float pos_x, float pos_y, float width, float height) {


	// Remove from current inabitation, if any
	if (cg_index->inhabited[0] != -1) {
		CollisionGridCell inhabited_cell = cg->cells[cg_index->inhabited[0]];
		sba_remove(inhabited_cell.inhabitants, cg_index->id, inhabited_cell.num_inhabitants);
		// TODO:
	}

	// Update index's inhabited cells
	int cell_row = pos_y / CELL_SIZE;
	int cell_col = pos_x / CELL_SIZE;
	int cell_id = (cell_row * CELLS_ACROSS) + cell_col;

	if (cell_id < 0 || cell_id >= NUM_CELLS) {
		cg_index->inhabited[0] = -1; // Inhabit no cell
		return;
	}

	cg_index->inhabited[0] = cell_id;

	// Update cells inhabitants
	CollisionGridCell *cell = &cg->cells[cell_id];
	cell->num_inhabitants += sba_insert(cell->inhabitants, cg_index->id, cell->num_inhabitants);

}

void cg_remove_index(CollisionGrid *cg, CollisionGridIndex *index) { 
	if (index->inhabited[0] == -1) { return; }

	int cell_id = index->inhabited[0];

	CollisionGridCell *cell = &cg->cells[cell_id];
	cell->num_inhabitants -= sba_remove(cell->inhabitants, index->id, cell->num_inhabitants);

	index->inhabited[0] = -1;


}

CollisionGridCell cg_get_cell_id_at_pos(CollisionGrid *cg, float pos_x, float pos_y) {
	int cell_row = pos_y / CELL_SIZE;
	int cell_col = pos_x / CELL_SIZE;
	int cell_id = (cell_row * CELLS_ACROSS) + cell_col;

	if (cell_id < 0 || cell_id >= NUM_CELLS) {
		CollisionGridCell fail = {
			.id = -1,
			.num_inhabitants = 0,
			.inhabitants = NULL,
		};
		return fail;
	}

	return cg->cells[cell_id];

}

//int cg_get_colliding_ids(int *id_arr, ) {
//}


#endif // _COLGRID_IMPLEMENTATION_

#endif
