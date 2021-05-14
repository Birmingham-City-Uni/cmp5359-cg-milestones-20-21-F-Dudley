#pragma once

#include <algorithm>

#include "geometry.h"
#include "Utils.h"
#include "Ray.h"
#include "Collideable.h"
#include "Collideable_List.h"

inline bool Box_Compare(const std::shared_ptr<Collideable> _a, const std::shared_ptr<Collideable> _b, int _axis) {
	AABB boxA, boxB;

	if (!_a->Bounding_Box(boxA) || !_b->Bounding_Box(boxB)) {
		std::cerr << "No Bounding Box Found in BVH_Node Constructor." << std::endl;
	}

	return boxA.min()[_axis] < boxB.min()[_axis];
}

bool Box_Compare_X(const std::shared_ptr<Collideable> _a, const std::shared_ptr<Collideable> _b) {
	return Box_Compare(_a, _b, 0);
}

bool Box_Compare_Y(const std::shared_ptr<Collideable> _a, const std::shared_ptr<Collideable> _b) {
	return Box_Compare(_a, _b, 1);
}

bool Box_Compare_Z(const std::shared_ptr<Collideable> _a, const std::shared_ptr<Collideable> _b) {
	return Box_Compare(_a, _b, 2);
}

class BVH_Node : public Collideable {
public:
	std::shared_ptr<Collideable> left;
	std::shared_ptr<Collideable> right;
	AABB box;

private:


public:
	BVH_Node() {

	}

	BVH_Node(const Collideable_List& _world)
		: BVH_Node(_world.objects, 0, _world.objects.size()) {

	}

	BVH_Node(const std::vector<std::shared_ptr<Collideable>>& _objectsSrc, size_t _start, size_t _end) {
		auto objects = _objectsSrc;

		int axis = Utils::random_int(0, 2);

		auto comparator = (axis == 0) ? Box_Compare_X
			: (axis == 1) ? Box_Compare_Y
			: Box_Compare_Z;

		size_t objectSpan = _end - _start;

		if (objectSpan == 1) {
			left = right = objects[_start];
		}
		else if (objectSpan == 2) {

			if (comparator(objects[_start], objects[_start + 1])) {
				left = objects[_start];
				right = objects[_start + 1];
			}
			else {
				left = objects[_start + 1];
				right = objects[_start];
			}

		}
		else {
			std::sort(objects.begin() + _start, objects.begin() + _end, comparator);

			auto mid = _start + objectSpan / 2;
			left = std::make_shared<BVH_Node>(objects, _start, mid);
			right = std::make_shared<BVH_Node>(objects, mid, _end);
		}

		AABB boxLeft, boxRight;

		if (!left->Bounding_Box(boxLeft) || !right->Bounding_Box(boxRight)) {
			std::cerr << "No Bounding Box in BVH_Node Constructor." << std::endl;
		}

		box = surrounding_box(boxLeft, boxRight);
	}

	virtual bool Hit(const Ray& _ray, double _t_min, double _t_max, Hit_Record& _record) const override {

		if (!box.Hit(_ray, _t_min, _t_max)) return false;
		
		bool hit_left = left->Hit(_ray, _t_min, _t_max, _record);
		bool hit_right = right->Hit(_ray, _t_min, hit_left ? _record.t : _t_max, _record);

		return hit_left || hit_right;
	}

	virtual bool Bounding_Box(AABB& _outputBox) const override {
		_outputBox = box;

		return true;
	}

private:


};

