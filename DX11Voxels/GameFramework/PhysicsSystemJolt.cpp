#include "PhysicsSystemJolt.h"
// Jolt includes
#include <Jolt/RegisterTypes.h>
#include <Jolt/Core/Factory.h>
#include <Jolt/Core/TempAllocator.h>
#include <Jolt/Core/JobSystemThreadPool.h>
#include <Jolt/Physics/PhysicsSettings.h>
#include <Jolt/Physics/PhysicsSystem.h>
#include <Jolt/Physics/Collision/Shape/BoxShape.h>
#include <Jolt/Physics/Collision/Shape/SphereShape.h>
#include <Jolt/Physics/Body/BodyCreationSettings.h>
#include <Jolt/Physics/Body/BodyActivationListener.h>

#include <Jolt/Math/DVec3.h>

#ifdef JPH_DEBUG_RENDERER
#include <Jolt/Renderer/DebugRenderer.h>
#endif


#include "../GameFramework/Game.h"
#include "../GameFramework/DebugRenderSystem.h"


using namespace JPH;


#ifdef JPH_DEBUG_RENDERER
class JoltDebugRenderer : public JPH::DebugRenderer
{
public:
	JoltDebugRenderer();

	void DrawLine(RVec3Arg inFrom, RVec3Arg inTo, ColorArg inColor) override;
	void DrawTriangle(JPH::Vec3Arg inV1, JPH::Vec3Arg inV2, JPH::Vec3Arg inV3, JPH::ColorArg inColor) override;
	Batch CreateTriangleBatch(const Triangle* inTriangles, int inTriangleCount) override;
	Batch CreateTriangleBatch(const Vertex* inVertices, int inVertexCount, const JPH::uint32* inIndices, int inIndexCount) override;
	void DrawGeometry(JPH::Mat44Arg inModelMatrix, const JPH::AABox& inWorldSpaceBounds, float inLODScaleSq, JPH::ColorArg inModelColor, const GeometryRef& inGeometry, ECullMode inCullMode, ECastShadow inCastShadow, EDrawMode inDrawMode) override;
	void DrawText3D(JPH::Vec3Arg inPosition, const std::string_view& inString, JPH::ColorArg inColor, float inHeight) override;
};
#endif

class BatchImpl : public RefTargetVirtual
{
public:
	std::vector<DirectX::SimpleMath::Vector3> triangles;
	DirectX::SimpleMath::Color BatchColor;

	int mRefCount = 0;

	virtual void					AddRef() override { mRefCount++; }
	virtual void					Release() override { if (--mRefCount == 0) delete this; }
};


static void TraceImpl(const char* inFMT, ...)
{
	// Format the message
	va_list list;
	va_start(list, inFMT);
	char buffer[1024];
	vsnprintf(buffer, sizeof(buffer), inFMT, list);

	// Print to the TTY
	std::cout << buffer << "\n";
}


static bool MyObjectCanCollide(ObjectLayer inObject1, ObjectLayer inObject2)
{
	switch (inObject1)
	{
	case Layers::NON_MOVING:
		return inObject2 == Layers::MOVING; // Non moving only collides with moving
	case Layers::MOVING:
		return true; // Moving collides with everything
	default:
		JPH_ASSERT(false);
		return false;
	}
};


static bool MyBroadPhaseCanCollide(ObjectLayer inLayer1, BroadPhaseLayer inLayer2)
{
	switch (inLayer1)
	{
	case Layers::NON_MOVING:
		return inLayer2 == BroadPhaseLayers::MOVING;
	case Layers::MOVING:
		return true;
	default:
		JPH_ASSERT(false);
		return false;
	}
}


class BPLayerInterfaceImpl final : public BroadPhaseLayerInterface
{
public:
	BPLayerInterfaceImpl()
	{
		// Create a mapping table from object to broad phase layer
		mObjectToBroadPhase[Layers::NON_MOVING] = BroadPhaseLayers::NON_MOVING;
		mObjectToBroadPhase[Layers::MOVING] = BroadPhaseLayers::MOVING;
	}

	virtual uint					GetNumBroadPhaseLayers() const override
	{
		return BroadPhaseLayers::NUM_LAYERS;
	}

