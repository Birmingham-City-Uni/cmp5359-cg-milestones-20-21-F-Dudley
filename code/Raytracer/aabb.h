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

	AABB(const Point3f& _minimum, const Point3f& _maximum) 
	: minimum(_maximum), maximum(_maximum) {
		
	}

	Point3f min() {
		return minimum;
	}

	Point3f max() {
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

			if (_t_max <= _t_min)
				return false;
		}

		return true;
	}

private:


};

AABB Surrounding_Box(AABB box0, AABB box1) {
	Point3f small(fmin(box0.min().x - 1e-3, box1.min().x - 1e-3),
		fmin(box0.min().y - 1e-3, box1.min().y - 1e-3),
		fmin(box0.min().z - 1e-3, box1.min().z - 1e-3));

	Point3f big(fmax(box0.max().x + 1e-3, box1.max().x + 1e-3),
		fmax(box0.max().y + 1e-3, box1.max().y + 1e-3),
		fmax(box0.max().z + 1e-3, box1.max().z + 1e-3));

	return AABB(small, big);
}