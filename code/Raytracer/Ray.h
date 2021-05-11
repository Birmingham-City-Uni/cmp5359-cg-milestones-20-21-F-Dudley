#pragma once

#include "geometry.h"

class Ray {
public:

private:
	Point3f origin;
	Vec3f direction;

public:
	Ray() {}
	Ray(const Point3f& _origin, const Vec3f& _direction)
		: origin(_origin), direction(_direction) {

	}

	Point3f GetOrigin() const { return origin; }
	Vec3f GetDirection() const { return direction; }

	Point3f at(double _t) const {
		return origin + _t * direction;
	}

private:

};