	virtual BroadPhaseLayer			GetBroadPhaseLayer(ObjectLayer inLayer) const override
	{
		JPH_ASSERT(inLayer < Layers::NUM_LAYERS);
		return mObjectToBroadPhase[inLayer];
	}

#if defined(JPH_EXTERNAL_PROFILE) || defined(JPH_PROFILE_ENABLED)
	virtual const char* GetBroadPhaseLayerName(BroadPhaseLayer inLayer) const override
	{
		switch ((BroadPhaseLayer::Type)inLayer)
		{
		case (BroadPhaseLayer::Type)BroadPhaseLayers::NON_MOVING:	return "NON_MOVING";
		case (BroadPhaseLayer::Type)BroadPhaseLayers::MOVING:		return "MOVING";
		default:													JPH_ASSERT(false); return "INVALID";
		}
	}
#endif // JPH_EXTERNAL_PROFILE || JPH_PROFILE_ENABLED

private:
	BroadPhaseLayer					mObjectToBroadPhase[Layers::NUM_LAYERS];
};


class MyContactListener : public ContactListener
{
public:
	// See: ContactListener
	virtual ValidateResult	OnContactValidate(const Body& inBody1, const Body& inBody2, RVec3Arg inBaseOffset, const CollideShapeResult& inCollisionResult) override
	{
		std::cout << "Contact validate callback" << std::endl;

		// Allows you to ignore a contact before it is created (using layers to not make objects collide is cheaper!)
		return ValidateResult::AcceptAllContactsForThisBodyPair;
	}

	virtual void			OnContactAdded(const Body& inBody1, const Body& inBody2, const ContactManifold& inManifold, ContactSettings& ioSettings) override
	{
		std::cout << "A contact was added" << std::endl;
	}

	virtual void			OnContactPersisted(const Body& inBody1, const Body& inBody2, const ContactManifold& inManifold, ContactSettings& ioSettings) override
	{
		std::cout << "A contact was persisted" << std::endl;
	}

	virtual void			OnContactRemoved(const SubShapeIDPair& inSubShapePair) override
	{
		std::cout << "A contact was removed" << std::endl;
	}
};


class MyBodyActivationListener : public BodyActivationListener
{
public:
	virtual void		OnBodyActivated(const BodyID& inBodyID, uint64 inBodyUserData) override
	{
		std::cout << "A body got activated" << std::endl;
	}

	virtual void		OnBodyDeactivated(const BodyID& inBodyID, uint64 inBodyUserData) override
	{
		std::cout << "A body went to sleep" << std::endl;
	}
};



void PhysicsSystemJolt::Initialize()
{
	RegisterDefaultAllocator();

	// Install callbacks
	//Trace = TraceImpl;
	//JPH_IF_ENABLE_ASSERTS(AssertFailed = AssertFailedImpl;)

	// Create a factory
	Factory::sInstance = new Factory();

	// Register all Jolt physics types
	RegisterTypes();

	
	job_system = new JobSystemThreadPool(cMaxPhysicsJobs, cMaxPhysicsBarriers, thread::hardware_concurrency() - 1);

	broad_phase_layer_interface = new BPLayerInterfaceImpl();
	physics_system = new JPH::PhysicsSystem();
	physics_system->Init(cMaxBodies, cNumBodyMutexes, cMaxBodyPairs, cMaxContactConstraints, *broad_phase_layer_interface, MyBroadPhaseCanCollide, MyObjectCanCollide);

	body_activation_listener = new MyBodyActivationListener();
	physics_system->SetBodyActivationListener(body_activation_listener);

	contact_listener = new MyContactListener();
	physics_system->SetContactListener(contact_listener);

	temp_allocator = new TempAllocatorImpl(10 * 1024 * 1024);


#ifdef JPH_DEBUG_RENDERER
	joltRenderer = new JoltDebugRenderer();
#endif
}


void PhysicsSystemJolt::Update(float dt)
{
	BodyInterface& body_interface = physics_system->GetBodyInterface();

	//static uint step = 0;
	//if (body_interface.IsActive(sphere_id))
	//{
	//	// Next step
	//	++step;
	//
	//	Vec3 position = body_interface.GetCenterOfMassPosition(sphere_id);
	//	Vec3 velocity = body_interface.GetLinearVelocity(sphere_id);
	//	cout << "Step " << step << ": Position = (" << position.GetX() << ", " << position.GetY() << ", " << position.GetZ() << "), Velocity = (" << velocity.GetX() << ", " << velocity.GetY() << ", " << velocity.GetZ() << ")" << endl;
	//}

	// If you take larger steps than 1 / 60th of a second you need to do multiple collision steps in order to keep the simulation stable. Do 1 collision step per 1 / 60th of a second (round up).
	const int cCollisionSteps = 1;

	// If you want more accurate step results you can do multiple sub steps within a collision step. Usually you would set this to 1.
	const int cIntegrationSubSteps = 1;

	// Step the world
	physics_system->Update(dt, cCollisionSteps, cIntegrationSubSteps, temp_allocator, job_system);
}


