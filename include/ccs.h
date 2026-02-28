/** An CCS_Entity-Component-CCS_System implemented in C using macro trickery.
 *
 * Usage:  TODO:
 * 1. define CCS_CCS_COMPONENTS \ Component(<component name>, <component bit>) \ ... in components.h
 * 2. define CCS_SYSTEMS \ System(<component name>, <component enum 1> | <component enum 2> | ...) \ ... in systems.h
 * 3. include ecs.h
 *
 ** Options **
 * CCS_OPT_NO_HELPER_TYPES - if defined, helper types will not be generated. A helper type is a renamed typedef (e.g. CCS_Entity -> CCS_Entity)
 *
 ** Defines **
 * MAX_CCS_ENTITIES
 * MAX_CCS_COMPONENTS
 * MAX_CCS_SYSTEMS
 *
 ** Vars **
 * enum CCS_Components - format C_<component name>. Used to refer to components in the CCS
 * enum CCS_ComponentBit - Enum where the CB_<component name> = the bit position in a signature denoting that component. (I think this is unused?)
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
 * 			CCS_System(Move, C_Position | C_Velocity) \
 *
 * main.c
 * 		#include "ecs.h"
 */


#ifndef _CCS_H_
#define _CCS_H_

#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>


// You need to make sure CCS_SYSTEMS and CCS_COMPONENTS are defined, along with any component structs

// #include "components.h"
// #include "systems.h"

#define MAX_CCS_ENTITIES 4
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

/** CCS Typedefs **/
typedef int CCS_Entity;
typedef uint32_t CCS_Signature; // Limits max components to 32 tho...


/** Construct components enum **/
/** C_{component_name} **/
#define Component(c, num) C_##c,
typedef enum CCS_Components {
	CCS_COMPONENTS
} CCS_Components;
#undef Component

/** Construct enum of component to their bit position in the signature **/
/** CB_{component_name} = 2 to the power of position **/
#define Component(c, num) CB_##c = 1 << num,
typedef enum CCS_ComponentBit {
	CCS_COMPONENTS
} CCS_ComponentBit;
#undef Component


/** Component Arrays **/
#define Component(c, _) typedef struct CCS_Components_##c { \
	c components[MAX_CCS_ENTITIES]; \
	CCS_Entity ent_to_comp_idx[MAX_CCS_ENTITIES]; \
	CCS_Entity comp_idx_to_ent[MAX_CCS_ENTITIES]; \
	int num_components; \
} CCS_Components_##c; \

CCS_COMPONENTS

#undef Component

#define System(name, sig) S_##name,
typedef enum CCS_Systems {
	CCS_SYSTEMS
} CCS_Systems;
#undef System


/** CCS_System Data **/
typedef struct CCS_System {
    CCS_Entity registered_entities[MAX_CCS_ENTITIES];
	CCS_Entity ent_to_reg_idx[MAX_CCS_ENTITIES]; // CCS_Entity-1 is index into this array = idx into registered arr
	CCS_Signature required_signature;
	int num_registered;
	int id;
} CCS_System;

/* CCS */
typedef struct CCS {
	/** Stack of free entity IDs. Popped when an entity is made and pushed when an eneity is removed. **/
	CCS_Entity free_entities[MAX_CCS_ENTITIES];
	/** Index of latest free entity */
	int free_ent_cursor;

	/* Ent id = idx into this arr */
	CCS_Signature signatures[MAX_CCS_ENTITIES];

	/** Component Array Strcuts contain lists of component data + index data about which entities have the data */
	#define Component(c, _) CCS_Components_##c components_##c;
	CCS_COMPONENTS
	#undef Component

	/** Systems know what signature applies to them and keeps a record of registered entities */
	CCS_System systems[MAX_CCS_SYSTEMS];
	int num_systems;
} CCS;


/** OPTIONS **/

/** For ease of use **/
#ifndef CCS_OPT_NO_HELPER_TYPES
	typedef CCS_Entity Entity;
	typedef CCS_System System;
	typedef CCS_Signature Signature;
	typedef CCS ECS;
#endif



/** DECLARATIONS **/

// CCS Methods
void ccs_init_ecs(CCS *ecs);
CCS_Entity ccs_add_entity(CCS *ecs);
void ccs_remove_entity(CCS *ecs, CCS_Entity ent);
CCS_Signature* ccs_get_signature_for_entity(CCS *ecs, CCS_Entity ent);
bool ccs_entity_exists(CCS *ecs, CCS_Entity ent);
bool ccs_entity_has_component(CCS *ecs, CCS_Entity ent, CCS_Components comp);
bool ccs_entity_satisfies_signature(CCS_Signature entities, CCS_Signature test);

