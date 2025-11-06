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

#ifdef ECS_INCLUDE
#include ECS_INCLUDE
#endif

#ifndef ECS_INCLUDE
// TODO: default defines for components so the program doesn't crash
#endif

#endif

#define MAX_ENTITIES 64
#define MAX_COMPONENTS 64
#define MAX_SYSTEMS 8



/** Construct components enum **/
#define Component(c) C_##c,
typedef enum Components {
	COMPONENTS
} Components;
#undef Component

typedef int Entity;
typedef int Signature;

/** Component Arrays **/
#define Component(c) typedef struct Components_##c { \
	c components[MAX_ENTITIES]; \
	Entity ent_to_comp_idx[MAX_ENTITIES]; \
	Entity comp_idx_to_ent[MAX_ENTITIES]; \
	int num_components; \
} Components_##c; \

COMPONENTS

#undef Component

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
	#define Component(c) Components_##c components_##c;
	COMPONENTS
	#undef Component
	
	System systems[MAX_SYSTEMS];
} ECS;


void init_ecs(ECS *ecs);
Signature get_signature_for_entity(ECS *ecs, Entity ent);
Entity add_entity(ECS *ecs);
void remove_entity(ECS *ecs, Entity ent);

#ifdef ECS_IMPLEMENTATION

/** ECS METHODS */

void init_ecs(ECS *ecs) {
	for (int e = 0; e < MAX_ENTITIES; e++) {
		ecs->free_entities[e] = MAX_ENTITIES-e; // 0 is not a valid entity ID
		ecs->signatures[e] = 0;
	}
	ecs->free_ent_cursor = MAX_ENTITIES-1;

	// TODO: component arrays
	#define Component(c) ecs->components_##c.num_components=0;
	COMPONENTS
	#undef Component

	//ecs->systems = { 0 };
}

Signature get_signature_for_entity(ECS *ecs, Entity ent) {
	return ecs->signatures[ent-1]; // -1 as 0 is not a valid entity ID
}

/* Adds an entity, returning it's ID. 0 is not a valid Entity ID, so this can be used like a boolean to see if an entity was returned */
Entity add_entity(ECS *ecs) {
	if (ecs->free_ent_cursor == -1) {
		return 0;
	}

	Entity new_ent = ecs->free_entities[ecs->free_ent_cursor];
	ecs->signatures[new_ent-1] = 1;
	ecs->free_ent_cursor -= 1;
	return new_ent;
}

void remove_entity(ECS *ecs, Entity ent) {
	if (get_signature_for_entity(ecs, ent) == 0) {
		// This entity doesn't exist actually
		return;
	}

	ecs->signatures[ent-1] = 0;
	ecs->free_ent_cursor += 1;
	ecs->free_entities[ecs->free_ent_cursor] = ent;
}



/** COMPONENT FNS **/
// Pointers are tricky, because they can outdate when you change components on an entity. Be careful!
void* add_component(ECS *ecs, Entity ent, Components comp) {

	// TODO: validate entity is real n stuff
	// TODO: update signature

	

	#define Component(c) if (comp == C_##c) {  \
		Components_##c *comp_array = &ecs->components_##c; \
		c *component = &comp_array->components[comp_array->num_components]; \
		comp_array->ent_to_comp_idx[ent] = comp_array->num_components; \
		comp_array->comp_idx_to_ent[comp_array->num_components] = ent; \
		comp_array->num_components++; \
		return component; \
	} \

	COMPONENTS

	#undef Component

	return 0;

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

#endif // _ECD_IMPL_

#endif // _ECS_H_
