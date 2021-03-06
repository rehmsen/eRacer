/**
 * @file PhysicsLayer.h
 * @brief The Physics Layer is responsible for simulating all physics objects in the scene.
 *
 * @date 16.01.2010
 * @author: John Stuart, Michael Blackadar
 */

#ifndef PHYSICS_LAYER_H
#define PHYSICS_LAYER_H

#include <iostream>
#include <vector>
#include <algorithm>

#include "NxPhysics.h"
#include "Core/Time.h"
#include "Core/Event.h"
#include "Core/Consts.h"
#include "Core/Math.h"
#include "Convert.h"


using namespace std;

namespace Physics{

enum CollisionGroup{
	INVALID,
	METEOR,
	CAR,
	TRACK
};

/**
* @brief The physics SDK object that the main game loop will use to store actors and return the results of their collisions
*/
class PhysicsLayer   : public Listener, public NxUserContactReport, public NxUserTriggerReport
{
public:
	static PhysicsLayer *g_PhysicsLayer;
	/**
	 * @brief Constructor. User must call Init before using the object.
	 */
	PhysicsLayer();
	/**
	 * @brief Destructor stub.
	 */
	~PhysicsLayer();

	/**
	* @brief Initializes the SDK instance
	*/
	void InitSDK();

	/**
	* @brief Ends the SDK
	*/
	void ReleaseSDK();

	/**
	* @brief Resets the SDK instance by calling Release and then Init
	* Must remember to reinitiaize all of the actors, materials ect.
	*/
	void ResetSDK();

	/**
	* @brief Starts the physics simulation
	*/
	void UpdatePhysics(const Time& t);

	/**
	* @brief Calculates the physics results and saves the info to the actors
	*/
	void GetPhysicsResults();

	/**
	* @brief Reads the parameters needed for the SDK from CONSTS
	*/
	void SetupParameters();

	int ReloadedConstsEvent();

	/**
	* @brief Sets the parameters
	*/
	void InitScene();	

	/**
	* @brief Method that adds an actor to the scene
	*
	* @param actorDesc 
	*						The physX description of the actor that the scene needs for creation
	*
	* @return A pointer to the created actor in the scene
	*/
	NxActor* AddActor(const NxActorDesc& actorDesc);
	void RemoveActor(NxActor* a) { gScene->releaseActor(*a); }

	NxTriangleMesh* CreateTriangleMesh(const NxStream& stream);

	/**
	* @brief Method that adds a material to the scene
	*
	* @param materialDesc 
	*						The physX description of the material that the scene needs for creation
	*
	* @return A pointer to the created material returned from the scene
	*/
	NxMaterial*	AddMaterial(const NxMaterialDesc& materialDesc);

	/**
	* @brief Method that returns the Material index in the scene, for the creation of actors with more detail
	*
	* @param material
	*						The physX material that the index is needed
	*
	* @return The index in the scene of the material needed for the actor
	*/
	int FindMaterialIndex(NxMaterial* material);

	/**
	* @brief Method that returns the Material index in the scene, for the creation of actors with more detail, 
	*		 This is equivalent to calling AddMaterial and FindMaterialIndex, but for human readablilty, it was reduced
	*
	* @param materialDesc 
	*						The physX description of the material that the scene needs for creation
	*
	* @return The index in the scene of the material needed for the actor
	*/
	int AddMaterialReturnIndex(const NxMaterialDesc& materialDesc);

	/**
	* @brief Stars the physics simulation
	*/
	void FinalizeSDK();

	/*
	* @brief Returns the instance of the scene
	*/
	NxScene* ReturnScene();

	virtual void onContactNotify(NxContactPair& pair, NxU32 events);

	virtual void onTrigger(NxShape& triggerShape, NxShape& otherShape, NxTriggerFlag status);

	/**
	* @brief casts a ray from pos and returns the distance to the closest object, 
	* and its normal.
	* @param pos the position to raycast from
	* @param dir the direction of the ray
	* @param normHit the normal of the hit object, overwrittin
	* @return the distance to the hit object
	*/
	float Raycast(const Point3& pos, const Vector3& dir, Vector3& normHit = Vector3(0, 0, 0));

protected:
	// Physics SDK globals
	NxPhysicsSDK* gPhysicsSDK;
	NxScene* gScene;
};

}
#endif