void* ccs_add_component(CCS *ecs, CCS_Entity ent, CCS_Components comp);
void ccs_remove_component(CCS *ecs, CCS_Entity ent, CCS_Components comp);
void* ccs_get_component(CCS *ecs, CCS_Entity ent, CCS_Components comp);

void ccs_register_with_system(CCS *ecs, CCS_Entity ent, CCS_System *system);
void ccs_deregister_from_system(CCS *ecs, CCS_Entity ent, CCS_System *system);


#ifdef CCS_IMPLEMENTATION

// Component name strings for logging
#define Component(c, _) #c,
const char* ComponentNames[] = {
	CCS_COMPONENTS
};
#undef Component

#define System(name, sig) #name,
const char* CCS_SystemNames[] = {
	CCS_SYSTEMS
};
#undef System

/*****************/
/** CCS METHODS **/
/*****************/

void ccs_init_ecs(CCS *ecs) {
	for (int e = 0; e < MAX_CCS_ENTITIES; e++) {
		ecs->free_entities[e] = MAX_CCS_ENTITIES - 1 - e;
		ecs->signatures[e] = 0;
	}
	ecs->free_ent_cursor = MAX_CCS_ENTITIES-1;

	// TODO: component arrays (right now we only set the num components)
	#define Component(c, _) ecs->components_##c.num_components=0; \
	for (int i=0; i<MAX_CCS_ENTITIES; i++) { \
		ecs->components_##c.components[i] = (c){ 0 }; \
		ecs->components_##c.comp_idx_to_ent[i] = -1; \
		ecs->components_##c.ent_to_comp_idx[i] = -1; \
	} \

	CCS_COMPONENTS

	#undef Component


	// Clear out systems
	for (int i=0; i<MAX_CCS_SYSTEMS; i++) {
		ecs->systems[i] = (CCS_System){ 0 };
		ecs->systems[i].id = i;
		for (int e=0; e<MAX_CCS_ENTITIES; e++) {
			ecs->systems[i].ent_to_reg_idx[e] = -1;
			ecs->systems[i].registered_entities[e] = -1;
		}
	}

	// Set system signatures
	int sys_idx=0;
	#define System(name, sig) \
		ecs->systems[sys_idx].required_signature = ( (sig) | 1); \
		sys_idx += 1; \

	CCS_SYSTEMS

	#undef System

	ecs->num_systems = sys_idx;
	printf("CCS: Number of CCS_Systems: %d\n", ecs->num_systems);

	//ecs->systems = { 0 };
}

/********************/
/** ENTITY METHODS **/
/********************/

/* Adds an entity, returning it's ID.
 * @returns -1 if an entity could not be added, or the entities ID if it was.
 */
CCS_Entity ccs_add_entity(CCS *ecs) {
	if (ecs->free_ent_cursor == -1) {
		printf("CCS: Cannot add entity as we are full!\n");
		return -1;
	}

	CCS_Entity new_ent = ecs->free_entities[ecs->free_ent_cursor];
	ecs->free_entities[ecs->free_ent_cursor] = -1;
	ecs->signatures[new_ent] = 1;
	ecs->free_ent_cursor -= 1;
	return new_ent;
}

void ccs_remove_entity(CCS *ecs, CCS_Entity ent) {
	if (*ccs_get_signature_for_entity(ecs, ent) == 0) {
		// This entity doesn't exist actually
		return;
	}

	CCS_Signature *sig = ccs_get_signature_for_entity(ecs, ent);

	#define Component(c, _) if (GetBit(*sig, (C_##c + 1))) { \
		ccs_remove_component(ecs, ent, C_##c); \
	} \

	CCS_COMPONENTS

	#undef Component

	ecs->signatures[ent] = 0;
	ecs->free_ent_cursor += 1;
	ecs->free_entities[ecs->free_ent_cursor] = ent;

}

/* Get's an entities signature */
CCS_Signature* ccs_get_signature_for_entity(CCS *ecs, CCS_Entity ent) {
	return &ecs->signatures[ent];
}

bool ccs_entity_exists(CCS *ecs, CCS_Entity ent) {
	return (GetBit(*ccs_get_signature_for_entity(ecs, ent), 0));
}

bool ccs_entity_has_component(CCS *ecs, CCS_Entity ent, CCS_Components comp) {
	CCS_Signature *sig = ccs_get_signature_for_entity(ecs, ent);
	return GetBit(*sig, (comp+1)) == 1;
}

