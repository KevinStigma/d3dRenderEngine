#ifndef GLOBALSYS_H
#define GLOBALSYS_H
#include "tiny_obj_loader.h"
#include "foundation.h"

class GlobalSys
{
public:
	GlobalSys();
	~GlobalSys();
	void reconNormals(tinyobj::shape_t& shape);
	bool loadObj(const std::string& filename);
	void genNormals();
	std::vector<tinyobj::shape_t> objects;
};

extern GlobalSys *g_pGlobalSys;
#endif