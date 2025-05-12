#pragma once
#include "Exports.h"
#include "PxPhysicsAPI.h"
#include "../GameFramework/TinyModelComponent.h"
#include "../ZMathLib/SimpleMath.h"


class GAMEFRAMEWORK_API ShadowedPhysModel
{
public:
	physx::PxRigidDynamic* dynamic;
	TinyModelComponent* comp;

	float scale = 1.0f;

	void Update();
};


struct GAMEFRAMEWORK_API PhysicsSystemPhysX
{
	physx::PxDefaultAllocator		gAllocator;
	physx::PxDefaultErrorCallback	gErrorCallback;

	physx::PxFoundation* gFoundation = NULL;
	physx::PxPhysics* gPhysics = NULL;

	physx::PxDefaultCpuDispatcher* gDispatcher = NULL;
	physx::PxScene* gScene = NULL;

	physx::PxCudaContextManager* gCudaContextManager = NULL;

	physx::PxMaterial* gMaterial = NULL;

	physx::PxPvd* gPvd = NULL;

	physx::PxReal stackZ = 10.0f;
#define PVD_HOST "127.0.0.1"


	physx::PxRigidDynamic* CreateDynamic(const physx::PxTransform& t, const physx::PxGeometry& geometry, const physx::PxVec3& velocity = physx::PxVec3(0));
	void CreateStack(const physx::PxTransform& t, physx::PxU32 size, physx::PxReal halfExtent);

	void InitPhysics(bool interactive);
	void StepPhysics(bool /*interactive*/);
	void DrawDebugLines();
};

