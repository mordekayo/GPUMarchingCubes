#pragma once
#include "Exports.h"
#include <cstdarg>
#include <iostream>

#include "Jolt/Jolt.h"
#include <Jolt/Physics/Body/BodyCreationSettings.h>
#include <Jolt/Physics/Collision/BroadPhase/BroadPhaseLayer.h>
#include <Jolt/Core/JobSystemThreadPool.h>
#include <Jolt/Physics/PhysicsSystem.h>



class JoltDebugRenderer;

namespace Layers
{
	static constexpr uint16_t NON_MOVING = 0;
	static constexpr uint16_t MOVING = 1;
	static constexpr uint16_t NUM_LAYERS = 2;
}


namespace JPH
{
	namespace BroadPhaseLayers
	{
		static constexpr BroadPhaseLayer NON_MOVING(0);
		static constexpr BroadPhaseLayer MOVING(1);
		static constexpr uint NUM_LAYERS(2);
	}
}

class BPLayerInterfaceImpl;

struct GAMEFRAMEWORK_API PhysicsSystemJolt
{
	const JPH::uint cMaxBodies = 1024;
	const JPH::uint cNumBodyMutexes = 0;
	const JPH::uint cMaxBodyPairs = 1024;
	const JPH::uint cMaxContactConstraints = 1024;

	JPH::PhysicsSystem* physics_system;
	JPH::JobSystemThreadPool* job_system;
	JPH::BroadPhaseLayerInterface* broad_phase_layer_interface;
	JPH::ContactListener* contact_listener;
	JPH::BodyActivationListener* body_activation_listener;

	JPH::TempAllocator* temp_allocator;
	JPH::BodyID sphere_id;

	JoltDebugRenderer* joltRenderer;


	void Initialize();
	void Update(float dt);


	void CreateDebugScene();
	void DebugDrawScene();
};


