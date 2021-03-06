#include "IO.h"

bool CachedMesh::IsValid() const{
	return NULL != d3dMesh && NULL != materials;
}
CachedMesh::~CachedMesh()
{
	delete [] materials;
	d3dMesh->Release();
}

IO* IO::g_IO = NULL;

IDirect3DTexture9* IO::_LoadTexture(const char* file)
{
	if (!file) return (IDirect3DTexture9*)-1;
	PDIRECT3DTEXTURE9 t = NULL;
	HRESULT r = D3DXCreateTextureFromFileA(
		d3dd,
		file,
		&t
	);
	if (FAILED(r)) return (IDirect3DTexture9*)-1;
	return t;
}



bool IO::_LoadMesh(const char* file, CachedMesh& mesh)
{
	LPD3DXBUFFER materialsbuffer;
	
	HRESULT r = D3DXLoadMeshFromX(
		file, 
		D3DXMESH_MANAGED,
		d3dd,
		NULL,
		&materialsbuffer, 
		NULL, 
		(DWORD*)&mesh.nMaterials,
		&mesh.d3dMesh
	);
	//use exceptions!
	if (!SUCCEEDED(r))
		return false;

	D3DXMATERIAL* materialBufferPointer = ( D3DXMATERIAL* )materialsbuffer->GetBufferPointer();
	
	mesh.materials = new D3DMATERIAL9[mesh.nMaterials];
	mesh.texturePatterns.resize(mesh.nMaterials);
	for(DWORD i=0; i<mesh.nMaterials; i++)
    {
		// Copy the material
		mesh.materials[i]		= materialBufferPointer[i].MatD3D;
		const char* c = materialBufferPointer[i].pTextureFilename;
		mesh.texturePatterns[i]	= NULL != c ? c : "";
        
		// Set the ambient color for the material (D3DX does not do this)
		mesh.materials[i].Ambient = mesh.materials[i].Diffuse;
    }
    mesh.localBounds.recompute(*mesh.d3dMesh);
	//mesh.Init(d3dMesh,nMaterials,materials,textures);
    // Done with the material buffer
    materialsbuffer->Release();
	return true;
}


void IO::_FreeTexture(IDirect3DTexture9* t)
{
	if (t)	t->Release();
}

