#pragma once
#include "Exports.h"
#include "pch.h"

class StaticMesh;
class ZActor;

class GAMEFRAMEWORK_API ZTransform
{
public:
	ZTransform() : Transform(DirectX::SimpleMath::Matrix::Identity), Position(DirectX::SimpleMath::Vector3::Zero),
		Scale(DirectX::SimpleMath::Vector3::One), Rotation(DirectX::SimpleMath::Quaternion::Identity)
	{
		
	}
	
	DirectX::SimpleMath::Matrix Transform;

	DirectX::SimpleMath::Vector3 Position;
	DirectX::SimpleMath::Vector3 Scale;
	DirectX::SimpleMath::Quaternion Rotation;

	inline void Update()
	{
		Transform = DirectX::SimpleMath::Matrix::CreateScale(Scale) * DirectX::SimpleMath::Matrix::CreateFromQuaternion(Rotation) * DirectX::SimpleMath::Matrix::CreateTranslation(Position);
	}
};



class GAMEFRAMEWORK_API ZComponent // more like ZSceneComponent
{
public:
	std::shared_ptr<ZActor> Owner = nullptr;
	std::shared_ptr<ZComponent> Parent = nullptr;

	ZTransform Transform;

public:
	bool shouldUpdate = true;

public:

	virtual void Update(float dt) {};


	virtual ~ZComponent();
};


class GAMEFRAMEWORK_API ZStaticMeshComponent : public ZComponent
{
public:
	
	std::vector<std::shared_ptr<StaticMesh>> Meshes;

	ZStaticMeshComponent();

	virtual void Update(float dt) override;

	virtual ~ZStaticMeshComponent();
};


class GAMEFRAMEWORK_API ZActor
{
public:
	std::vector<std::shared_ptr<ZComponent>> Components;

	std::shared_ptr<ZComponent> RootComponent = nullptr;

	template<class T>
	T* FindComponentByClass() const
	{
		T* ret = nullptr;

		for (auto& comp : Components) {
			auto t = dynamic_cast<T*> (comp.get());
			if (t) {
				ret = t;
				break;
			}
		}

		return ret;
	}

	void Update(float dt);

	~ZActor();
};


std::shared_ptr<ZActor> GAMEFRAMEWORK_API CreateNewStaticMeshActor(const std::string& meshPath);
std::shared_ptr<ZActor> GAMEFRAMEWORK_API CreateNewStaticMeshActorAssimp(const std::string& meshPath);

