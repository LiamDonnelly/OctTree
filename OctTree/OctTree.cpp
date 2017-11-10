// OctTree.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <vector>

template<typename T>
class Vec3
{
public:
	T x, y, z;
	Vec3() : x(T(0)), y(T(0)), z(T(0)) {}
	Vec3(T xx) : x(xx), y(xx), z(xx) {}
	Vec3(T xx, T yy, T zz) : x(xx), y(yy), z(zz) {}
	Vec3& normalize()
	{
		T nor2 = length2();
		if (nor2 > 0) {
			T invNor = 1 / sqrt(nor2);
			x *= invNor, y *= invNor, z *= invNor;
		}
		return *this;
	}
	Vec3<T> operator * (const T &f) const { return Vec3<T>(x * f, y * f, z * f); }
	Vec3<T> operator * (const Vec3<T> &v) const { return Vec3<T>(x * v.x, y * v.y, z * v.z); }
	T dot(const Vec3<T> &v) const { return x * v.x + y * v.y + z * v.z; }
	Vec3<T> operator - (const Vec3<T> &v) const { return Vec3<T>(x - v.x, y - v.y, z - v.z); }
	Vec3<T> operator + (const Vec3<T> &v) const { return Vec3<T>(x + v.x, y + v.y, z + v.z); }
	Vec3<T>& operator += (const Vec3<T> &v) { x += v.x, y += v.y, z += v.z; return *this; }
	bool Vec3::operator == (const Vec3<T> &v) { if (x == v.x && y == v.y && z == v.z) { return true; }; return false; }
	Vec3<T>& operator *= (const Vec3<T> &v) { x *= v.x, y *= v.y, z *= v.z; return *this; }
	Vec3<T> operator - () const { return Vec3<T>(-x, -y, -z); }
	T length2() const { return x * x + y * y + z * z; }
	T length() const { return sqrt(length2()); }
	friend std::ostream & operator << (std::ostream &os, const Vec3<T> &v)
	{
		os << "[" << v.x << " " << v.y << " " << v.z << "]";
		return os;
	}
};

typedef Vec3<float> Vec3f;

class GameObjects
{
public:
	GameObjects() {

	}
};

struct BoundingBox
{
public:

	 Vec3f _min, _max;

	BoundingBox(Vec3f p1, Vec3f p2)
	{
		_min = p1;
		_max = p2;
	}

};

class OctTree
{
public:
	std::vector<GameObjects>* _objects;

	OctTree* _parent;
	OctTree* _childNodes = new OctTree[8];
	BoundingBox* _region;

	OctTree()
	{
		_objects = new std::vector<GameObjects>();
		_region = new BoundingBox((0.0f), (0.0f));
	}
	OctTree(BoundingBox* region, std::vector<GameObjects>* objects)
	{
		_region = region;
		_objects = objects;

	}

	void resize(BoundingBox* region)
	{
		_region = region;
	}
	

	void BuildTree()
	{
		if (_objects->size() <= 1)
		{
			return;
		}

		Vec3f dimensions = _region->_max - _region->_min;

		if (dimensions == Vec3f(0.0f))
		{
			return;
		}
	}
};

int main()
{
	OctTree oct = OctTree();
	oct.BuildTree();
    return 0;
}

