#pragma once

class Vector3;

class IGraphNode
{
public: 
	virtual ~IGraphNode() {}
	virtual bool IsActive() const = 0;
	virtual int GetIndex() const = 0;
	virtual Vector3 GetPosition() const = 0;
};