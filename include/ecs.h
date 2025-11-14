/** An Entity-Component-System implemented in C using macro trickery.
 *
 * Usage: 
 * 1. define CCS_CCS_COMPONENTS \ Component(<component name>, <component bit>) \ ... in components.h
 * 2. define CCS_CCS_SYSTEMS \ System(<component name>, <component enum 1> | <component enum 2> | ...) \ ... in systems.h 
 * 3. include ecs.h
 *
 ** Example **
 *
 * components.h
 * 		#define CCS_COMPONENTS \
 * 			Component(Position, 1) \
 * 			Component(Velocity, 2) \
 *
 * 		typedef struct Position { int x; int y; } Position;
 * 		typedef struct Velocity { int x_speed; int y_speed; } Velocity;
 *
 * systems.h
 * 		#define CCS_SYSTEMS \
 * 			System(Move, C_Position | C_Velocity) \
 *
 * main.c
 * 		#include "ecs.h"
 */

#ifndef _ECS_H_
#define _ECS_H_

#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

#include "components.h"
#include "systems.h"

#define MAX_CCS_ENTITIES 5
#define MAX_CCS_COMPONENTS 32
#define MAX_CCS_SYSTEMS 8

// Stolen from stack overflow
// TODO: credit
#define GetBit(var, bit) (( (var) & (1 << bit)) != 0) // Returns true / false if bit is set
#define SetBit(var, bit) (var |= (1 << bit))
#define UnsetBit(var, bit) (var -= (1 << bit))
#define FlipBit(var, bit) (var ^= (1 << bit))


/*************/
/** STRUCTS **/
/*************/

/** ECS Typedefs **/
typedef int Entity;
typedef uint32_t Signature; // Limits max components to 32 tho...

/** Construct components enum **/
#define Component(c, num) C_##c,
typedef enum Components {
	CCS_COMPONENTS
} Components;
#undef Component

#define Component(c, num) CB_##c = 1 << num,
typedef enum ComponentBit {
	CCS_COMPONENTS
} ComponentBit;
#undef Component


/** Component Arrays **/
#define Component(c, _) typedef struct Components_##c { \
	c components[MAX_CCS_ENTITIES]; \
	Entity ent_to_comp_idx[MAX_CCS_ENTITIES]; \
	Entity comp_idx_to_ent[MAX_CCS_ENTITIES]; \
	int num_components; \
} Components_##c; \

CCS_COMPONENTS

#undef Component

#define System(name, sig) S_##name,
typedef enum Systems {
	CCS_SYSTEMS
} Systems;
#undef System


/** System Data **/
typedef struct System {
	Entity registered_entities[MAX_CCS_ENTITIES];
	Entity ent_to_reg_idx[MAX_CCS_ENTITIES]; // Entity-1 is index into this array = idx into registered arr
	Signature required_signature;
	int num_registered;
} System;

/* ECS */
typedef struct ECS {
	Entity free_entities[MAX_CCS_ENTITIES];
	int free_ent_cursor; // Points to latest free entity.

	/* Ent id = idx into this arr */
	Signature signatures[MAX_CCS_ENTITIES];

	/** Add component arrays **/
	#define Component(c, _) Components_##c components_##c;
	CCS_COMPONENTS
	#undef Component
	
	System systems[MAX_CCS_SYSTEMS];
	int num_systems;
} ECS;





/** DECLARATIONS **/

// ECS Methods
void ccs_init_ecs(ECS *ecs);
Entity ccs_add_entity(ECS *ecs);
void ccs_remove_entity(ECS *ecs, Entity ent);
Signature* ccs_get_signature_for_entity(ECS *ecs, Entity ent);
bool ccs_entity_exists(ECS *ecs, Entity ent);
bool ccs_entity_has_component(ECS *ecs, Entity ent, Components comp);
bool ccs_entity_satisfies_signature(Signature entities, Signature test);

void* ccs_add_component(ECS *ecs, Entity ent, Components comp);
void ccs_remove_component(ECS *ecs, Entity ent, Components comp);
void* ccs_get_component(ECS *ecs, Entity ent, Components comp);

void ccs_register_with_system(ECS *ecs, Entity ent, System *system);
void ccs_deregister_from_system(ECS *ecs, Entity ent, System *system);


#ifdef ECS_IMPLEMENTATION

// Component name strings for logging
#define Component(c, _) #c,
const char* ComponentNames[] = {
	CCS_COMPONENTS
};
#undef Component

#define System(name, sig) #name,
const char* SystemNames[] = {
	CCS_SYSTEMS
};
#undef System

