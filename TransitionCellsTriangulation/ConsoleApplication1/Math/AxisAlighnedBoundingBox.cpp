#include "AxisAlighnedBoundingBox.hpp"

AABB::AABB()
{
}

AABB::AABB(Vector3 _lowLeft, Vector3 _highRight)
{
	if (_lowLeft.y < _highRight.y)
	{
		lowLeft = _lowLeft;
		highRight = _highRight;
	}
	else
	{
		lowLeft = Vector3(_lowLeft.x, _highRight.y, _lowLeft.z);
		highRight = Vector3(_highRight.x, _lowLeft.y, _highRight.z);
	}
}

bool AABB::IsPointInside(Vector3 point) const
{
	return point.x >= lowLeft.x && point.x <= highRight.x &&
		   point.y >= lowLeft.y && point.y <= highRight.y &&
		   point.z >= lowLeft.z && point.z <= highRight.z;
}
