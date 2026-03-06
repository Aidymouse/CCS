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
int point_in_circle(float px, float py, float cx, float cy, float cr);
int rect_circle(float rx, float ry, float rw, float rh, float cx, float cy, float cr);

int point_in_square_shapes(CollisionShape point, CollisionShape rect);

int rect_circle_shapes(CollisionShape rect, CollisionShape circle);

#ifdef COLLISIONS_IMPLEMENTATION

int point_in_square(float px, float py, float rx, float ry, float rw, float rh) {
	return px >= rx && px <= rx+rw && py >= ry && py <= ry+rh;
}

int point_in_circle(float px, float py, float cx, float cy, float cr) {
	return (px-cx)*(px-cx) + (py-cy)*(py-cy) <= (cr*cr);
}

int rect_circle(float rx, float ry, float rw, float rh, float cx, float cy, float cr) {
	// Initializing this to cx and cy stops phsing through the circle sometimes somehow
	float testX = cx;
	float testY = cy;

	if (cx <= rx) { testX = rx; }
	else if (cx >= rx+rw) { testX = rx+rw; }

	if (cy <= ry) { testY = ry; }
	else if (cy >= ry+rh) { testY = ry+rh; }

	float distX = cx-testX;
	float distY = cy-testY;
	return (distX*distX) + (distY*distY) < (cr*cr);
}

int point_in_square_shapes(CollisionShape point, CollisionShape rect) {
	return point_in_square(
		point.x + point.offset_x, point.y + point.offset_y,
		rect.x + rect.offset_x, rect.y + rect.offset_y, rect.data.rect.width, rect.data.rect.height
	);
}

int point_in_cicle_shapes(CollisionShape point, CollisionShape circle) {
	return point_in_circle(
		point.x + point.offset_x, point.y + point.offset_y,
		circle.x + circle.offset_x, circle.y + circle.offset_y, circle.data.circle.radius
	);
}

int rect_circle_shapes(CollisionShape rect, CollisionShape circle) {
	return rect_circle(
		rect.x+rect.offset_x, rect.y+rect.offset_y, rect.data.rect.width, rect.data.rect.height,
		circle.x+circle.offset_x, circle.y+circle.offset_y, circle.data.circle.radius
	);
}

#endif // COLLISIONS_IMPLEMENTATION
#endif // _H_COLLISIONS_
