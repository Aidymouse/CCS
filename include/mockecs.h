/** An Entity-Component-System implemented in C using macro trickery.
 *
 * Usage: 
 * 1. define ECS_INCLUDE with the include path to where your component structs are
 * 2. define ECS_IMPLEMENTATION to get methods
 * 3. define COMPONENTS \ Component(<component name>) \ ... for as many components as you have
 * 4. include ecs.h
 */

#ifndef _ECS_H_
#define _ECS_H_

#ifdef ECS_IMPLEMENTATION

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>


#ifdef ECS_INCLUDE
#include ECS_INCLUDE
#endif

#endif // ECS_IMPLEMENTATION - though it's used again


#define MAX_ENTITIES 4
#define MAX_COMPONENTS 32
#define MAX_SYSTEMS 8

// Stolen from stack overflow
// TODO: credit
#define GetBit(var, bit) ((var & (1 << bit)) != 0) // Returns true / false if bit is set
#define SetBit(var, bit) (var |= (1 << bit))
#define FlipBit(var, bit) (var ^= (1 << bit))

/** Construct components enum **/
typedef enum Components {
	C_Position,
	C_Velocity
} Components;

// Component name strings for logging
const char* ComponentNames[] = {
	"Position",
	"Velocity"
};

/** ECS Typedefs **/
typedef int Entity;
typedef uint32_t Signature; // Limits max components to 32 tho...

/** Component Arrays **/
typedef struct Components_Position {
	Position components[MAX_ENTITIES];
	Entity ent_to_comp_idx[MAX_ENTITIES];
	Entity comp_idx_to_ent[MAX_ENTITIES];
	int num_components;
} Components_Position;

typedef struct Components_Velocity {
	Velocity components[MAX_ENTITIES];
	Entity ent_to_comp_idx[MAX_ENTITIES];
	Entity comp_idx_to_ent[MAX_ENTITIES];
	int num_components;
} Components_Velocity;

//void remove_component

/** System Data **/
typedef struct System {
	Entity registered_entities[MAX_ENTITIES];
	Signature required_signature;
} System;





/* ECS */
typedef struct ECS {
	Entity free_entities[MAX_ENTITIES];
	int free_ent_cursor; // Points to latest free entity.

	/* Ent id = idx into this arr */
	Signature signatures[MAX_ENTITIES];

	/** Add component arrays **/
	Components_Position components_Position;
	Components_Velocity components_Velocity;
	
	System systems[MAX_SYSTEMS];
} ECS;


/** DECLARATIONS **/

// ECS Methods
void init_ecs(ECS *ecs);
Entity add_entity(ECS *ecs);
void remove_entity(ECS *ecs, Entity ent);
Signature* get_signature_for_entity(ECS *ecs, Entity ent);
bool entity_exists(ECS *ecs, Entity ent);
bool entity_has_component(ECS *ecs, Entity ent, Components comp);

void* add_component(ECS *ecs, Entity ent, Components comp);
void remove_component(ECS *ecs, Entity ent, Components comp);

#ifdef ECS_IMPLEMENTATION

/*****************/
/** ECS METHODS **/
/*****************/

void init_ecs(ECS *ecs) {
	for (int e = 0; e < MAX_ENTITIES; e++) {
		ecs->free_entities[e] = MAX_ENTITIES-e; // 0 is not a valid entity ID
		ecs->signatures[e] = 0;
	}
	ecs->free_ent_cursor = MAX_ENTITIES-1;

	// TODO: component arrays (right now we only set the num components)
	ecs->components_Position.num_components=0;
	for (int i=0; i<MAX_ENTITIES; i++) {
		ecs->components_Position.components[i] = (Position){ 0 };
		ecs->components_Position.comp_idx_to_ent[i] = 0;
		ecs->components_Position.ent_to_comp_idx[i] = -1;

		ecs->components_Velocity.components[i] = (Velocity){ 0 };
		ecs->components_Velocity.comp_idx_to_ent[i] = 0;
		ecs->components_Velocity.ent_to_comp_idx[i] = -1;
	}


}


/* Adds an entity, returning it's ID. 0 is not a valid Entity ID, so this can be used like a boolean to see if an entity was returned */
Entity add_entity(ECS *ecs) {
	if (ecs->free_ent_cursor == -1) {
		return 0;
	}

	Entity new_ent = ecs->free_entities[ecs->free_ent_cursor];
	ecs->free_entities[ecs->free_ent_cursor] = 0;
	ecs->signatures[new_ent-1] = 1;
	ecs->free_ent_cursor -= 1;
	return new_ent;
}