void PhysicsSystemJolt::CreateDebugScene()
{
	BodyInterface& body_interface = physics_system->GetBodyInterface();

	BoxShapeSettings floor_shape_settings(Vec3(100.0f, 1.0f, 100.0f));

	// Create the shape
	ShapeSettings::ShapeResult floor_shape_result = floor_shape_settings.Create();
	ShapeRefC floor_shape = floor_shape_result.Get(); // We don't expect an error here, but you can check floor_shape_result for HasError() / GetError()

	// Create the settings for the body itself. Note that here you can also set other properties like the restitution / friction.
	BodyCreationSettings floor_settings(floor_shape, Vec3(0.0f, -1.0f, 0.0f), Quat::sIdentity(), EMotionType::Static, Layers::NON_MOVING);

	// Create the actual rigid body
	Body* floor = body_interface.CreateBody(floor_settings); // Note that if we run out of bodies this can return nullptr

	// Add it to the world
	body_interface.AddBody(floor->GetID(), EActivation::DontActivate);

	// Now create a dynamic body to bounce on the floor
	// Note that this uses the shorthand version of creating and adding a body to the world
	BodyCreationSettings sphere_settings(new SphereShape(10.0f), Vec3(0.0f, 200.0f, 0.0f), Quat::sIdentity(), EMotionType::Dynamic, Layers::MOVING);
	sphere_settings.mRestitution = 0.8f;

	sphere_id = body_interface.CreateAndAddBody(sphere_settings, EActivation::Activate);

	sphere_settings.mPosition = Vec3(5.0f, 250.0f, 5.0f);
	sphere_id = body_interface.CreateAndAddBody(sphere_settings, EActivation::Activate);

	// Now you can interact with the dynamic body, in this case we're going to give it a velocity.
	// (note that if we had used CreateBody then we could have set the velocity straight on the body before adding it to the physics system)
	body_interface.SetLinearVelocity(sphere_id, Vec3(0.0f, -5.0f, 0.0f));

	physics_system->OptimizeBroadPhase();

}


void PhysicsSystemJolt::DebugDrawScene()
{
	BodyInterface& body_interface = physics_system->GetBodyInterface();

#ifdef JPH_DEBUG_RENDERER
	BodyManager::DrawSettings settings;
	physics_system->DrawBodies(settings, joltRenderer);
#endif
}

#ifdef JPH_DEBUG_RENDERER
JoltDebugRenderer::JoltDebugRenderer()
{
	Initialize();
}

void JoltDebugRenderer::DrawLine(RVec3Arg inFrom, RVec3Arg inTo, ColorArg inColor)
{
	Game::Instance->DebugRender->DrawLine(DirectX::SimpleMath::Vector3(inFrom.GetX(), inFrom.GetY(), inFrom.GetZ()), DirectX::SimpleMath::Vector3(inTo.GetX(), inTo.GetY(), inTo.GetZ()), DirectX::SimpleMath::Color(inColor.r, inColor.g, inColor.b));
}

void JoltDebugRenderer::DrawTriangle(JPH::Vec3Arg inV1, JPH::Vec3Arg inV2, JPH::Vec3Arg inV3, JPH::ColorArg inColor)
{
	Float3 p0, p1, p2;
	inV1.StoreFloat3(&p0);
	inV2.StoreFloat3(&p1);
	inV3.StoreFloat3(&p2);

	DrawLine(RVec3Arg(p0), RVec3Arg(p1), inColor);
	DrawLine(RVec3Arg(p0), RVec3Arg(p2), inColor);
	DrawLine(RVec3Arg(p1), RVec3Arg(p2), inColor);
}

