#pragma once

#include <memory>
#include <vector>

#include "Collideable.h"

class Collideable_List : public Collideable {
public:
	std::vector<std::shared_ptr<Collideable>> objects;
private:

public:
	Collideable_List() {

	}

	Collideable_List(std::shared_ptr<Collideable> _object) {
		Add(_object);
	}

	void Add(std::shared_ptr<Collideable> _object) {
		objects.push_back(_object);
	}

	void Clear() {
		objects.clear();
	}

	virtual bool Hit(const Ray& _ray, double _t_min, double _t_max, Hit_Record& _record) const override {
		Hit_Record temp_record;
		bool hit_occured = false;
		auto closest_so_far = _t_max;

		for (const auto& object : objects) {
			if (object->Hit(_ray, _t_min, closest_so_far, temp_record)) {
				hit_occured = true;
				closest_so_far = temp_record.t;
				_record = temp_record;
			}
		}

		return hit_occured;
	}

	inline virtual bool Bounding_Box(AABB& _outputBox) const override {

		if (objects.empty()) return false;

		AABB tmpBox;
		bool firstBox = true;

		for (const auto& object : objects) {

			if (!object->Bounding_Box(tmpBox)) return false;

			_outputBox = firstBox ? tmpBox : surrounding_box(_outputBox, tmpBox);
			firstBox = false;
		}

		return true;
	}

private:

};