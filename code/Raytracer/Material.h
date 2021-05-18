#pragma once

#include "geometry.h"
#include "Ray.h"
#include "Collideable.h"
#include "Utils.h"

struct Hit_Record;

class Material {
public:
	Colour albedo;

protected:

public:
	virtual bool Scatter(const Ray& _ray_in, const Hit_Record& _record, Colour& _attenuation, Ray& _scattered) const = 0;

	virtual Vec3f Refract(const Vec3f& _uv, const Vec3f& _normal, double _etai_over_etat) const {

		auto cosTheta = fmin(-_uv.dotProduct(_normal), 1.0);
		
		Vec3f r_out_perp = _etai_over_etat * (_uv + cosTheta * _normal);
		Vec3f r_out_parallel = -sqrt(fabs(1.0 - r_out_perp.norm())) * _normal;

		Vec3f refracted_ = r_out_perp + r_out_parallel;

		return refracted_;
	}

	Vec3f Reflect(const Vec3f& _v, const Vec3f& _n) const {
		return _v - 2 * _v.dotProduct(_n) * _n;
	}

	virtual Colour emitted() const {
		return Colour(0);
	}

protected:

};

class Lambertian : public Material {
public:

private:

public:
	Lambertian(const Colour& _albedo) {
		albedo = _albedo;
	}

	virtual bool Scatter(const Ray& _ray_in, const Hit_Record& _record, Colour& _attenuation, Ray& _scattered) const override {
		auto scatter_direction = _record.normal + Vec3f().random_in_unit_sphere();

		if (scatter_direction.near_zero()) {
			scatter_direction = _record.normal;
		}

		_scattered = Ray(_record.point, scatter_direction);
		_attenuation = albedo;

		return true;
	}

private:


};

class Metal : public Material {
public:
	double fuzz;
private:

public:
	Metal(const Colour& _albedo, double _fuzz) : fuzz (_fuzz < 1 ? _fuzz : 1) {
		albedo = _albedo;
	}

	virtual bool Scatter(const Ray& _ray_in, const Hit_Record& _record, Colour& _attenuation, Ray& _scattered) const override {
		Vec3f reflected = Reflect(_ray_in.GetDirection().normalize(), _record.normal);
		
		_scattered = Ray(_record.point, reflected + fuzz * Vec3f().random_in_unit_sphere());
		_attenuation = albedo;

		return (_scattered.GetDirection().dotProduct(_record.normal) > 0);
	}

private:
	
};

class Dielectric : public Material {
public:
	double refractionIndex;

private:
	

public:
	Dielectric(double _refractionIndex) : refractionIndex(_refractionIndex) {

	}

	virtual bool Scatter(const Ray& _ray_in, const Hit_Record& _record, Colour& _attenuation, Ray& _scattered) const override {

		_attenuation = Colour(1.0, 1.0, 1.0);
		double refractionRatio = _record.front_face ? (1.0 / refractionIndex) : refractionIndex;

		Vec3f unitDirection = _ray_in.GetDirection().normalize();
		double cosTheta = fmin(-unitDirection.dotProduct(_record.normal), 1.0);
		double sinTheta = sqrt(1.0 - cosTheta * cosTheta);

		bool cannotRefract = refractionRatio * sinTheta > 1.0;
		Vec3f newDirection;

		if (cannotRefract || Reflectance(cosTheta, refractionRatio) > Utils::random_double()) {
			newDirection = Reflect(unitDirection, _record.normal);
		}
		else {
			newDirection = Refract(unitDirection, _record.normal, refractionRatio);
		}

		_scattered = Ray(_record.point, newDirection);
		
		return true;
	}

private:
	static double Reflectance(double _cosine, double _indexRef) {

		auto r = (1 - _indexRef) / (1 + _indexRef);
		r = r * r;

		return r + (1 - r) * pow((1 - _cosine), 5);
	}

};

class Diffuse_Light : public Material {
public:
	std::shared_ptr<Colour> emit;

private:


public:
	Diffuse_Light() {

	}

	Diffuse_Light(Colour _colour) 
	: emit(std::make_shared<Colour>(_colour)) {

	}

	virtual bool Scatter(const Ray& _ray_in, const Hit_Record& _record, Colour& _attenuation, Ray& _scattered) const override {
		return false;
	}

	virtual Colour emitted() const override {
		return *emit;
	}

private:


};