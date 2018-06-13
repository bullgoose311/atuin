#pragma once

struct Vector3
{
	Vector3() : m_x(0), m_y(0), m_z(0) {}
	Vector3(float x, float y, float z) : m_x(x), m_y(y), m_z(z) {}

	float m_x;
	float m_y;
	float m_z;
};

struct Quaternion
{
};