/*****************/
/** ECS METHODS **/
/*****************/

void ccs_init_ecs(ECS *ecs) {
	for (int e = 0; e < MAX_CCS_ENTITIES; e++) {
		ecs->free_entities[e] = MAX_CCS_ENTITIES-e; // 0 is not a valid entity ID
		ecs->signatures[e] = 0;
	}
	ecs->free_ent_cursor = MAX_CCS_ENTITIES-1;

	// TODO: component arrays (right now we only set the num components)
	#define Component(c, _) ecs->components_##c.num_components=0; \
	for (int i=0; i<MAX_CCS_ENTITIES; i++) { \
		ecs->components_##c.components[i] = (c){ 0 }; \
		ecs->components_##c.comp_idx_to_ent[i] = 0; \
		ecs->components_##c.ent_to_comp_idx[i] = -1; \
	} \

	CCS_COMPONENTS

	#undef Component


	// Clear out systems
	for (int i=0; i<MAX_CCS_SYSTEMS; i++) {
		ecs->systems[i] = (System){ 0 };
		for (int e=0; e<MAX_CCS_ENTITIES; e++) {
			ecs->systems[i].ent_to_reg_idx[e] = -1;
		}
	}
	
	// Set system signatures
	int sys_idx=0;
	#define System(name, sig) ecs->systems[sys_idx].required_signature = ( (sig) | 1); \
	sys_idx += 1; \

	CCS_SYSTEMS
	
	#undef System

	ecs->num_systems = sys_idx;
	printf("ECS: Number of Systems: %d\n", ecs->num_systems);

	//ecs->systems = { 0 };
}

/********************/
/** ENTITY METHODS **/
/********************/

/* Adds an entity, returning it's ID. 0 is not a valid Entity ID, so this can be used like a boolean to see if an entity was returned */
Entity ccs_add_entity(ECS *ecs) {
	if (ecs->free_ent_cursor == -1) {
		return 0;
	}

	Entity new_ent = ecs->free_entities[ecs->free_ent_cursor];
	ecs->free_entities[ecs->free_ent_cursor] = 0;
	ecs->signatures[new_ent-1] = 1;
	ecs->free_ent_cursor -= 1;
	return new_ent;
}

void ccs_remove_entity(ECS *ecs, Entity ent) {
	if (*ccs_get_signature_for_entity(ecs, ent) == 0) {
		// This entity doesn't exist actually
		return;
	}

	Signature *sig = ccs_get_signature_for_entity(ecs, ent);

	#define Component(c, _) if (GetBit(*sig, (C_##c + 1))) { \
		ccs_remove_component(ecs, ent, C_##c); \
	} \

	CCS_COMPONENTS

	#undef Component
	
	ecs->signatures[ent-1] = 0;
	ecs->free_ent_cursor += 1;
	ecs->free_entities[ecs->free_ent_cursor] = ent;
	
}

/* Get's an entities signature */
Signature* ccs_get_signature_for_entity(ECS *ecs, Entity ent) {
	return &ecs->signatures[ent-1]; // -1 as 0 is not a valid entity ID
}

bool ccs_entity_exists(ECS *ecs, Entity ent) {
	return (GetBit(*ccs_get_signature_for_entity(ecs, ent), 0));
}

bool ccs_entity_has_component(ECS *ecs, Entity ent, Components comp) {
	Signature *sig = ccs_get_signature_for_entity(ecs, ent);
	return GetBit(*sig, (comp+1)) == 1;
}

bool ccs_entity_satisfies_signature(Signature entities, Signature test) {
	return (test & entities) == test;
}

/***********************/
/** COMPONENT METHODS **/
/***********************/

// !!! Pointers are tricky, because they can outdate when you change components on an entity. Be careful! !!!

/** Adds a component to an entity, then returns a pointer to it (for your own init purposes) 
 * 1. Checks if the entity exists
 * 2. Checks if the entity already has the component
 * */
