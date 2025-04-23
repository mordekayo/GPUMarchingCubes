#pragma once

class IGraphNode
{
public: 
	virtual ~IGraphNode() {}
	virtual bool IsActive() const = 0;
	virtual int GetIndex() const = 0;
};