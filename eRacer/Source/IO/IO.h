#ifndef IO_H_
#define IO_H_

#include "..\Graphics\StaticGeometry.h"

class IO 
{
	IO* g_IO;
	LPDIRECT3DDEVICE9 d3dd;
  public:
	IO(LPDIRECT3DDEVICE9 d) { g_IO = this; d3dd = d; }
	// TODO this should return a tuple (mesh, materials, textures)
	virtual int LoadMesh(Graphics::StaticGeometry &geom, const char* file);
	virtual LPDIRECT3DTEXTURE9 LoadTexture(const char* file);
};



#endif
