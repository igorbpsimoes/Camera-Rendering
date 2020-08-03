#include <vector>
#include <stdio.h>
#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <cstring>
#include "vec3.h"
#include "vec2.h"
#include "matrix44.h"
#include <algorithm>
#include "lodepng.h"
#include <cfloat>

#define min_x 0
#define max_x 1
#define min_y 2
#define max_y 3
#define min_z 4
#define max_z 5

#define M_PI 3.141592653589793

//Rotações Horárias
const matrix44 rotateObjAxisX(1.f, 0.f, 0.f, 0.f,
			0.f, 0.9998477, 0.01745241, 0.f,
			0.f, -0.01745241, 0.9998477, 0.f,
			0.f, 0.f, 0.f, 1.f);

const matrix44 rotateObjAxisY(0.9998477, 0.f, -0.01745241, 0.f,
			0.f, 1.f, 0.f, 0.f,
			0.01745241, 0.f, 0.9998477, 0.f,
			0.f, 0.f, 0.f, 1.f);

const matrix44 rotateObjAxisZ(0.9998477, 0.01745241, 0.f, 0.f,
			-0.01745241, 0.9998477, 0.f, 0.f,
			0.f, 0.f, 1, 0.f,
			0.f, 0.f, 0.f, 1.f);
//Rotações Anti-Horárias
const matrix44 rotateObjAxisXCounter(1.f, 0.f, 0.f, 0.f,
			0.f, 0.9998477, -0.01745241, 0.f,
			0.f, 0.01745241, 0.9998477, 0.f,
			0.f, 0.f, 0.f, 1.f);

const matrix44 rotateObjAxisYCounter(0.9998477, 0.f, 0.01745241, 0.f,
			0.f, 1.f, 0.f, 0.f,
			-0.01745241, 0.f, 0.9998477, 0.f,
			0.f, 0.f, 0.f, 1.f);

const matrix44 rotateObjAxisZCounter(0.9998477, -0.01745241, 0.f, 0.f,
			0.01745241, 0.9998477, 0.f, 0.f,
			0.f, 0.f, 1, 0.f,
			0.f, 0.f, 0.f, 1.f);

struct Vertex {
	vec3 pos;
	vec2 texture;
	vec3 normal;
};

struct Triangle {

	Vertex vertex[3];

	Triangle( const std::vector<vec3> v , const std::vector<vec2> texture, const std::vector<vec3> normal)
	{
		vertex[0].pos = v[0];
		vertex[1].pos = v[1];
		vertex[2].pos = v[2];

		vertex[0].texture = texture[0];
		vertex[1].texture = texture[1];
		vertex[2].texture = texture[2];

		vertex[0].normal = normal[0];
		vertex[1].normal = normal[1];
		vertex[2].normal = normal[2];
	}

	~Triangle(){}
};

class Mesh 
{
public:
	std::vector<Triangle> tris;
	vec3 min_v = vec3(FLT_MAX, FLT_MAX, FLT_MAX);
	vec3 max_v = vec3(-FLT_MAX, -FLT_MAX, -FLT_MAX);

	Mesh() {}
	~Mesh() {}

