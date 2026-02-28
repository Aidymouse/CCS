#ifndef _COLLISIONS_H_
#define _COLLISIONS_H_


// Types
typedef enum CollisionShapes {
	COL_CIRCLE,
	COL_RECT,
	COL_POINT,
} CollisionShapes;

typedef struct CollisionCircle {
	float x;
	float y;
	float radius;
} CollisionCircle;

typedef struct CollisionPoint {
	float x;
	float y;
} CollisionPoint;

typedef struct CollisionRect {
	float x;
	float y;
	float width;
	float height;
} CollisionRect;

typedef struct CollisionShape {
	enum CollisionShapes type;
	union {
		CollisionCircle circle;
		CollisionRect rect;
		CollisionPoint point;
	} data;
} CollisionShape;

/* Declarations */
int point_in_square(float px, float py, float rx, float ry, float rw, float rh);

#ifdef COLLISIONS_IMPLEMENTATION
#endif

int point_in_square(float px, float py, float rx, float ry, float rw, float rh) {
	return px >= rx && px <= rx+rw && py >= ry && py <= ry+rh;
}

#endif
