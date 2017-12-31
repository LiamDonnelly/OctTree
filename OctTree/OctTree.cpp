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
	Vec3f postion;
	float radius;
	GameObject(Vec3f pos, float rad) : postion(pos), radius(rad)
	{
	}
};

struct BoundingBox
{
public:

	Vec3f _point, _center;
	float _width, _height, _length;

	BoundingBox(Vec3f point, float w, float h, float l) : _point(point), _width(w), _height(h), _length(l)
	{
		_center = Vec3f(point.x + (_width / 2), point.y + (_height / 2), point.z + (_length / 2));
	} 

};

class OctTree
{
public:

	OctTree* _parent;
	OctTree* _childNodes[8];
	std::vector<GameObject> _objects;
	BoundingBox* _region;

	int level;
	bool isLeaf;

	int MAX_RECURSION = 5;
	int MAX_OBJECTS = 2;

	OctTree(BoundingBox* region, int lvl = 0, OctTree* parent = nullptr) : level(lvl), _parent(parent)
	{
		_region = region;
		isLeaf = true;
	}

	OctTree(int size, int lvl = 0, OctTree* parent = nullptr) : level(lvl), _parent(parent)
	{	
		_region = new BoundingBox(Vec3f(0),size,size,size);
		isLeaf = true;
	}

	void Add(GameObject obj)
	{
		if (!isLeaf)
		{
			int index = GetObjectPosition(obj.postion, obj.radius);

			if (index != -1)
			{
				_childNodes[index]->Add(obj);
			}
			else
			{
				_objects.push_back(obj);
			}
		}
		else
		{
			if (_objects.size() == MAX_OBJECTS && level != MAX_RECURSION)
			{
				BuildTree();
				std::vector<GameObject> objs(_objects);
				_objects.clear();
				int num = objs.size();
				for (int i = 0; i < num; i++)
				{
					Add(objs[i]);
				}
				Add(obj);
			}
			else
			{
				_objects.push_back(obj);
			}
		}
	}

	void BuildTree()
	{
		isLeaf = false;

		float hw = _region->_width / 2;
		float hh = _region->_height / 2;
		float hl = _region->_length / 2;
		
		float nx = _region->_point.x + hw;
		float ny = _region->_point.y + hh;
		float nz = _region->_point.z + hl;

		_childNodes[0] = new OctTree(new BoundingBox(_region->_point,hw,hh,hl), level + 1, this);
		_childNodes[1] = new OctTree(new BoundingBox(Vec3f(nx, _region->_point.y, _region->_point.z),hw,hh,hl), level + 1, this);
		_childNodes[2] = new OctTree(new BoundingBox(Vec3f(_region->_point.x, _region->_point.y, nz), hw, hh, hl), level + 1, this);
		_childNodes[3] = new OctTree(new BoundingBox(Vec3f(nx, _region->_point.y, nz), hw, hh, hl), level + 1, this);
		_childNodes[4] = new OctTree(new BoundingBox(Vec3f(_region->_point.x, ny, _region->_point.z), hw, hh, hl), level + 1, this);
		_childNodes[5] = new OctTree(new BoundingBox(Vec3f(nx, ny, _region->_point.z), hw, hh, hl), level + 1, this);
		_childNodes[6] = new OctTree(new BoundingBox(Vec3f(_region->_point.x, ny, nz), hw, hh, hl), level + 1, this);
		_childNodes[7] = new OctTree(new BoundingBox(Vec3f(nx, ny, nz), hw, hh, hl), level + 1, this);
	}

	int GetObjectPosition(Vec3f pos, float radius)
	{
		int index = -1;

		float objTop = pos.y += radius;
		float objBottom = pos.y -= radius;

		float objRight = pos.x += radius;
		float objLeft = pos.x -= radius;

		float objFront = pos.z += radius;
		float objBack = pos.z -= radius;

		if (objTop < _region->_center.y)
		{
			//Can be 0,1,2,3
			if (objLeft < _region->_center.x)
			{
				//Can be 0,2
				if (objBack < _region->_center.z)
				{
					//Is 0
					index = 0;
				}
				else if (objFront > _region->_center.z)
				{
					//Is 2
					index = 2;
				}
			}
			else if(objRight > _region->_center.x)
			{
				//Can be 1,3
				if (objBack < _region->_center.z)
				{
					//Is 1
					index = 1;
				}
				else if(objFront > _region->_center.z)
				{
					//Is 3
					index = 3;
				}
			}
		}
		else if(objBottom > _region->_center.y)
		{
			//Can be 4,5,6,7
			if (objLeft < _region->_center.x)
			{
				//Can be 4,6
				if (objBack < _region->_center.z)
				{
					//Is 4
					index = 4;
				}
				else if (objFront > _region->_center.z)
				{
					//Is 6
					index = 6;
				}
			}
			else if (objRight > _region->_center.x)
			{
				//Can be 5,7
				if (objBack < _region->_center.z)
				{
					//Is 5
					index = 5;
				}
				else if (objFront > _region->_center.z)
				{
					//Is 7
					index = 7;
				}
			}
		}

		return index;
	}
	
};

int main()
{
	OctTree tree = OctTree(16);
	tree.Add(GameObject(Vec3f(1),9));
	tree.Add(GameObject(Vec3f(1), 1));
	tree.Add(GameObject(Vec3f(9), 1));
}