void remove_entity(ECS *ecs, Entity ent) {
	if (*get_signature_for_entity(ecs, ent) == 0) {
		// This entity doesn't exist actually
		return;
	}

	ecs->signatures[ent-1] = 0;
	ecs->free_ent_cursor += 1;
	ecs->free_entities[ecs->free_ent_cursor] = ent;

	// TODO: unregister from systems and components
}

/* Get's an entities signature */
Signature* get_signature_for_entity(ECS *ecs, Entity ent) {
	return &ecs->signatures[ent-1]; // -1 as 0 is not a valid entity ID
}

bool entity_exists(ECS *ecs, Entity ent) {
	return GetBit(*get_signature_for_entity(ecs, ent), 0) == 1;
}

bool entity_has_component(ECS *ecs, Entity ent, Components comp) {
	Signature *sig = get_signature_for_entity(ecs, ent);
	return GetBit(*sig, (comp+1)) == 1;
}

/***********************/
/** COMPONENT METHODS **/
/***********************/

// Pointers are tricky, because they can outdate when you change components on an entity. Be careful!
/** Adds a component to an entity, then returns a pointer to it (for your own init purposes) */
void* add_component(ECS *ecs, Entity ent, Components comp) {

	Signature *sig = get_signature_for_entity(ecs, ent);

	if (*sig == 0) { 
		printf("Can't add %s to Entity %d, it does not exist\n", ComponentNames[comp], ent);
		return NULL;
	}

	//int ca = comp+1;
	if (entity_has_component(ecs, ent, comp)) { 
		printf("Can't add %s to Entity %d, it already has it\n", ComponentNames[comp], ent);
		return NULL;
	}

	SetBit(*sig, (comp+1));

	if (comp == C_Position) {
		Components_Position *comp_array = &ecs->components_Position;
		Position *component = &comp_array->components[comp_array->num_components];
		comp_array->ent_to_comp_idx[ent-1] = comp_array->num_components;
		comp_array->comp_idx_to_ent[comp_array->num_components] = ent;
		comp_array->num_components++;
		return component;
	}

	if (comp == C_Velocity) {
		Components_Velocity *comp_array = &ecs->components_Velocity;
		Velocity *component = &comp_array->components[comp_array->num_components];
		comp_array->ent_to_comp_idx[ent-1] = comp_array->num_components;
		comp_array->comp_idx_to_ent[comp_array->num_components] = ent;
		comp_array->num_components++;
		return component;
	}


	return 0;

}

void remove_component(ECS *ecs, Entity ent, Components comp) {

	if (!entity_has_component(ecs, ent, comp)) {
		printf("Tried to remove %s from Entity %d, but it does not have it\n", ComponentNames[comp], ent);
		return;
	}

	
	/* Generic */
	if (comp == C_Position) {
		Components_Position *comp_array = &ecs->components_Position;
		int comp_idx = comp_array->ent_to_comp_idx[ent-1];
		int last_idx = comp_array->num_components-1;

		Entity at_end = comp_array->comp_idx_to_ent[last_idx];
		comp_array->ent_to_comp_idx[at_end-1] = comp_idx;
		comp_array->comp_idx_to_ent[comp_idx] = at_end;

		comp_array->ent_to_comp_idx[ent-1] = -1;
		comp_array->comp_idx_to_ent[last_idx] = 0;

		comp_array->components[comp_idx] = comp_array->components[last_idx];
		comp_array->num_components -= 1;
	}
	if (comp == C_Velocity) {
		Components_Velocity *comp_array = &ecs->components_Velocity;
		int comp_idx = comp_array->ent_to_comp_idx[ent-1];
		int last_idx = comp_array->num_components-1;

		Entity at_end = comp_array->comp_idx_to_ent[last_idx];
		comp_array->ent_to_comp_idx[at_end-1] = comp_idx;
		comp_array->comp_idx_to_ent[comp_idx] = at_end;

		comp_array->ent_to_comp_idx[ent-1] = -1;
		comp_array->comp_idx_to_ent[last_idx] = 0;

		comp_array->components[comp_idx] = comp_array->components[last_idx];
		comp_array->num_components -= 1;
	}
	/* /Generic */


}


/** Component Fns */
// Might do it this way
/*
#define Component(c) *##c add_##c##_to_entity(ECS *ecs, Entity *entity) { \
	}

void remove_component_from_entity() {
}

void* add_component_to_entity() {
	return 0;
}
*/
 
// TODO

#undef Component



/** System functons **/
//#define X(c) void sys_##c(ECS *ecs, System *sys, Entity ent);

//SYSTEMS

//#undef X

#endif // ECS_IMPLEMENTATION
#endif // _ECS_H_


