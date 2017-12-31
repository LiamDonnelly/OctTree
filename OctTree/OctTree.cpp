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

class GameObject
{
public:
	GameObject() {

	}
};

struct BoundingBox
{
public:

	Vec3f _point, _center;
	float _width, _height, _length;

	BoundingBox(Vec3f point, float w, float h, float l) : _point(point), _width(w), _height(h), _length(l)
	{
	} 

};

class OctTree
{
public:
	OctTree* _parent;
	OctTree* _childNodes[8];
	std::vector<GameObject>* _objects;

	
	BoundingBox* _region;

	OctTree(BoundingBox* region)
	{
		_region = region;
	}

	void Add(GameObject obj)
	{
		if (_childNodes == nullptr && _objects->size > 1)
		{
			BuildTree();
		}
		else
		{
			_objects->push_back(obj);
			return;
		}

		int index = GetObjectPosition();
		
		if (index != -1)
		{
			_childNodes[index]->Add(obj);
		}
		else
		{
			_objects->push_back(obj);
		}

	}

	void BuildTree()
	{
		float hw = _region->_width / 2;
		float hh = _region->_height / 2;
		float hl = _region->_length / 2;
		
		float nx = _region->_point.x += hw;
		float ny = _region->_point.y += hh;
		float nz = _region->_point.z += hl;

		_childNodes[0] = new OctTree(new BoundingBox(_region->_point,hw,hh,hl));
		_childNodes[1] = new OctTree(new BoundingBox(Vec3f(nx, _region->_point.y, _region->_point.z),hw,hh,hl));
		_childNodes[2] = new OctTree(new BoundingBox(Vec3f(_region->_point.x, _region->_point.y, nz), hw, hh, hl));
		_childNodes[3] = new OctTree(new BoundingBox(Vec3f(nx, _region->_point.y, nz), hw, hh, hl));
		_childNodes[4] = new OctTree(new BoundingBox(Vec3f(_region->_point.x, ny, _region->_point.z), hw, hh, hl));
		_childNodes[5] = new OctTree(new BoundingBox(Vec3f(nx, ny, _region->_point.z), hw, hh, hl));
		_childNodes[6] = new OctTree(new BoundingBox(Vec3f(_region->_point.x, ny, nz), hw, hh, hl));
		_childNodes[7] = new OctTree(new BoundingBox(Vec3f(nx, ny, nz), hw, hh, hl));


	}

	int GetObjectPosition()
	{

	}
	
};

int main()
{
}

