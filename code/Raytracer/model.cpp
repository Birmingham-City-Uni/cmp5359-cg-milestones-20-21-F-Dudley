#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include "model.h"

Model::Model(const char *filename) : verts_(), faces_() {
    std::ifstream in;
    in.open (filename, std::ifstream::in);
    if (in.fail()) return;
    std::string line;
    while (!in.eof()) {
        std::getline(in, line);
        std::istringstream iss(line.c_str());
        char trash;
        if (!line.compare(0, 2, "v ")) {
            iss >> trash;
            Vec3f v;
            for (int i=0;i<3;i++) iss >> v[i];
            verts_.push_back(v);
        }
        else if (!line.compare(0, 3, "vn ")) {
            iss >> trash;
            Vec3f vn;
            for (int i = 0; i < 3; i++) iss >> vn[i];
            vns_.push_back(vn);
        }
        else if (!line.compare(0, 2, "f ")) { // f v1/vt1/vn1 v2/vt2/vn2 v3/vt3/vn3 
            std::vector<int> f;
            std::vector<int> vns;
            std::vector<int> uvs;

            int itrash, v_idx, vt_idx, vn_idx;
            iss >> trash;

            while (iss >> v_idx >> trash >> vt_idx >> trash >> vn_idx) {
                v_idx--, vt_idx--, vn_idx--; // in wavefront obj all indices start at 1, not zero
                f.push_back(v_idx);
                vns.push_back(vn_idx);
                uvs.push_back(vt_idx);
            }

            faces_.push_back(f);
            vnorms_.push_back(vns);
            uvs_.push_back(uvs);
        }
    }
    std::cerr << "# vertex# " << verts_.size() << " face# "  << faces_.size() << std::endl;
}

Model::~Model() {
}

int Model::nverts() {
    return (int)verts_.size();
}

int Model::nfaces() {
    return (int)faces_.size();
}

int Model::nnorms() {
    return (int)vnorms_.size();
}

std::vector<int> Model::face(int idx) {
    return faces_[idx];
}

std::vector<int> Model::vertex_normal(int idx) {
    return vnorms_[idx];
}

Vec3f Model::vert(int i) {
    return verts_[i];
}

Vec3f Model::vn(int i) {
    return vns_[i];
}
