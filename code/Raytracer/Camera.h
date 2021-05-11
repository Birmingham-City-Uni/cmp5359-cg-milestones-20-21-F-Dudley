#pragma once

#include "geometry.h"
#include "Ray.h"
#include "Utils.h"

class Camera {
public:

private:
	Point3f origin;
	Point3f lower_left_corner;
	Vec3f horizontal;
	Vec3f vertical;

	Vec3f u, v, w;
	double lensRadius;

public:
	Camera(Point3f _lookFrom, Point3f _lookAt, Vec3f _vup, double _vFov, double _aspectRatio, double _aperture, double _focusDistance) {
		auto theta = Utils::degrees_to_radians(_vFov);
		auto height = tan(theta / 2);

		auto viewportHeight = 2.0 * height;
		auto viewportWidth = _aspectRatio * viewportHeight;
		
		// Vectors
		w = (_lookFrom - _lookAt).normalize();
		u = (_vup.crossProduct(w)).normalize();
		v = w.crossProduct(u);

		origin = _lookFrom;
		horizontal = _focusDistance * viewportWidth * u;
		vertical = _focusDistance * viewportHeight * v;
		lower_left_corner = origin - horizontal / 2 - vertical / 2 - _focusDistance * w;
		lensRadius = _aperture / 2;
	}

	Ray Get_Ray(double _s, double _t) {
		Vec3f rd = lensRadius * Vec3f().random_in_unit_disk();
		Vec3f offset = u * rd.x + v * rd.y;

		Point3f origin_ = origin + offset;
		Vec3f direction_ = lower_left_corner + _s * horizontal + _t * vertical - origin - offset;

		return Ray(origin_, direction_);
	}

private:

};