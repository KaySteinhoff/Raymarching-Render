#include <iostream>
#include <math.h>
#include <vector>
#include <fstream>
#include "toojpeg.h"
using namespace std;

class vec2
{
public:
	float x, y;

	vec2()
	{
		this->x = 0.0;
		this->y = 0.0;
	}

	vec2(float x, float y)
	{
		this->x = x;
		this->y = y;
	}

};

class vec3
{
public:
	float x, y, z;

	vec3()
	{
		this->x = 0.0;
		this->y = 0.0;
		this->z = 0.0;
	}

	vec3(float x, float y, float z)
	{
		this->x = x;
		this->y = y;
		this->z = z;
	}

	vec3(vec2 start, float z)
	{
		this->x = start.x;
		this->y = start.y;
		this->z = z;
	}

	vec3(float x, vec2 end)
	{
		this->x = x;
		this->y = end.x;
		this->z = end.y;
	}

};

vec3 VecMul(vec3 first, vec3 second)
{
	return vec3(first.x * second.x, first.y * second.y, first.z * second.z);
}

vec3 VecMul(vec3 first, float second)
{
	return vec3(first.x * second, first.y * second, first.z * second);
}

vec2 VecMul(vec2 first, vec2 second)
{
	return vec2(first.x * second.x, first.y * second.y);
}

vec2 VecMul(vec2 first, float second)
{
	return vec2(first.x * second, first.y * second);
}

float Distance(vec3 f, vec3 s)
{
	return sqrt((f.x - s.x) * (f.x - s.x) + (f.y - s.y) * (f.y - s.y) + (f.z - s.z) * (f.z - s.z));
}

float dot(vec3 f, vec3 s)
{
	return f.x * s.x + f.y * s.y + f.z * s.z;
}

double radians(float degrees)
{
	return degrees * (3.14159265359/180.0);
}

vec3 normalize(vec3 v)
{
	float length = sqrt(v.x * v.x +  v.y * v.y + v.z * v.z);
	return vec3(v.x/length, v.y/length, v.z/length);
}

vec3 CalculateRay(float fieldOfView, vec2 size, vec2 fragCoord) {
    vec2 xy = vec2(fragCoord.x - size.x / 2.0, fragCoord.y - size.y / 2.0);
    float z = size.y / tan(radians(fieldOfView) / 2.0);
    return normalize(vec3(xy, -z));
}

class Material
{
public:
	float r = 255.0, g = 255.0, b = 255.0;
	float metallic = 0.8;
};

class Sphere
{
public:
	vec3 position;
	float radius;
	Material material;
	
	Sphere()
	{
		this->position = vec3(0.0, 0.0, 0.0);
		this->radius = 1.0;
	}

	Sphere(vec3 position)
	{
		this->position = position;
		this->radius = 1.0;
	}

	Sphere(vec3 position, float radius)
	{
		this->position = position;
		this->radius = radius;
	}

	float Sdf(vec3 pointToCheck)
	{
		float displacement = sin(5.0 * pointToCheck.x) * sin(5.0 * pointToCheck.y) * sin(5.0 * pointToCheck.z) * 0.25;
		return Distance(pointToCheck, this->position) - radius + displacement;
	}

	vec3 calculate_normal(vec3 p)
	{
		vec3 v1 = vec3(p.x + 0.001f, p.y, p.z);
		vec3 v2 = vec3(p.x, p.y + 0.001f, p.z);
		vec3 v3 = vec3(p.x, p.y, p.z + 0.001f);

		vec3 v4 = vec3(p.x - 0.001f, p.y, p.z);
		vec3 v5 = vec3(p.x, p.y - 0.001f, p.z);
		vec3 v6 = vec3(p.x, p.y, p.z - 0.001f);

		float gradientX = Sdf(v1) - Sdf(v4);
		float gradientY = Sdf(v2) - Sdf(v5);
		float gradientZ = Sdf(v3) - Sdf(v6);

		return normalize(vec3(gradientX, gradientY, gradientZ));

	}

};
