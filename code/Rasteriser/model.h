#pragma once

#include <vector>
#include "geometry.h"

class Model {
private:
	std::vector<Vec3f> verts_;              // Stores Vec3f for every model vertex world position
	std::vector<std::vector<int> > faces_;  // Stores a vector of vector<int> that represent indices in verts_ for vertices comprising a face
	std::vector<Vec2f> vts_;				// Stores Vec3f for every model vertex texture coordinate
public:
	Model(const char *filename);
	~Model();
	int nverts();
	int nfaces();
	Vec3f vert(int i);
	Vec2f vt(int i);
	std::vector<int> face(int idx);
};

