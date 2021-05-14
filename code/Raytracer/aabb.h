#pragma once

#include "geometry.h"
#include "Ray.h"
#include "Utils.h"

class AABB {
public:
	Point3f minimum;
	Point3f maximum;

private:


public:

	AABB() {

	}

	AABB(const Point3f& _mini, const Point3f& _maxi) 
	: minimum(_mini), maximum(_maxi) {

	}

	Point3f min() const {
		return minimum;
	}

	Point3f max() const {
		return maximum;
	}

	bool Hit(const Ray& _ray, double _t_min, double _t_max) const {
		for (int a = 0; a < 3; a++)
		{
			auto t0 = fmin((minimum[a] - _ray.GetOrigin()[a]) / _ray.GetDirection()[a], 
						   (maximum[a] - _ray.GetOrigin()[a]) / _ray.GetDirection()[a]);

			auto t1 = fmax((minimum[a] - _ray.GetOrigin()[a]) / _ray.GetDirection()[a],
						   (maximum[a] - _ray.GetOrigin()[a]) / _ray.GetDirection()[a]);

			_t_min = fmax(t0, _t_min);
			_t_max = fmin(t1, _t_max);

			if (_t_max <= _t_max) return false;
		}

		return false;
	}

private:


};

AABB surrounding_box(AABB _box0, AABB _box1) {
	Point3f small(fmin(_box0.min().x - 1e-3, _box1.min().x - 1e-3), // x
				  fmin(_box0.min().y - 1e-3, _box0.min().y - 1e-3), // y
				  fmin(_box0.min().z - 1e-3, _box1.min().z - 1e-3));// z

	Point3f big(fmax(_box0.max().x + 1e-3, _box1.max().x + 1e-3), // x
				fmax(_box0.max().y + 1e-3, _box0.max().y + 1e-3), // y
				fmax(_box0.max().z + 1e-3, _box1.max().z + 1e-3));// z

	return AABB(small, big);
}