/**
 * @file Scene.cpp
 * @brief Implementation of the Scene class
 *
 * @date 18.01.2010
 * @author: Ole Rehmsen
 */

#include "Scene.h"
#include "IO/IO.h"
#include "SkyBox.h"

namespace Graphics {


Scene::Scene()
{

}


Scene::~Scene(){
	for(vector<Geometry*>::iterator i = geometries_.begin();
		i != geometries_.end(); i++)
		delete *i;
	for(vector<Renderable*>::iterator i = renderables_.begin();
		i != renderables_.end(); i++)
		delete *i;
}

void Scene::GetVisibleRenderables(const Camera& camera, vector<Renderable*>& visibleRenderables) const {
	for (vector<Geometry*>::const_iterator i = geometries_.begin(); i != geometries_.end(); i++)
	{
		if ((*i)->visible)
			visibleRenderables.push_back(*i);
	}
	visibleRenderables.insert(visibleRenderables.end(), renderables_.begin(), renderables_.end());
}

MovingGeometry* Scene::CreateMovingGeometry(const string& name, const Matrix& transform) {
	MovingGeometry* result = new MovingGeometry(name);
	result->SetTransform(transform);
	geometries_.push_back(result);
	return result;
}
StaticGeometry* Scene::CreateStaticGeometry(const string& name, const Matrix& transform) {
	StaticGeometry* result = new StaticGeometry(name, transform);
	geometries_.push_back(result);
	return result;
}


void Scene::LoadSkyBox(const std::string& filename){
	const float SKYBOX_SIZE = 1000;

	SkyBox* skyBox = new SkyBox();

	//we might want to make IO use strings in future...
	IO::GetInstance()->LoadMesh(skyBox, filename.c_str());
	//skyBox_.SetTransform(CreateMatrix(SKYBOX_SIZE,SKYBOX_SIZE,SKYBOX_SIZE));
	renderables_.push_back(skyBox);
}

/*
const Geometry& Scene::GetSkyBox() const{
	return skyBox_;
}
*/

};
