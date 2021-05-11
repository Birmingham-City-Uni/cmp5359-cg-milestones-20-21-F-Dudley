#pragma once

#include "geometry.h"
#include "Collideable.h"
#include "Material.h"

class Sphere : public Collideable {
public:
	Point3f centre;
	double radius;
	std::shared_ptr<Material> material_ptr;

private:


public:
	Sphere() {

	};

	Sphere(Point3f _centre, double _radius, std::shared_ptr<Material> _material) : centre(_centre), radius(_radius), material_ptr(_material) {

	}

	virtual bool Hit(const Ray& _ray, double _t_min, double _t_max, Hit_Record& _record) const override {
		Vec3f oc = _ray.GetOrigin() - centre;

		auto a = _ray.GetDirection().norm();
		auto half_b = oc.dotProduct(_ray.GetDirection());
		auto c = oc.norm() - radius * radius;

		auto discriminant = half_b * half_b - a * c;
		if (discriminant < 0) {
			return false;
		}

		auto sqrtd = sqrt(discriminant);
		
		// Check Root With Provided Range.
		auto root = (-half_b - sqrtd) / a;
		if (root < _t_min || _t_max < root) {
			root = (-half_b + sqrtd) / a;

			if (root < _t_min || _t_max < root) return false;
		}

		// Update the Castable Objects Hit_Records.
		_record.t = root;
		_record.point = _ray.at(_record.t);

		Vec3f outward_normal = (_record.point - centre) / radius;
		_record.set_face_normal(_ray, outward_normal);
		_record.material_ptr = material_ptr;

		return true;
	}

private:


};