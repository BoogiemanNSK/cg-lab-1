#include "triangle_rasterization.h"

#define _USE_MATH_DEFINES
#include <math.h>

#include <iostream>


cg::TriangleRasterization::TriangleRasterization(unsigned short width, unsigned short height, std::string obj_file) : cg::Projections(width, height, obj_file)
{
}

cg::TriangleRasterization::~TriangleRasterization()
{
}

void cg::TriangleRasterization::DrawTriangle(face face)
{
	float x_min = std::min(std::min(face.vertexes[0].x, face.vertexes[1].x), face.vertexes[2].x);
	float x_max = std::max(std::max(face.vertexes[0].x, face.vertexes[1].x), face.vertexes[2].x);
	float y_min = std::min(std::min(face.vertexes[0].y, face.vertexes[1].y), face.vertexes[2].y);
	float y_max = std::max(std::max(face.vertexes[0].y, face.vertexes[1].y), face.vertexes[2].y);

	bool ccw = false;
	float direction = ccw ? -1.f : 1.f;
	float area = direction * EdgeFunction(face.vertexes[0].xy(), face.vertexes[1].xy(), face.vertexes[2].xy());
	
	for (float x = x_min; x <= x_max; x += 1.f) {
		for (float y = y_min; y <= y_max; y += 1.f) {
			float2 point{ x, y };
			float e0 = direction * EdgeFunction(face.vertexes[0].xy(), face.vertexes[1].xy(), point);
			float e1 = direction * EdgeFunction(face.vertexes[1].xy(), face.vertexes[2].xy(), point);
			float e2 = direction * EdgeFunction(face.vertexes[2].xy(), face.vertexes[0].xy(), point);

			if (e0 >= 0.f && e1 >= 0.f && e2 >= 0.f) {
				float3 bary{ e0 / area, e1 / area, e2 / area };
				 SetPixel(round(x), round(y), PixelShader(point, bary, face.primitive_id));
			}
		}
	}
	
	// Wireframe
	DrawLine(face.vertexes[0].x, face.vertexes[0].y, face.vertexes[1].x, face.vertexes[1].y, color(255, 0, 0));
	DrawLine(face.vertexes[1].x, face.vertexes[1].y, face.vertexes[2].x, face.vertexes[2].y, color(0, 255, 0));
	DrawLine(face.vertexes[2].x, face.vertexes[2].y, face.vertexes[0].x, face.vertexes[0].y, color(0, 0, 255));
}

cg::color cg::TriangleRasterization::PixelShader(float2 coordinates, float3 bary, unsigned id) {
	/*switch (id % 6) {
		case 0: return color(200, 0, 0);
		case 1: return color(0, 200, 0);
		case 2: return color(0, 0, 200);
		case 3: return color(200, 0, 200);
		case 4: return color(200, 200, 0);
		case 5: return color(0, 200, 200);
	}
	return color(128, 128, 128);*/
	float3 inter_color = bary * 255;
	return color(round(inter_color.x), round(inter_color.y), round(inter_color.z));
}

float cg::TriangleRasterization::EdgeFunction(float2 a, float2 b, float2 c)
{
	return (c.x - a.x) * (b.y - a.y) - (c.y - a.y) * (b.x - a.x);
}