	bool load_mesh_from_file(const char* path) 
	{
		tris.clear();
		std::vector< unsigned int > vertexIndices, uvIndices, normalIndices;
		std::vector< vec3 > temp_vertices;
		std::vector< vec2 > temp_uvs;
		std::vector< vec3 > temp_normals;

		std::ifstream f(path);
		if (!f.is_open())
		{
			std::cout << "File cannot be oppened or does not exist\n";
			return false;
		}

		std::cout << "file was  oppened!\n";

		
		while (!f.eof())
		{
			char line[256];
			f.getline(line, 256);

			std::stringstream s;
			s << line;

			char junk;

			if ( line[0] == 'v')
			{
				if (line[1] == 't') 
				{
					vec2 uv;
					s >> junk >> junk >> uv[0] >> uv[1];
					temp_uvs.push_back(uv);
				}
				else if (line[1] == 'n') 
				{
					vec3 normal;
					s >> junk >> junk >> normal[0] >> normal[1] >> normal[2];
					temp_normals.push_back(normal);
				}
				else {
					vec3 vertex;
					s >> junk >> vertex[0] >> vertex[1] >> vertex[2];
					temp_vertices.push_back(vertex);
				}
			}

			else if ( line[0] == 'f')
			{
				std::string vertex1, vertex2, vertex3;
				unsigned int vertexIndex[3], uvIndex[3], normalIndex[3];

				s >> junk >> vertex1 >> vertex2 >> vertex3;

				//first vertex
				int fstslash = vertex1.find("/");
				std::string fst = vertex1.substr(0, fstslash);
				vertexIndex[0] = atoi( fst.c_str() );

				int nextfstslash = vertex1.find("/", fstslash + 1);
				fst = vertex1.substr(fstslash + 1, nextfstslash);
				uvIndex[0] = atoi(fst.c_str());

				fst = vertex1.substr(nextfstslash + 1, vertex1.size());
				normalIndex[0] = atoi(fst.c_str());

				//second vertex
				fstslash = vertex2.find("/");
				fst = vertex2.substr(0, fstslash);
				vertexIndex[1] = atoi(fst.c_str());

				nextfstslash = vertex2.find("/", fstslash + 1);
				fst = vertex2.substr(fstslash + 1, nextfstslash);
				uvIndex[1] = atoi(fst.c_str());

				fst = vertex2.substr(nextfstslash + 1, vertex2.size());
				normalIndex[1] = atoi(fst.c_str());

				//third vertex
				fstslash = vertex3.find("/");
				fst = vertex3.substr(0, fstslash);
				vertexIndex[2] = atoi(fst.c_str());

				nextfstslash = vertex3.find("/", fstslash + 1);
				fst = vertex3.substr(fstslash + 1, nextfstslash);
				uvIndex[2] = atoi(fst.c_str());

				fst = vertex3.substr(nextfstslash + 1, vertex3.size());
				normalIndex[2] = atoi(fst.c_str());


				vertexIndices.push_back(vertexIndex[0]);
				vertexIndices.push_back(vertexIndex[1]);
				vertexIndices.push_back(vertexIndex[2]);

				uvIndices.push_back(uvIndex[0]);
				uvIndices.push_back(uvIndex[1]);
				uvIndices.push_back(uvIndex[2]);

				normalIndices.push_back(normalIndex[0]);
				normalIndices.push_back(normalIndex[1]);
				normalIndices.push_back(normalIndex[2]);
			}
		}

		for (unsigned int i = 0; i < vertexIndices.size(); i+=3)
		{
			unsigned int v1 = vertexIndices[i];
			unsigned int v2 = vertexIndices[i+1];
			unsigned int v3 = vertexIndices[i+2];

			unsigned int vt1 = uvIndices[i];
			unsigned int vt2 = uvIndices[i + 1];
			unsigned int vt3 = uvIndices[i + 2];

			unsigned int vn1 = normalIndices[i];
			unsigned int vn2 = normalIndices[i + 1];
			unsigned int vn3 = normalIndices[i + 2];

			std::vector<vec3> vertices, normals;
			std::vector<vec2> textures;

			vertices.push_back(temp_vertices[v1 - 1]);
			vertices.push_back(temp_vertices[v2 - 1]);
			vertices.push_back(temp_vertices[v3 - 1]);

			textures.push_back(temp_uvs[vt1 - 1]);
			textures.push_back(temp_uvs[vt2 - 1]);
			textures.push_back(temp_uvs[vt3 - 1]);

			normals.push_back(temp_normals[vn1 - 1]);
			normals.push_back(temp_normals[vn2 - 1]);
			normals.push_back(temp_normals[vn3 - 1]);

			Triangle t(vertices, textures, normals);
			//bounding box
			min_v[0] = std::min(min_v[0], vertices[0][0]);
			min_v[0] = std::min(min_v[0], vertices[1][0]);
			min_v[0] = std::min(min_v[0], vertices[2][0]);
			min_v[1] = std::min(min_v[1], vertices[0][1]);
			min_v[1] = std::min(min_v[1], vertices[1][1]);
			min_v[1] = std::min(min_v[1], vertices[2][1]);
			min_v[2] = std::min(min_v[2], vertices[0][2]);
			min_v[2] = std::min(min_v[2], vertices[1][2]);
			min_v[2] = std::min(min_v[2], vertices[2][2]);

			max_v[0] = std::max(max_v[0], vertices[0][0]);
			max_v[0] = std::max(max_v[0], vertices[1][0]);
			max_v[0] = std::max(max_v[0], vertices[2][0]);
			max_v[1] = std::max(max_v[1], vertices[0][1]);
			max_v[1] = std::max(max_v[1], vertices[1][1]);
			max_v[1] = std::max(max_v[1], vertices[2][1]);
			max_v[2] = std::max(max_v[2], vertices[0][2]);
			max_v[2] = std::max(max_v[2], vertices[1][2]);
			max_v[2] = std::max(max_v[2], vertices[2][2]);
			
			tris.push_back(t);
		}

		std::cout << "vertSize = " << vertexIndices.size() << "\n";
		return true;
	}
};

