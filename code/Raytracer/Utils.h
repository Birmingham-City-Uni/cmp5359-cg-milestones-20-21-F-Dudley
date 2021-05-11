#pragma once

#include <cmath>
#include <limits>
#include <memory>
#include <cstdlib>

namespace Utils {

	// - Constants

	const double infinity = std::numeric_limits<double>::infinity();
	const double pi = 3.1415926;

	// - Functions

	inline double degrees_to_radians(double _degrees) {
		return _degrees * pi / 180.0;
	}

	inline double random_double() {
		return rand() / (RAND_MAX + 1.0);
	}

	inline double random_double(double _min, double _max) {
		return _min + (_max - _min) * random_double();
	}

}
