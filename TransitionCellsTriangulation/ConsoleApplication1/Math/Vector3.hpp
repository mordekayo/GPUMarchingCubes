#pragma once

struct Vector3
{
	Vector3();
	Vector3(float x, float y, float z);
	Vector3(const Vector3& other);
	Vector3& operator=(const Vector3& other);

	float x;
	float y;
	float z;

	Vector3 operator+(const Vector3& other) const;
	Vector3 operator*(int other) const;
	Vector3 operator/(int other) const;

	Vector3 operator-() const;
	Vector3 operator-(const Vector3& other) const;

	float GetMagnitude() const;

	static float DotProduct(const Vector3& lhs, const Vector3& rhs);
	static Vector3 CrossProduct(const Vector3 lhs, const Vector3& rhs);
	static float Distance(const Vector3& lhs, const Vector3& rhs);
	static float AngleBetween(const Vector3& lhs, const Vector3& rhs);
};