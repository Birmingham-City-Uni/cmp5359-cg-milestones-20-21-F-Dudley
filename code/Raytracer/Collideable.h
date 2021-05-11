#pragma once

#include "Ray.h"

class Material;

struct Hit_Record {
	Point3f point;
	Vec3f normal;
	double t;
	bool front_face;

	std::shared_ptr<Material> material_ptr;

	inline void set_face_normal(const Ray& _ray, const Vec3f& _outward_normal) {
		front_face = (_ray.GetDirection().dotProduct(_outward_normal)) < 0;
		normal = front_face ? _outward_normal : -_outward_normal;
	}
};

class Collideable {
public:


protected:


public:
	virtual bool Hit(const Ray& _ray, double _t_min, double _t_max, Hit_Record& _record) const = 0;

protected:


};