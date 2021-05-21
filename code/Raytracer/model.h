#pragma once

#include <vector>
#include "geometry.h"

class Model {
private:
	std::vector<Vec3f> verts_;
	std::vector<Vec3f> vns_;

	std::vector<std::vector<int> > faces_;
	std::vector<std::vector<int> > vnorms_;
	std::vector<std::vector<int> > uvs_;

public:
	Model(const char *filename);
	~Model();
	int nverts();
	int nfaces();
	int nnorms();

	Vec3f vert(int i);
	Vec3f vn(int i);

	std::vector<int> face(int idx);
	std::vector<int> vertex_normal(int idx);
};

