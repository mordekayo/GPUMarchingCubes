#define _SILENCE_CXX20_CISO646_REMOVED_WARNING

#include "PhysicsSystemPhysX.h"

#include "../GameFramework/DebugRenderSystem.h"

using namespace physx;



void ShadowedPhysModel::Update()
{
	auto transform = dynamic->getGlobalPose();

	comp->Transform = Matrix::CreateScale(scale) *
		Matrix::CreateFromQuaternion(Quaternion(transform.q.x, transform.q.y, transform.q.z, transform.q.w));
	comp->Position = Vector3(transform.p.x, transform.p.y, transform.p.z);
}


PxRigidDynamic* PhysicsSystemPhysX::CreateDynamic(const PxTransform& t, const PxGeometry& geometry, const PxVec3& velocity)
{
	PxRigidDynamic* dynamic = PxCreateDynamic(*gPhysics, t, geometry, *gMaterial, 10.0f);
	dynamic->setAngularDamping(0.5f);
	dynamic->setLinearVelocity(velocity);
	gScene->addActor(*dynamic);

	return dynamic;
}

void PhysicsSystemPhysX::CreateStack(const PxTransform& t, PxU32 size, PxReal halfExtent)
{
	//auto game = static_cast<ShadowGame*>(Game::Instance);
	//
	//PxShape* shape = gPhysics->createShape(PxBoxGeometry(halfExtent, halfExtent, halfExtent), *gMaterial);
	//for (PxU32 i = 0; i < size; i++)
	//{
	//	for (PxU32 j = 0; j < size - i; j++)
	//	{
	//		PxTransform localTm(PxVec3(PxReal(j * 2) - PxReal(size - i), PxReal(i * 2 + 1), 0) * halfExtent);
	//		PxRigidDynamic* body = gPhysics->createRigidDynamic(t.transform(localTm));
	//		body->attachShape(*shape);
	//		PxRigidBodyExt::updateMassAndInertia(*body, 10.0f);
	//		gScene->addActor(*body);
	//
	//		auto model = game->SpawnModel("./Content/BoxPivotCenter.obj");
	//		model->dynamic = body;
	//		model->scale = halfExtent * 2;
	//	}
	//}
	//shape->release();
}

void PhysicsSystemPhysX::InitPhysics(bool interactive)
{
	gFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, gAllocator, gErrorCallback);

	//gPvd = PxCreatePvd(*gFoundation);
	//PxPvdTransport* transport = PxDefaultPvdSocketTransportCreate(PVD_HOST, 5425, 10);
	//gPvd->connect(*transport, PxPvdInstrumentationFlag::eALL);

	gPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *gFoundation, PxTolerancesScale(), true, nullptr /*gPvd*/);

	PxCudaContextManagerDesc cudaContextManagerDesc;
	cudaContextManagerDesc.interopMode = PxCudaInteropMode::NO_INTEROP;	//Choose interop mode. As the snippets use OGL, we select OGL_INTEROP

	gCudaContextManager = PxCreateCudaContextManager(*gFoundation, cudaContextManagerDesc, PxGetProfilerCallback());	//Create the CUDA context manager, required for GRB to dispatch CUDA kernels.
	if (gCudaContextManager)
	{
		if (!gCudaContextManager->contextIsValid())
		{
			gCudaContextManager->release();
			gCudaContextManager = NULL;
		}
	}


	PxSceneDesc sceneDesc(gPhysics->getTolerancesScale());
	sceneDesc.gravity = PxVec3(0.0f, -9.81f, 0.0f);
	gDispatcher = PxDefaultCpuDispatcherCreate(4);
	sceneDesc.cpuDispatcher = gDispatcher;
	sceneDesc.filterShader = PxDefaultSimulationFilterShader;

	sceneDesc.cudaContextManager = gCudaContextManager;		//Set the CUDA context manager, used by GRB.

	sceneDesc.flags |= PxSceneFlag::eENABLE_GPU_DYNAMICS;	//Enable GPU dynamics - without this enabled, simulation (contact gen and solver) will run on the CPU.
	sceneDesc.flags |= PxSceneFlag::eENABLE_PCM;			//Enable PCM. PCM NP is supported on GPU. Legacy contact gen will fall back to CPU
	sceneDesc.flags |= PxSceneFlag::eENABLE_STABILIZATION;	//Improve solver stability by enabling post-stabilization.
	sceneDesc.broadPhaseType = PxBroadPhaseType::eGPU;		//Enable GPU broad phase. Without this set, broad phase will run on the CPU.
	sceneDesc.gpuMaxNumPartitions = 8;						//Defines the maximum number of partitions used by the solver. Only power-of-2 values are valid. 
	//A value of 8 generally gives best balance between performance and stability.


	gScene = gPhysics->createScene(sceneDesc);

	gScene->setVisualizationParameter(PxVisualizationParameter::eSCALE, 1.0f);
	gScene->setVisualizationParameter(PxVisualizationParameter::eACTOR_AXES, 1.0f);
	gScene->setVisualizationParameter(PxVisualizationParameter::eCOLLISION_SHAPES, 1.0f);

	//PxPvdSceneClient* pvdClient = gScene->getScenePvdClient();
	//if (pvdClient)
	//{
	//	pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_CONSTRAINTS, true);
	//	pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_CONTACTS, true);
	//	pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_SCENEQUERIES, true);
	//}

	gMaterial = gPhysics->createMaterial(0.5f, 0.5f, 0.6f);

	PxRigidStatic* groundPlane = PxCreatePlane(*gPhysics, PxPlane(0, 1, 0, 0), *gMaterial);
	gScene->addActor(*groundPlane);
}

