#pragma once

#include "Vector3.hpp"

class AABB
{
public:

	AABB();
	AABB(Vector3 lowLeft, Vector3 highRight);
	bool IsPointInside(Vector3 point) const;

private:

	Vector3 lowLeft;
	Vector3 highRight;
};