class Obj
{
public:
	Mesh mesh;
	std::vector<vec3> texture_buffer;
	int texture_width, texture_height;
	vec3 objCenter;
	matrix44 toOrigin, fromOrigin;

	void decodeOneStep(const char* file_path) {
		std::vector<unsigned char> png;
		std::vector<unsigned char> image; //the raw pixels
		unsigned width, height;
		lodepng::State state; //optionally customize this one

		unsigned error = lodepng::load_file(png, file_path); //load the image file with given file_path
		if (!error) error = lodepng::decode(image, width, height, state, png);

		//if there's an error, display it
		if (error) std::cout << "decoder error " << error << ": " << lodepng_error_text(error) << std::endl;

		//the pixels are now in the vector "image", 4 bytes per pixel, ordered RGBARGBA..., use it as texture, draw it, ...
		texture_buffer.reserve((int)width * (int)height);
		for (int i = 0; i < image.size(); i += 4)
			texture_buffer.push_back(vec3(float(image[i]), float(image[i + 1]), float(image[i + 2])));

		texture_width = (int)width;
		texture_height = (int)height;
	}

	Obj() {}
	Obj(const char* file_path) {
		mesh.load_mesh_from_file(file_path);
		objCenter = vec3((mesh.max_v[0] - mesh.min_v[0])/2, (mesh.max_v[1] - mesh.min_v[1])/2, (mesh.max_v[2] - mesh.min_v[2])/2);
	}
	Obj(const char* file_path, const char* texture_path) {
		mesh.load_mesh_from_file(file_path);
		decodeOneStep(texture_path);
		objCenter = vec3((mesh.max_v[0] - mesh.min_v[0])/2, (mesh.max_v[1] - mesh.min_v[1])/2, (mesh.max_v[2] - mesh.min_v[2])/2);
		toOrigin = matrix44(1.f, 0.f, 0.f, 0.f,
			0.f, 1.f, 0.f, 0.f,
			0.f, 0.f, 1.f, 0.f,
			-objCenter[0], -objCenter[1], -objCenter[2], 1.f);
		fromOrigin = matrix44(1.f, 0.f, 0.f, 0.f,
			0.f, 1.f, 0.f, 0.f,
			0.f, 0.f, 1.f, 0.f,
			objCenter[0], objCenter[1], objCenter[2], 1.f);
	}

	~Obj() {}