void PhysicsSystemPhysX::StepPhysics(bool)
{
	gScene->simulate(1.0f / 60.0f);
	gScene->fetchResults(true);
}

void PhysicsSystemPhysX::DrawDebugLines()
{
	const PxRenderBuffer& rb = gScene->getRenderBuffer();
	for (PxU32 i = 0; i < rb.getNbLines(); i++)
	{
		const PxDebugLine& line = rb.getLines()[i];
		// render the line
		Game::Instance->DebugRender->DrawLine(DirectX::SimpleMath::Vector3(line.pos0.x, line.pos0.y, line.pos0.z),
		                      DirectX::SimpleMath::Vector3(line.pos1.x, line.pos1.y, line.pos1.z),
		                      DirectX::SimpleMath::Color(0.8f, 0.8f, 0.8f, 1.0f));
	}
	for (PxU32 i = 0; i < rb.getNbPoints(); i++)
	{
		const PxDebugPoint& point = rb.getPoints()[i];
		// render the point
		Game::Instance->DebugRender->DrawPoint(DirectX::SimpleMath::Vector3(point.pos.x, point.pos.y, point.pos.z), 10);
	}
	for (PxU32 i = 0; i < rb.getNbTriangles(); i++)
	{
		const PxDebugTriangle& tri = rb.getTriangles()[i];
		// render the line
		Game::Instance->DebugRender->DrawLine(DirectX::SimpleMath::Vector3(tri.pos0.x, tri.pos0.y, tri.pos0.z),
		                      DirectX::SimpleMath::Vector3(tri.pos1.x, tri.pos1.y, tri.pos1.z),
		                      DirectX::SimpleMath::Color(1.0f, 0.0f, 0.0f, 1.0f));
		Game::Instance->DebugRender->DrawLine(DirectX::SimpleMath::Vector3(tri.pos1.x, tri.pos1.y, tri.pos1.z),
		                      DirectX::SimpleMath::Vector3(tri.pos2.x, tri.pos2.y, tri.pos2.z),
		                      DirectX::SimpleMath::Color(1.0f, 0.0f, 0.0f, 1.0f));
		Game::Instance->DebugRender->DrawLine(DirectX::SimpleMath::Vector3(tri.pos0.x, tri.pos0.y, tri.pos0.z),
		                      DirectX::SimpleMath::Vector3(tri.pos2.x, tri.pos2.y, tri.pos2.z),
		                      DirectX::SimpleMath::Color(1.0f, 0.0f, 0.0f, 1.0f));
	}
	//*/
}
