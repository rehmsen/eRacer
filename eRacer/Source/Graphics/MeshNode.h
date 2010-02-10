/**
 * @file MeshNode.h
 * @brief Definition of the MeshNode class
 *
 * @date 22.01.2010
 * @author: Ole Rehmsen
 */

#pragma once

#include "Spatial.h"
#include <vector>
#include "Renderable.h"
#include "d3d9types.h"
#include "d3dx9mesh.h"
#include "Mesh.h"


using namespace std;

namespace Graphics {

/**
 * @brief A mesh that is part of the scene graph
 * 
 * @see MovingGeometry
 * @see StaticGeometry
 */
class MeshNode : public Spatial, public Mesh
{
public:
	/**
	 * @brief Destructor stub. Virtual so that subclasse's constructors will be called
	 *
	 */
	virtual ~MeshNode();


	virtual void Draw(IDirect3DDevice9* device) const;

	/**
	 * @brief Add myself to the list
	 *
	 * @param camera
	 *			The camera to cull against - not needed here
	 * @param visibleNodes
	 * 			A vector to push this node to
	 *
	 * @see Spatial::cullRecursive
	 */
	virtual void cullRecursive(const Camera& camera, vector<const MeshNode*>& visibleNodes) const;

	const Matrix& GetTransform() const { return transform_; }



protected:
	/**
	 * @brief Constructor. Only for inheriting classes because this class is abstract.
	 *
	 * @param name
	 *			a name for this node to fascilitate debugging
	 */
	MeshNode(const string& name);

	void UpdateBounds();


	Matrix transform_;

};



}