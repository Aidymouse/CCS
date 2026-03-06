#ifndef _COLLISIONS_H_
#define _COLLISIONS_H_


// Types
typedef enum CollisionShapes {
	COL_CIRCLE,
	COL_RECT,
	COL_POINT,
} CollisionShapes;

typedef struct CollisionCircle {
	float radius;
} CollisionCircle;

typedef struct CollisionPoint {
} CollisionPoint;

typedef struct CollisionRect {
	float width;
	float height;
} CollisionRect;

typedef struct CollisionShape {
	enum CollisionShapes type;
	float x;
	float y;
	float offset_x;
	float offset_y;
	union {
		CollisionCircle circle;
		CollisionRect rect;
		CollisionPoint point;
	} data;
} CollisionShape;

/* Declarations */
int point_in_square(float px, float py, float rx, float ry, float rw, float rh);
int point_in_square_shapes(CollisionShape point, CollisionShape rect);

#ifdef COLLISIONS_IMPLEMENTATION
#endif

int point_in_square(float px, float py, float rx, float ry, float rw, float rh) {
	return px >= rx && px <= rx+rw && py >= ry && py <= ry+rh;
}

int point_in_square_shapes(CollisionShape point, CollisionShape rect) {
	return point_in_square(
		point.x + point.offset_x, point.y + point.offset_y,
		rect.x + rect.offset_x, rect.y + rect.offset_y, rect.data.rect.width, rect.data.rect.height
	);
}

#endif
