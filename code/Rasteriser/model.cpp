#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include "model.h"

Model::Model(const char *filename) : verts_(), faces_(), vts_() {
    std::ifstream in;
    in.open (filename, std::ifstream::in);
    if (in.fail()) return;
    std::string line;
    while (!in.eof()) {
        std::getline(in, line);
        std::istringstream iss(line.c_str());
        char trash;
        if (!line.compare(0, 2, "v ")) {       // read 2 characters and check the line starts with "v"
            iss >> trash;
            Vec3f v;
            for (int i=0;i<3;i++) iss >> v[i];
            verts_.push_back(v);
        }
        else if (!line.compare(0, 3, "vt ")) { // read 3 characters and check the line starts with "vt "
            iss >> trash;
            iss >> trash;
            Vec2f vt;
            for (int i=0; i<2; i++) iss >> vt[i]; 
            vts_.push_back(vt);
        }
        else if (!line.compare(0, 2, "f ")) { // f v1/vt1/vn1 v2/vt2/vn2 v3/vt3/vn3 ... making assumption v1==vt1 etc.
            std::vector<int> f;
            std::vector<int> vns;
            std::vector<int> uvs;

            int itrash, v_idx, vt_idx, vn_idx;

            iss >> trash;
            while (iss >> v_idx >> trash >> vt_idx >> trash >> vn_idx) { // read in v_i to idx and discard /vt_i/vn_i
                v_idx--; vt_idx--; vn_idx--;// in wavefront obj all indices start at 1, not zero, we need them to start at zero
                f.push_back(v_idx);
                vns.push_back(vn_idx);
                uvs.push_back(vt_idx);

            }
            faces_.push_back(f);
            vnorms_.push_back(vns);
            uvs_.push_back(uvs);

        }
    }
    std::cerr << "# v# " << verts_.size() << " f# "  << faces_.size() << std::endl;
}

Model::~Model() {
}

int Model::nverts() {
    return (int)verts_.size();
}

int Model::nfaces() {
    return (int)faces_.size();
}

std::vector<int> Model::face(int idx) {
    return faces_[idx];
}

std::vector<int> Model::vertex_normal(int idx) {
    return vnorms_[idx];
}

std::vector<int> Model::uv_coord(int idx) {
    return uvs_[idx];
}

Vec3f Model::vert(int i) {
    return verts_[i];
}

Vec2f Model::vt(int i) {
    return vts_[i];
}

