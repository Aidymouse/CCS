/** Collision Grid **/

#ifndef _COLGRID_H_
#define _COLGRID_H_

#include <stdlib.h>
#include "Collisions.h"

#include "SwapbackArray.h"

#define MAX_INHABITANTS 50
#define MAX_INHABITED_CELLS 4
#define GRID_WIDTH 800
#define GRID_HEIGHT 800
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
	int inhabited[MAX_INHABITED_CELLS];
} CollisionGridIndex;

// Helper

/** Declarations **/
void cg_init_colgrid(CollisionGrid *cg);
void cg_free_colgrid(CollisionGrid *cg);
void cg_init_cell(CollisionGridCell *cell, int id, int init_inhabitants);
void cg_update_index(CollisionGrid *cg, CollisionGridIndex *cg_index, float pos_x, float pos_y, float width, float height);
void cg_remove_index(CollisionGrid *cg, CollisionGridIndex *cg_index);
CollisionGridCell cg_get_cell_at_pos(CollisionGrid *cg, float pos_x, float pos_y);
int cg_does_cell_exist(int row, int col);

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

int cg_get_cell_id_for_pos(int x, int y) {
	int cell_row = y / CELL_SIZE;
	int cell_col = x / CELL_SIZE;

	if (!cg_does_cell_exist(cell_row, cell_col)) {
		return -1;
	}

	int cell_id = (cell_row * CELLS_ACROSS) + cell_col;


	return cell_id;

}


/** Takes an index and updates it and the colgrid based on a new position **/
void cg_update_index(CollisionGrid *cg, CollisionGridIndex *cg_index, float pos_x, float pos_y, float width, float height) {

	// Reset all cells
	for (int c = 0; c<MAX_INHABITED_CELLS; c++) {
		int cell_id = cg_index->inhabited[c];
		if (cell_id == -1) { break; }
		CollisionGridCell *inhabited_cell = &cg->cells[cell_id];
		inhabited_cell->num_inhabitants -= sba_remove(inhabited_cell->inhabitants, cg_index->id, inhabited_cell->num_inhabitants);

		cg_index->inhabited[c] = -1; // Inhabit no cell
	}
	int cell_idx = 0;

	// General approach here is we test the four corners, assuming our guy is a square

	// Update index's inhabited cells
	int ul_y = pos_y;
	int ul_x = pos_x;
	int ul_cell_id = cg_get_cell_id_for_pos(ul_x, ul_y);
	if (ul_cell_id != -1) {
		cg_index->inhabited[cell_idx] = ul_cell_id;
		cell_idx += 1;
		// Update cells inhabitants
		CollisionGridCell *cell = &cg->cells[ul_cell_id];
		cell->num_inhabitants += sba_insert(cell->inhabitants, cg_index->id, cell->num_inhabitants);
	}

	int ur_y = pos_y;
	int ur_x = pos_x + width;
	int ur_cell_id = cg_get_cell_id_for_pos(ur_x, ur_y);
	if (ur_cell_id != -1) {
		cg_index->inhabited[cell_idx] = ur_cell_id;
		cell_idx += 1;
		// Update cells inhabitants
		CollisionGridCell *cell = &cg->cells[ur_cell_id];
		cell->num_inhabitants += sba_insert(cell->inhabitants, cg_index->id, cell->num_inhabitants);
	}

	int br_y = pos_y + height;
	int br_x = pos_x + width;
	int br_cell_id = cg_get_cell_id_for_pos(br_x, br_y);
	if (br_cell_id != -1) {
		cg_index->inhabited[cell_idx] = br_cell_id;
		cell_idx += 1;
		// Update cells inhabitants
		CollisionGridCell *cell = &cg->cells[br_cell_id];
		cell->num_inhabitants += sba_insert(cell->inhabitants, cg_index->id, cell->num_inhabitants);
	}

	int bl_y = pos_y + height;
	int bl_x = pos_x;
	int bl_cell_id = cg_get_cell_id_for_pos(bl_x, bl_y);
	if (bl_cell_id != -1) {
		cg_index->inhabited[cell_idx] = bl_cell_id;
		cell_idx += 1;
		// Update cells inhabitants
		CollisionGridCell *cell = &cg->cells[bl_cell_id];
		cell->num_inhabitants += sba_insert(cell->inhabitants, cg_index->id, cell->num_inhabitants);
	}


}

void cg_remove_index(CollisionGrid *cg, CollisionGridIndex *index) { 
	for (int cell_idx=0; cell_idx < MAX_INHABITED_CELLS; cell_idx++) {
		if (index->inhabited[cell_idx] == -1) { break; }

		int cell_id = index->inhabited[cell_idx];

		CollisionGridCell *cell = &cg->cells[cell_id];
		cell->num_inhabitants -= sba_remove(cell->inhabitants, index->id, cell->num_inhabitants);

		index->inhabited[cell_idx] = -1;
	}


}

int cg_does_cell_exist(int row, int col) {
	return 0 <= row && row < CELLS_DOWN && 0 <= col && col < CELLS_ACROSS;
}

CollisionGridCell cg_get_cell_at_pos(CollisionGrid *cg, float pos_x, float pos_y) {
	CollisionGridCell fail = {
		.id = -1,
		.num_inhabitants = 0,
		.inhabitants = NULL,
	};

	int cell_row = pos_y / CELL_SIZE;
	int cell_col = pos_x / CELL_SIZE;
	if (!cg_does_cell_exist(cell_row, cell_col)) { return fail; }

	int cell_id = (cell_row * CELLS_ACROSS) + cell_col;
	return cg->cells[cell_id];

}

//int cg_get_colliding_ids(int *id_arr, ) {
//}


#endif // _COLGRID_IMPLEMENTATION_

#endif