DebugRenderer::Batch JoltDebugRenderer::CreateTriangleBatch(const Triangle* inTriangles, int inTriangleCount)
{
	auto batch = new BatchImpl();

	for (int i = 0; i < inTriangleCount; ++i)
	{
		auto& tri = inTriangles[i];

		auto p0 = tri.mV[0].mPosition;
		auto p1 = tri.mV[1].mPosition;
		auto p2 = tri.mV[2].mPosition;

		batch->triangles.push_back(DirectX::SimpleMath::Vector3(p0.x, p0.y, p0.z));
		batch->triangles.push_back(DirectX::SimpleMath::Vector3(p1.x, p1.y, p1.z));
		batch->triangles.push_back(DirectX::SimpleMath::Vector3(p2.x, p2.y, p2.z));
	}

	if (inTriangleCount > 0) {
		auto color = inTriangles[0].mV[0].mColor;
		batch->BatchColor = DirectX::SimpleMath::Color(color.r, color.g, color.b);
	}

	return batch;
}

DebugRenderer::Batch JoltDebugRenderer::CreateTriangleBatch(const Vertex* inVertices, int inVertexCount, const JPH::uint32* inIndices, int inIndexCount)
{

	auto batch = new BatchImpl();

	for (int i = 0; i < inIndexCount; i += 3)
	{
		auto p0 = inVertices[inIndices[i+0]].mPosition;
		auto p1 = inVertices[inIndices[i+1]].mPosition;
		auto p2 = inVertices[inIndices[i+2]].mPosition;

		batch->triangles.push_back(DirectX::SimpleMath::Vector3(p0.x, p0.y, p0.z));
		batch->triangles.push_back(DirectX::SimpleMath::Vector3(p1.x, p1.y, p1.z));
		batch->triangles.push_back(DirectX::SimpleMath::Vector3(p2.x, p2.y, p2.z));
	}

	if (inVertexCount > 0) {
		auto color = inVertices[0].mColor;
		batch->BatchColor = DirectX::SimpleMath::Color(color.r, color.g, color.b);
	}

	return batch;
}

void JoltDebugRenderer::DrawGeometry(JPH::Mat44Arg inModelMatrix, const JPH::AABox& inWorldSpaceBounds, float inLODScaleSq, JPH::ColorArg inModelColor, const GeometryRef& inGeometry, ECullMode inCullMode, ECastShadow inCastShadow, EDrawMode inDrawMode)
{
	auto col0 = inModelMatrix.GetColumn4(0);
	auto col1 = inModelMatrix.GetColumn4(1);
	auto col2 = inModelMatrix.GetColumn4(2);
	auto col3 = inModelMatrix.GetColumn4(3);

	DirectX::SimpleMath::Matrix mat(
		col0.GetX(), col0.GetY(), col0.GetZ(), col0.GetW(),
		col1.GetX(), col1.GetY(), col1.GetZ(), col1.GetW(),
		col2.GetX(), col2.GetY(), col2.GetZ(), col2.GetW(),
		col3.GetX(), col3.GetY(), col3.GetZ(), col3.GetW());


	auto pos = inModelMatrix.GetTranslation();
	Game::Instance->DebugRender->DrawPoint(DirectX::SimpleMath::Vector3(pos.GetX(), pos.GetY(), pos.GetZ()), 10);

	//auto center = inWorldSpaceBounds.GetCenter();
	//auto ext = inWorldSpaceBounds.GetExtent();s
	//auto extVec3 = DirectX::SimpleMath::Vector3(ext.GetX(), ext.GetY(), ext.GetZ());
	//
	//DirectX::BoundingBox box(DirectX::SimpleMath::Vector3(center.GetX(), center.GetY(), center.GetZ()), extVec3);
	//Game::Instance->DebugRender->DrawBoundingBox(box);


	auto batch = (BatchImpl*)inGeometry->mLODs[0].mTriangleBatch.GetPtr();

	for (int i = 0; i < batch->triangles.size(); i+=3)
	{
		auto p0 = DirectX::SimpleMath::Vector3::Transform(batch->triangles[i + 0], mat);
		auto p1 = DirectX::SimpleMath::Vector3::Transform(batch->triangles[i + 1], mat);
		auto p2 = DirectX::SimpleMath::Vector3::Transform(batch->triangles[i + 2], mat);

		Game::Instance->DebugRender->DrawLine(p0, p1, batch->BatchColor);
		Game::Instance->DebugRender->DrawLine(p0, p2, batch->BatchColor);
		Game::Instance->DebugRender->DrawLine(p1, p2, batch->BatchColor);
	}
}

void JoltDebugRenderer::DrawText3D(JPH::Vec3Arg inPosition, const std::string_view& inString, JPH::ColorArg inColor, float inHeight)
{
}
#endif