void* ccs_add_component(ECS *ecs, Entity ent, Components comp) {

	Signature *sig = ccs_get_signature_for_entity(ecs, ent);

	if (*sig == 0) { 
		printf("ECS: Can't add %s to Entity %d, it does not exist\n", ComponentNames[comp], ent);
		return NULL;
	}

	if (ccs_entity_has_component(ecs, ent, comp)) { 
		printf("ECS: Can't add %s to Entity %d, it already has it\n", ComponentNames[comp], ent);
		return NULL;
	}

	SetBit(*sig, (comp+1));
	
	// Register with systems
	for (int i=0; i<ecs->num_systems; i++) {
		System *system = &ecs->systems[i];

		if (ccs_entity_satisfies_signature(*sig, system->required_signature)) {
			//printf("ECS: Entity %d qualifies for System %s (%d)\n", ent, SystemNames[i], i);
			ccs_register_with_system(ecs, ent, system);
		}
	}

	// Add the component (done last cos this will return)
	#define Component(c, _) if (comp == C_##c) { \
		Components_##c *comp_array = &ecs->components_##c; \
		c *component = &comp_array->components[comp_array->num_components]; \
		comp_array->ent_to_comp_idx[ent-1] = comp_array->num_components; \
		comp_array->comp_idx_to_ent[comp_array->num_components] = ent; \
		comp_array->num_components++; \
		return component; \
	} \

	CCS_COMPONENTS

	#undef Component


	return 0;

}

/* Removes a component from an entity
 * 1. Checks if the entity has the component
 * 2. Updates the signature
 * 3. Updates the component array (moves last component to current slot, remove the last componnt, updates records)
 * 4. Checks if the entity no longer qualifies for any systems and deregisters if so
 */
void ccs_remove_component(ECS *ecs, Entity ent, Components comp) {

	if (!ccs_entity_has_component(ecs, ent, comp)) {
		printf("ECS: Tried to remove %s from Entity %d, but it does not have it\n", ComponentNames[comp], ent);
		return;
	}

	Signature *sig = ccs_get_signature_for_entity(ecs, ent);
	UnsetBit(*sig, (comp+1));
	
	#define Component(c, _) if (comp == C_##c) { \
		Components_##c *comp_array = &ecs->components_##c; \
		int comp_idx = comp_array->ent_to_comp_idx[ent-1]; \
		int last_idx = comp_array->num_components-1; \
\
		Entity at_end = comp_array->comp_idx_to_ent[last_idx]; \
		comp_array->ent_to_comp_idx[at_end-1] = comp_idx; \
		comp_array->comp_idx_to_ent[comp_idx] = at_end; \
\
		comp_array->ent_to_comp_idx[ent-1] = -1; \
		comp_array->comp_idx_to_ent[last_idx] = 0; \
\
		comp_array->components[comp_idx] = comp_array->components[last_idx]; \
\
		comp_array->num_components -= 1; \
	} \

	CCS_COMPONENTS

	#undef Component

	// Remove from systems
	for (int i=0; i<ecs->num_systems; i++) {
		System *sys = &ecs->systems[i];

		if (!ccs_entity_satisfies_signature(*sig, sys->required_signature)) {
			//printf("ECS: Entity %d no longer qualifies for System %s\n", ent, SystemNames[i]);
			ccs_deregister_from_system(ecs, ent, sys);
		}
		
	}

}

/** Gets a component for an entity.
 * 1. Checks if entity has the component
 * 2. Acquires the component using records in component array
 */
void* ccs_get_component(ECS *ecs, Entity ent, Components comp) {

	if (!ccs_entity_has_component(ecs, ent, comp)) {
		printf("ECS: Can't get %s from Entity %d, it does not have it!\n", ComponentNames[comp], ent);
		return 0;
	}

	#define Component(c, _) if (comp == C_##c) { \
		Components_##c *comp_array = &ecs->components_##c; \
		int comp_idx = comp_array->ent_to_comp_idx[ent-1]; \
		return &comp_array->components[comp_idx]; \
	} \

	CCS_COMPONENTS

	#undef Component

	return 0;
}


/*********************/
/** SYSTEM FUNCTONS **/
/*********************/

// TODO: test
void ccs_register_with_system(ECS *ecs, Entity ent, System *system) {
	if (system->ent_to_reg_idx[ent-1] != -1) { return; }
	system->registered_entities[system->num_registered] = ent;
	system->ent_to_reg_idx[ent-1] = system->num_registered;
	system->num_registered += 1;
}

void ccs_deregister_from_system(ECS *ecs, Entity ent, System *system) {
	int ents_idx = system->ent_to_reg_idx[ent-1];
	int last_idx = system->num_registered-1;
	Entity last_ent = system->registered_entities[last_idx];

	system->registered_entities[ents_idx] = system->registered_entities[last_idx];
	system->registered_entities[last_idx] = 0; // Not technically needed

	system->ent_to_reg_idx[last_ent-1] = ents_idx;

	system->ent_to_reg_idx[ent-1] = -1; // Not technically needed

	system->num_registered -= 1;
}


//#define X(c) void sys_##c(ECS *ecs, System *sys, Entity ent);

//CCS_SYSTEMS

//#undef X

#endif // ECS_IMPLEMENTATION

#endif // _ECS_H_