	void rotateXClockwise() {
		for(int i = 0; i < mesh.tris.size(); ++i) {
			toOrigin.mult_point_matrix(mesh.tris[i].vertex[0].pos, mesh.tris[i].vertex[0].pos);
			toOrigin.mult_point_matrix(mesh.tris[i].vertex[1].pos, mesh.tris[i].vertex[1].pos);
			toOrigin.mult_point_matrix(mesh.tris[i].vertex[2].pos, mesh.tris[i].vertex[2].pos);

			rotateObjAxisX.mult_point_matrix(mesh.tris[i].vertex[0].pos, mesh.tris[i].vertex[0].pos);
			rotateObjAxisX.mult_point_matrix(mesh.tris[i].vertex[1].pos, mesh.tris[i].vertex[1].pos);
			rotateObjAxisX.mult_point_matrix(mesh.tris[i].vertex[2].pos, mesh.tris[i].vertex[2].pos);
			
			fromOrigin.mult_point_matrix(mesh.tris[i].vertex[0].pos, mesh.tris[i].vertex[0].pos);
			fromOrigin.mult_point_matrix(mesh.tris[i].vertex[1].pos, mesh.tris[i].vertex[1].pos);
			fromOrigin.mult_point_matrix(mesh.tris[i].vertex[2].pos, mesh.tris[i].vertex[2].pos);
		}
	}

	void rotateXCounterClockwise() {
		for(int i = 0; i < mesh.tris.size(); ++i) {
			toOrigin.mult_point_matrix(mesh.tris[i].vertex[0].pos, mesh.tris[i].vertex[0].pos);
			toOrigin.mult_point_matrix(mesh.tris[i].vertex[1].pos, mesh.tris[i].vertex[1].pos);
			toOrigin.mult_point_matrix(mesh.tris[i].vertex[2].pos, mesh.tris[i].vertex[2].pos);

			rotateObjAxisXCounter.mult_point_matrix(mesh.tris[i].vertex[0].pos, mesh.tris[i].vertex[0].pos);
			rotateObjAxisXCounter.mult_point_matrix(mesh.tris[i].vertex[1].pos, mesh.tris[i].vertex[1].pos);
			rotateObjAxisXCounter.mult_point_matrix(mesh.tris[i].vertex[2].pos, mesh.tris[i].vertex[2].pos);
			
			fromOrigin.mult_point_matrix(mesh.tris[i].vertex[0].pos, mesh.tris[i].vertex[0].pos);
			fromOrigin.mult_point_matrix(mesh.tris[i].vertex[1].pos, mesh.tris[i].vertex[1].pos);
			fromOrigin.mult_point_matrix(mesh.tris[i].vertex[2].pos, mesh.tris[i].vertex[2].pos);
		}
	}

	void rotateYClockwise() {
		for(int i = 0; i < mesh.tris.size(); ++i) {
			toOrigin.mult_point_matrix(mesh.tris[i].vertex[0].pos, mesh.tris[i].vertex[0].pos);
			toOrigin.mult_point_matrix(mesh.tris[i].vertex[1].pos, mesh.tris[i].vertex[1].pos);
			toOrigin.mult_point_matrix(mesh.tris[i].vertex[2].pos, mesh.tris[i].vertex[2].pos);

			rotateObjAxisY.mult_point_matrix(mesh.tris[i].vertex[0].pos, mesh.tris[i].vertex[0].pos);
			rotateObjAxisY.mult_point_matrix(mesh.tris[i].vertex[1].pos, mesh.tris[i].vertex[1].pos);
			rotateObjAxisY.mult_point_matrix(mesh.tris[i].vertex[2].pos, mesh.tris[i].vertex[2].pos);
			
			fromOrigin.mult_point_matrix(mesh.tris[i].vertex[0].pos, mesh.tris[i].vertex[0].pos);
			fromOrigin.mult_point_matrix(mesh.tris[i].vertex[1].pos, mesh.tris[i].vertex[1].pos);
			fromOrigin.mult_point_matrix(mesh.tris[i].vertex[2].pos, mesh.tris[i].vertex[2].pos);
		}
	}