bool ccs_entity_satisfies_signature(CCS_Signature entities, CCS_Signature test) {
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
void* ccs_add_component(CCS *ecs, CCS_Entity ent, CCS_Components comp) {

	CCS_Signature *sig = ccs_get_signature_for_entity(ecs, ent);

	if (*sig == 0) {
		printf("CCS: Can't add %s to CCS_Entity %d, it does not exist\n", ComponentNames[comp], ent);
		return NULL;
	}

	if (ccs_entity_has_component(ecs, ent, comp)) {
		printf("CCS: Can't add %s to CCS_Entity %d, it already has it\n", ComponentNames[comp], ent);
		return NULL;
	}

	SetBit(*sig, (comp+1));

	// Register with systems
	for (int i=0; i<ecs->num_systems; i++) {
		CCS_System *system = &ecs->systems[i];

		if (ccs_entity_satisfies_signature(*sig, system->required_signature)) {
			//printf("CCS: CCS_Entity %d qualifies for CCS_System %s (%d)\n", ent, CCS_SystemNames[i], i);
			ccs_register_with_system(ecs, ent, system);
		}
	}

	// Add the component (done last cos this will return)
	#define Component(c, _) if (comp == C_##c) { \
		CCS_Components_##c *comp_array = &ecs->components_##c; \
		c *component = &comp_array->components[comp_array->num_components]; \
		comp_array->ent_to_comp_idx[ent] = comp_array->num_components; \
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
void ccs_remove_component(CCS *ecs, CCS_Entity ent, CCS_Components comp) {

	if (!ccs_entity_has_component(ecs, ent, comp)) {
		printf("CCS: Tried to remove %s from CCS_Entity %d, but it does not have it\n", ComponentNames[comp], ent);
		return;
	}

	CCS_Signature *sig = ccs_get_signature_for_entity(ecs, ent);
	UnsetBit(*sig, (comp+1));

	#define Component(c, _) if (comp == C_##c) { \
		CCS_Components_##c *comp_array = &ecs->components_##c; \
		int comp_idx = comp_array->ent_to_comp_idx[ent]; \
		int last_idx = comp_array->num_components-1; \
\
		CCS_Entity at_end = comp_array->comp_idx_to_ent[last_idx]; \
		comp_array->ent_to_comp_idx[at_end-1] = comp_idx; \
		comp_array->comp_idx_to_ent[comp_idx] = at_end; \
\
		comp_array->ent_to_comp_idx[ent] = -1; \
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
		CCS_System *sys = &ecs->systems[i];

		if (!ccs_entity_satisfies_signature(*sig, sys->required_signature)) {
			printf("CCS: CCS_Entity %d no longer qualifies for CCS_System %s\n", ent, CCS_SystemNames[i]);
			ccs_deregister_from_system(ecs, ent, sys);
		}

	}

}

/** Gets a component for an entity.
 * 1. Checks if entity has the component
 * 2. Acquires the component using records in component array
 * @returns NULL (0) if component doesn't exist, otherwise pointer to component
 */
void* ccs_get_component(CCS *ecs, CCS_Entity ent, CCS_Components comp) {

	if (!ccs_entity_has_component(ecs, ent, comp)) {
		printf("CCS: Can't get %s from CCS_Entity %d, it does not have it!\n", ComponentNames[comp], ent);
		return 0;
	}

	#define Component(c, _) if (comp == C_##c) { \
		CCS_Components_##c *comp_array = &ecs->components_##c; \
		int comp_idx = comp_array->ent_to_comp_idx[ent]; \
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
void ccs_register_with_system(CCS *ecs, CCS_Entity ent, CCS_System *system) {
	if (system->ent_to_reg_idx[ent] != -1) {
	    printf("CCS: Attempting to register entity %d from system it is already registered with.\n", ent);
	    return;
	}
	system->registered_entities[system->num_registered] = ent;
	system->ent_to_reg_idx[ent] = system->num_registered;
	system->num_registered += 1;
}

void ccs_deregister_from_system(CCS *ecs, CCS_Entity ent, CCS_System *system) {
	int ents_idx = system->ent_to_reg_idx[ent];
	if (ents_idx == -1) {
	    printf("CCS: Attempting to de-register entity %d from system %d it is not registered in.\n", ent, system->id);
		return;
	}
	int last_idx = system->num_registered-1;
	CCS_Entity last_ent = system->registered_entities[last_idx];


	system->registered_entities[ents_idx] = system->registered_entities[last_idx];
	system->registered_entities[last_idx] = -1; // Not technically needed

	system->ent_to_reg_idx[last_ent] = ents_idx;

	system->ent_to_reg_idx[ent] = -1; // Not technically needed

	system->num_registered -= 1;
}


//#define X(c) void sys_##c(CCS *ecs, CCS_System *sys, CCS_Entity ent);

//CCS_SYSTEMS

//#undef X

#endif // CCS_IMPLEMENTATION

#endif // _CCS_H_
