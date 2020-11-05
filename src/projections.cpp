#include "Projections.h"

#define _USE_MATH_DEFINES
#include <math.h>

#include <iostream>
#include <linalg.h>
using namespace linalg::aliases;
using namespace linalg::ostream_overloads;

#define TINYOBJLOADER_IMPLEMENTATION // define this in only *one* .cc
#include "tiny_obj_loader.h"


cg::ObjParser::ObjParser(std::string filename) :
	filename(filename)
{
	faces.clear();
}


cg::ObjParser::~ObjParser()
{
	faces.clear();
}

void cg::ObjParser::Parse()
{
	tinyobj::attrib_t attrib;
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;

	std::string warn;
	std::string err;

	bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, filename.string().c_str());

	if (!warn.empty()) {
		std::cout << warn << std::endl;
	}

	if (!err.empty()) {
		std::cerr << err << std::endl;
	}

	if (!ret) {
		throw std::runtime_error("Can't parse OBJ file");
	}

	// Loop over shapes
	for (size_t s = 0; s < shapes.size(); s++) {
		// Loop over faces(polygon)
		size_t index_offset = 0;
		for (size_t f = 0; f < shapes[s].mesh.num_face_vertices.size(); f++) {
			int fv = shapes[s].mesh.num_face_vertices[f];
			face current_face;

			// Loop over vertices in the face.
			for (size_t v = 0; v < fv; v++) {
				// access to vertex
				tinyobj::index_t idx = shapes[s].mesh.indices[index_offset + v];
				tinyobj::real_t vx = attrib.vertices[3*idx.vertex_index+0];
				tinyobj::real_t vy = attrib.vertices[3*idx.vertex_index+1];
				tinyobj::real_t vz = attrib.vertices[3*idx.vertex_index+2];
				current_face.vertexes[v] = float4(vx, vy, vz, 1.f);
			}
			index_offset += fv;
			faces.push_back(current_face);
		}
	}
}

const std::vector<cg::face>& cg::ObjParser::GetFaces()
{
	return faces;
}



cg::Projections::Projections(unsigned short width, unsigned short height, std::string obj_file) : cg::LineDrawing(width, height)
{
	parser = new ObjParser(obj_file);
	parser->Parse();

	cb = {};
	cb.World = float4x4 { {1, 0, 0, 0}, {0, 1, 0, 0}, {0, 0, 1, 0}, {0, -1, -3, 1} };

	float3 eye {0, 0, 1};
	float3 at {0, 0, 0};
	float3 up {0, 1, 0};

	float3 zaxis = normalize(at - eye);
	float3 xaxis = normalize(cross(up, zaxis));
	float3 yaxis = cross(zaxis, xaxis);

	cb.View = float4x4 { 
		{xaxis.x, xaxis.y, xaxis.z, -dot(xaxis, eye)},
		{yaxis.x, yaxis.y, yaxis.z, -dot(yaxis, eye)},
		{zaxis.x, zaxis.y, zaxis.z, -dot(zaxis, eye)},
		{0, 0, 0, 1} 
	};

	float z_near = 1.f;
	float z_far = 10.f;
	float near_height = (float)1;
	float near_width = (float)1;

	cb.Projection = float4x4 {
		{2 * z_near / near_width, 0, 0, 0},
		{0, 2 * z_near / near_height, 0, 0},
		{0, 0, z_far / (z_far - z_near), z_near * z_far / (z_far - z_near)},
		{0, 0, 1, 0}
	};
}

cg::Projections::~Projections()
{
	delete parser;
}

void cg::Projections::DrawScene()
{
	unsigned id = 0;
	for (auto face : parser->GetFaces()) {
		face.primitive_id = id++;
		for (unsigned i = 0; i < 3; i++) { face.vertexes[i] = VertexShader(face.vertexes[i]); }
		Rasterizer(face);
	}
}

void cg::Projections::Rasterizer(face face) {
	// To screen space
	unsigned x_center = width / 2;
	unsigned y_center = height / 2;
	unsigned scale = std::min(x_center, y_center) - 1;

	// From homogeneous to cartezian
	for (unsigned i = 0; i < 3; i++) {
		face.vertexes[i] /= face.vertexes[i].w;
		face.vertexes[i].x = std::clamp(x_center - scale * face.vertexes[i].x, 0.f, width - 1.f);
		face.vertexes[i].y = std::clamp(y_center - scale * face.vertexes[i].y, 0.f, height - 1.f);
	}

	DrawTriangle(face);
}

void cg::Projections::DrawTriangle(face face) {
	DrawLine(face.vertexes[0].x, face.vertexes[0].y, face.vertexes[1].x, face.vertexes[1].y, color(255, 0, 0));
	DrawLine(face.vertexes[1].x, face.vertexes[1].y, face.vertexes[2].x, face.vertexes[2].y, color(0, 255, 0));
	DrawLine(face.vertexes[2].x, face.vertexes[2].y, face.vertexes[0].x, face.vertexes[0].y, color(0, 0, 255));
}

float4 cg::Projections::VertexShader(float4 vertex) {
	return mul(cb.Projection, mul(cb.View, mul(cb.World, vertex)));
}
