#include <vector>
#include <cmath>
#include "tgaimage.h"
#include "model.h"
#include "geometry.h"

const TGAColor white = TGAColor(255, 255, 255, 255);
const TGAColor red = TGAColor(255, 0, 0, 255);
const TGAColor green = TGAColor(0, 255, 0, 255);
const TGAColor blue = TGAColor(0, 0, 255, 255);
const int width = 1080;
const int height = 1080;

//void line(Vec2i p0, Vec2i p1, TGAImage &image, TGAColor color) {
//	bool steep = false;
//	if (std::abs(p0.x - p1.x) < std::abs(p0.y - p1.y)) {
//		std::swap(p0.x, p0.y);
//		std::swap(p1.x, p1.y);
//		steep = true;
//	}
//	if (p0.x > p1.x) {
//		std::swap(p0, p1);
//	}
//
//	for (int x = p0.x; x <= p1.x; x++) {
//		float t = (x - p0.x) / (float)(p1.x - p0.x);
//		int y = p0.y*(1. - t) + p1.y*t;
//		if (steep) {
//			image.set(y, x, color);
//		}
//		else {
//			image.set(x, y, color);
//		}
//	}
//}
//
//void triangle(Vec2i t0, Vec2i t1, Vec2i t2, TGAImage &image, TGAColor color) {
//	
//	if (t0.y > t1.y) std::swap(t0, t1);
//	if (t0.y > t2.y) std::swap(t0, t2);
//	if (t1.y > t2.y) std::swap(t1, t2);
//
//	float total_height = t2.y - t0.y;
//
//	for (float y = t0.y; y == t1.y; y++) {
//		float segment_height = t1.y - t0.y + 1;
//
//		float alpha = (y - t0.y) / total_height;
//		float beta = (y - t0.y) / segment_height;
//
//		Vec2i A = t0 + (t2 - t0) * alpha;
//		Vec2i B = t0 + (t1 - t0) * beta;
//
//		image.set(A.x, y, red);
//		image.set(B.x, y, green);
//	}
//
//	/*line(t0, t1, image, color);
//	line(t1, t2, image, color);
//	line(t2, t0, image, color);*/
//}

void triangle(Vec2i t0, Vec2i t1, Vec2i t2, TGAImage& image, TGAColor color) {

	if (t0.y == t1.y && t0.y == t2.y) return;

	if (t0.y > t1.y) std::swap(t0, t1);
	if (t0.y > t2.y) std::swap(t0, t2);
	if (t1.y > t2.y) std::swap(t1, t2);

	float total_height = t2.y - t0.y;

	for (float y = t0.y; y == t1.y; y++) {
		float segment_height = t1.y - t0.y + 1;

		float alpha = (y - t0.y) / total_height;
		float beta = (y - t0.y) / segment_height;

		Vec2i A = t0 + (t2 - t0) * alpha;
		Vec2i B = t0 + (t1 - t0) * beta;

		image.set(A.x, y, red);
		image.set(B.x, y, green);
	}

	for (float y = t1.y; y == t2.y; y++) {
		float segment_height = t2.y - t1.y + 1;

		float alpha = (y - t0.y) / total_height;
		float beta = (y - t1.y) / segment_height;

		Vec2i A = t0 + (t2 - t0) * alpha;
		Vec2i B = t1 + (t2 - t1) * beta;

		image.set(A.x, y, red);
		image.set(B.x, y, green);
	}
}

int main(int argc, char** argv) {
	TGAImage image(width, height, TGAImage::RGB);

	Vec2i t0[3] = { Vec2i(650, 1000),   Vec2i(530, 1066),  Vec2i(968, 901) };
	Vec2i t1[3] = { Vec2i(90, 680),  Vec2i(42, 750),   Vec2i(895, 627) };
	Vec2i t2[3] = { Vec2i(360, 380), Vec2i(1050, 160), Vec2i(1079, 256) };

	triangle(t0[0], t0[1], t0[2], image, red);
	triangle(t1[0], t1[1], t1[2], image, white);
	triangle(t2[0], t2[1], t2[2], image, green);

	image.flip_vertically(); // Origin at the left bottom corner of the image
	image.write_tga_file("output.tga");
	return 0;
}

