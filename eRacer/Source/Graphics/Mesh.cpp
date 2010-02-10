#include "Mesh.h"
#include <cassert>

namespace Graphics {

void Mesh::Draw(IDirect3DDevice9* device) const{
	if(!initialized)
		return;

	assert(NULL != device);

    //there need to be the same number of textures and materials
	assert(textures_.size()==materials_.size());
    // Meshes are divided into subsets, one for each material. Render them in a loop
        
    for(unsigned int i = 0; i<materials_.size(); i++){
        device->SetMaterial( materials_[i]);
        device->SetTexture(0, textures_[i]);
        
        //make sure the mesh has been initialized at this point
        assert(NULL != mesh_);

        mesh_->DrawSubset(i);
    }
}

}