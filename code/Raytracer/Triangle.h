#pragma once

#include "geometry.h"
#include "Collideable.h"

class Triangle : public Collideable {
public:
	Point3f vertex0, vertex1, vertex2;
	Vec3f normals;
	std::shared_ptr<Material> material;

private:


public:
	Triangle() {

	}

	Triangle(Point3f _vertex0, Point3f _vertex1, Point3f _vertex2, std::shared_ptr<Material> _material) 
	: vertex0(_vertex0), vertex1(_vertex1), vertex2(_vertex2), material(_material) {

	}

	Triangle(Point3f _vertex0, Point3f _vertex1, Point3f _vertex2, Vec3f _normals, std::shared_ptr<Material> _material)
	: vertex0(_vertex0), vertex1(_vertex1), vertex2(_vertex2), normals(_normals), material(_material) {

	}

	~Triangle() {

	}

	bool Hit(const Ray& _ray, double _t_min, double _t_max, Hit_Record& _record) const override {
		Vec3f v0v1 = vertex1 - vertex0;
		Vec3f v0v2 = vertex2 - vertex0;

		Vec3f pvec = _ray.GetDirection().crossProduct(v0v2);

		float determinant = pvec.dotProduct(v0v1);
		float kEpsilon = 0.00001;

		// If Determinant is Negative it's backfacing.
		// If the Determinant is close to 0, the Ray Missed the Triangle Face.
		if (determinant < kEpsilon) return false;

		float invDeterminat = 1 / determinant;

		Vec3f tvec = _ray.GetOrigin() - vertex0;
		float u = tvec.dotProduct(pvec) * invDeterminat;
		if (u < 0 || u > 1) return false;

		Vec3f qvec = tvec.crossProduct(v0v1);
		float v = _ray.GetDirection().dotProduct(qvec) * invDeterminat;
		if (v < 0 || u + v > 1) return false;

		float t = v0v2.dotProduct(qvec) * invDeterminat;
		if (t < 0) return false;

		_record.point = _ray.at(t);
		_record.t = t;

		// FIX NORMAL CALCULATIONS
		_record.normal = normals;
		_record.material_ptr = material;

	}

	inline virtual bool Bounding_Box(AABB& _outputBox) const override {
		float min[3];
		float max[3];

		for (int i = 0; i < 3; i++)
		{
			min[i] = std::min(vertex0[i], std::min(vertex1[i], vertex2[i]));
			max[i] = std::max(vertex0[i], std::max(vertex1[i], vertex2[i]));
		}

		_outputBox = AABB(Vec3f(min[0], min[1], min[2]), Vec3f(max[0], max[1], max[2]));

		return true;
	}

private:


};
