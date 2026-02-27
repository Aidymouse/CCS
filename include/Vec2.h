#include <math.h>

#ifndef _VEC2_H_
#define _VEC2_H_

typedef struct Vec2 {
	double x;
	double y;
} Vec2;

double to_degrees(double radians);
double to_radians(double degrees);
Vec2 Vec2Add(Vec2 v1, Vec2 v2);
Vec2 Vec2Sub(Vec2 v1, Vec2 v2);
Vec2 Vec2AddScalar(Vec2 v1, float s);
Vec2 Vec2MultScalar(Vec2 v, float s);
float Vec2GetAngle(Vec2 v);
float Vec2GetMagnitude(Vec2 v);
Vec2 Vec2Normalize(Vec2 v);
Vec2 Vec2Rotate(Vec2 v, float degrees);
Vec2 GetVec2FromAngle(float degrees);

#ifdef VEC2_IMPLEMENTATION

double to_degrees(double radians) {
    return radians * (180.0 / M_PI);
}
double to_radians(double degrees) {
	return degrees / (180.0 / M_PI);
}

Vec2 Vec2Add(Vec2 v1, Vec2 v2) {
	Vec2 added;
	added.x = v1.x + v2.x;
	added.y = v1.y + v2.y;
	return added;
}

Vec2 Vec2AddScalar(Vec2 v1, float s) {
	Vec2 added;
	added.x = v1.x + s;
	added.y = v1.y + s;
	return added;
}

Vec2 Vec2MultScalar(Vec2 v, float s) {
	Vec2 mult;
	mult.x = v.x * s;
	mult.y = v.y * s;
	return mult;
}

float Vec2GetAngle(Vec2 v) {
	// Get's angle in degrees assuming ? is 0
	float a = atan2(v.y, v.x);
	return to_degrees(a);
}

float Vec2GetMagnitude(Vec2 v) {
	return sqrt(v.x*v.x + v.y*v.y);
}

Vec2 Vec2Normalize(Vec2 v) {
	float mag = Vec2GetMagnitude(v);
	Vec2 norm;
	norm.x = v.x / mag;
	norm.y = v.y / mag;
	return norm;
}

Vec2 Vec2Rotate(Vec2 v, float degrees) {
	Vec2 rotated;
	float rad = to_radians(degrees);
	rotated.x = v.x * cos(rad) - v.y * sin(rad);
	rotated.y = v.x * sin(rad) + v.y * cos(rad);
	return rotated;
}

/** Assumes ?? is direclty right. Gets a unit vector */
Vec2 GetVec2FromAngle(float degrees) {
	// Get adjacent
	float rads = to_radians(degrees);

	float x = cos(rads);
	float y = sin(rads);

	Vec2 from_angle;
	from_angle.x = x;
	from_angle.y = y;
	return from_angle;

}

Vec2 Vec2Sub(Vec2 v1, Vec2 v2) {
	Vec2 subbed;
	subbed.x = v1.x - v2.x;
	subbed.y = v1.y - v2.y;
	return subbed;
}
#endif

#endif // _VEC2_H_
