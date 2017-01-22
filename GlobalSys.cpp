#define TINYOBJLOADER_IMPLEMENTATION
#include "GlobalSys.h"
#include <iostream>
GlobalSys *g_pGlobalSys = NULL;

GlobalSys::GlobalSys()
{

}
GlobalSys::~GlobalSys()
{

}

void GlobalSys::genNormals()
{
	std::vector<tinyobj::shape_t>& objects = g_pGlobalSys->objects;
	for (int i = 0; i<(int)objects.size(); i++)
		reconNormals(objects[i]);
}

bool GlobalSys::loadObj(const std::string& filename)
{
	std::string err;
	std::vector<tinyobj::material_t> mat_list; //we ignore the information of materials now :)

	std::vector<tinyobj::shape_t> obj_list;
	bool ret = tinyobj::LoadObj(obj_list, mat_list, err, filename.c_str());
	if (!err.empty()) {
		std::cerr << err << std::endl;
	}

	if (!ret)
		return false;
	std::vector<tinyobj::shape_t>& objects = g_pGlobalSys->objects;
	objects.insert(objects.end(), obj_list.begin(), obj_list.end());
	std::cout << "load " << obj_list.size() << " shapes successfully!" << std::endl;
	return true;
}

void GlobalSys::reconNormals(tinyobj::shape_t& shape)
{
	int face_num = shape.mesh.indices.size() / 3;
	int vertex_num = shape.mesh.positions.size() / 3;
	std::vector<Vec3> face_normals(face_num);
	std::vector<std::vector<int>> faces_adj_verts(vertex_num);
	std::vector<float>& positions = shape.mesh.positions;
	int index[3];
	Vec3 tri_v[3];
	for (int i = 0; i<face_num; i++)
	{
		for (int j = 0; j<3; j++)
		{
			index[j] = shape.mesh.indices[i * 3 + j];
			tri_v[j] = Vec3(positions[index[j] * 3],
				positions[index[j] * 3 + 1],
				positions[index[j] * 3 + 2]);
			faces_adj_verts[index[j]].push_back(i);
		}
		face_normals[i] = -(tri_v[2] - tri_v[1]).cross(tri_v[1] - tri_v[0]).normalized();
	}

	std::vector<float>& normals = shape.mesh.normals;
	normals.resize(positions.size());
	for (int i = 0; i<(int)faces_adj_verts.size(); i++)
	{
		Vec3 nor(0, 0, 0);
		for (int j = 0; j<(int)faces_adj_verts[i].size(); j++)
		{
			int index = faces_adj_verts[i][j];
			nor += face_normals[index];
		}
		nor /= (int)faces_adj_verts[i].size();
		normals[i * 3] = nor(0);
		normals[i * 3 + 1] = nor(1);
		normals[i * 3 + 2] = nor(2);
	}
}