	void rotateYCounterClockwise() {
		for(int i = 0; i < mesh.tris.size(); ++i) {
			toOrigin.mult_point_matrix(mesh.tris[i].vertex[0].pos, mesh.tris[i].vertex[0].pos);
			toOrigin.mult_point_matrix(mesh.tris[i].vertex[1].pos, mesh.tris[i].vertex[1].pos);
			toOrigin.mult_point_matrix(mesh.tris[i].vertex[2].pos, mesh.tris[i].vertex[2].pos);

			rotateObjAxisYCounter.mult_point_matrix(mesh.tris[i].vertex[0].pos, mesh.tris[i].vertex[0].pos);
			rotateObjAxisYCounter.mult_point_matrix(mesh.tris[i].vertex[1].pos, mesh.tris[i].vertex[1].pos);
			rotateObjAxisYCounter.mult_point_matrix(mesh.tris[i].vertex[2].pos, mesh.tris[i].vertex[2].pos);
			
			fromOrigin.mult_point_matrix(mesh.tris[i].vertex[0].pos, mesh.tris[i].vertex[0].pos);
			fromOrigin.mult_point_matrix(mesh.tris[i].vertex[1].pos, mesh.tris[i].vertex[1].pos);
			fromOrigin.mult_point_matrix(mesh.tris[i].vertex[2].pos, mesh.tris[i].vertex[2].pos);
		}
	}

	void rotateZClockwise() {
		for(int i = 0; i < mesh.tris.size(); ++i) {
			toOrigin.mult_point_matrix(mesh.tris[i].vertex[0].pos, mesh.tris[i].vertex[0].pos);
			toOrigin.mult_point_matrix(mesh.tris[i].vertex[1].pos, mesh.tris[i].vertex[1].pos);
			toOrigin.mult_point_matrix(mesh.tris[i].vertex[2].pos, mesh.tris[i].vertex[2].pos);

			rotateObjAxisZ.mult_point_matrix(mesh.tris[i].vertex[0].pos, mesh.tris[i].vertex[0].pos);
			rotateObjAxisZ.mult_point_matrix(mesh.tris[i].vertex[1].pos, mesh.tris[i].vertex[1].pos);
			rotateObjAxisZ.mult_point_matrix(mesh.tris[i].vertex[2].pos, mesh.tris[i].vertex[2].pos);
			
			fromOrigin.mult_point_matrix(mesh.tris[i].vertex[0].pos, mesh.tris[i].vertex[0].pos);
			fromOrigin.mult_point_matrix(mesh.tris[i].vertex[1].pos, mesh.tris[i].vertex[1].pos);
			fromOrigin.mult_point_matrix(mesh.tris[i].vertex[2].pos, mesh.tris[i].vertex[2].pos);
		}
	}

	void rotateZCounterClockwise() {
		for(int i = 0; i < mesh.tris.size(); ++i) {
			toOrigin.mult_point_matrix(mesh.tris[i].vertex[0].pos, mesh.tris[i].vertex[0].pos);
			toOrigin.mult_point_matrix(mesh.tris[i].vertex[1].pos, mesh.tris[i].vertex[1].pos);
			toOrigin.mult_point_matrix(mesh.tris[i].vertex[2].pos, mesh.tris[i].vertex[2].pos);

			rotateObjAxisZCounter.mult_point_matrix(mesh.tris[i].vertex[0].pos, mesh.tris[i].vertex[0].pos);
			rotateObjAxisZCounter.mult_point_matrix(mesh.tris[i].vertex[1].pos, mesh.tris[i].vertex[1].pos);
			rotateObjAxisZCounter.mult_point_matrix(mesh.tris[i].vertex[2].pos, mesh.tris[i].vertex[2].pos);
			
			fromOrigin.mult_point_matrix(mesh.tris[i].vertex[0].pos, mesh.tris[i].vertex[0].pos);
			fromOrigin.mult_point_matrix(mesh.tris[i].vertex[1].pos, mesh.tris[i].vertex[1].pos);
			fromOrigin.mult_point_matrix(mesh.tris[i].vertex[2].pos, mesh.tris[i].vertex[2].pos);
		}
	}
	
};
