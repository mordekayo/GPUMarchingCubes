#include "Vector3.hpp"
#include <cmath>
#include <algorithm>
#include <numbers>
#include <cmath>

#define _USE_MATH_DEFINES

Vector3::Vector3() : x(0), y(0), z(0) {}

Vector3::Vector3(float _x, float _y, float _z) : x(_x), y(_y), z(_z) {}

Vector3::Vector3(const Vector3& other) : x(other.x), y(other.y), z(other.z) {}

Vector3& Vector3::operator=(const Vector3& other)
{
	x = other.x;
	y = other.y;
	z = other.z;
	return *this;
}

Vector3 Vector3::operator+(const Vector3& other) const
{
	return Vector3(x + other.x, y + other.y, z + other.z);
}

Vector3 Vector3::operator*(int other) const
{
	return Vector3(x * other, y * other, z * other);
}

Vector3 Vector3::operator/(int other) const
{
	return Vector3(x / other, y / other, z / other);
}

Vector3 Vector3::operator-() const
{
	return Vector3(-x,-y,-z);
}

Vector3 Vector3::operator-(const Vector3& other) const
{
	return *this + (-other);
}

float Vector3::GetMagnitude() const
{
	return std::sqrt(x * x + y * y + z * z);
}

float Vector3::DotProduct(const Vector3& lhs, const Vector3& rhs)
{
	return lhs.x * rhs.x + lhs.y * rhs.y + lhs.z * rhs.z;
}

Vector3 Vector3::CrossProduct(const Vector3 lhs, const Vector3& rhs)
{
	return Vector3(lhs.y * rhs.z - lhs.z * rhs.y, lhs.z * rhs.x - lhs.x * rhs.z, lhs.x * rhs.y - lhs.y * rhs.x);
}

float Vector3::Distance(const Vector3& lhs, const Vector3& rhs)
{
	return (rhs - lhs).GetMagnitude();
}

float Vector3::AngleBetween(const Vector3& lhs, const Vector3& rhs)
{
	auto dotProduct = DotProduct(lhs, rhs);
	auto lMagnitude = lhs.GetMagnitude();
	auto rMagnitude = rhs.GetMagnitude();
	auto cos = dotProduct / (lMagnitude * rMagnitude);
	return std::acos(std::clamp(cos, -1.0f, 1.0f)) * 180.0f / std::numbers::pi;
}
