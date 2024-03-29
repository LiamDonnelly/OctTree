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
	friend Vec3 operator / (const T &r, const Vec3 &v)
	{
		return Vec3<T>(r / v.x, r / v.y, r / v.z);
	}

	T length2() const { return x * x + y * y + z * z; }
	T length() const { return sqrt(length2()); }
	friend std::ostream & operator << (std::ostream &os, const Vec3<T> &v)
	{
		os << "[" << v.x << " " << v.y << " " << v.z << "]";
		return os;
	}
};

typedef Vec3<float> Vec3f;



struct BoundingBox
{
public:

	Vec3f _point, _center;
	float _width, _height, _length;
	Vec3f bounds[2];

	BoundingBox() {}
	BoundingBox(Vec3f point, float w, float h, float l) : _point(point), _width(w), _height(h), _length(l)
	{
		_center = Vec3f(point.x + (_width / 2), point.y + (_height / 2), point.z + (_length / 2));
		bounds[0] = _point;
		bounds[1] = Vec3f(_point.x + _width, _point.y + _height, _point.z + _length);
	} 

	bool Intersect(const Vec3f rayOrig, Vec3f rayDir, float t)
	{
		float tmin, tmax, tymin, tymax, tzmin, tzmax;
		Vec3f invdir = 1 / rayDir;
		int sign[3];
		sign[0] = (invdir.x < 0);
		sign[1] = (invdir.y < 0);
		sign[2] = (invdir.z < 0);

		tmin = (bounds[sign[0]].x - rayOrig.x) * invdir.x;
		tmax = (bounds[1 - sign[0]].x - rayOrig.x) * invdir.x;
		tymin = (bounds[sign[1]].y - rayOrig.y) * invdir.y;
		tymax = (bounds[1 - sign[1]].y - rayOrig.y) * invdir.y;

		if ((tmin > tymax) || (tymin > tmax))
			return false;

		if (tymin > tmin)
			tmin = tymin;
		if (tymax < tmax)
			tmax = tymax;

		tzmin = (bounds[sign[2]].z - rayOrig.z) * invdir.z;
		tzmax = (bounds[1 - sign[2]].z - rayOrig.z) * invdir.z;

		if ((tmin > tzmax) || (tzmin > tmax))
			return false;

		if (tzmin > tmin)
			tmin = tzmin;
		if (tzmax < tmax)
			tmax = tzmax;

		t = tmin;

		if (t < 0) {
			t = tmax;
			if (t < 0) return false;
		}

		return true;
	}
};

class GameObject
{
public:
	std::string name;
	BoundingBox bounds;
	Vec3f postion;
	float radius;
	GameObject() {}
	GameObject(Vec3f pos, float rad,std::string n) : postion(pos), radius(rad), name(n)
	{
		int r = rad * 2;
		bounds = BoundingBox(Vec3f(pos.x - rad, pos.y - rad, pos.z + rad), r, r, r);
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

	//Axis Alligned
	OctTree(int size, int lvl = 0, OctTree* parent = nullptr) : level(lvl), _parent(parent)
	{	
		_region = new BoundingBox(Vec3f(0),size,size,-size);
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
		_childNodes[4] = new OctTree(new BoundingBox(Vec3f(_region->_point.x, _region->_point.y, nz), hw, hh, hl), level + 1, this);
		_childNodes[5] = new OctTree(new BoundingBox(Vec3f(nx, _region->_point.y, nz), hw, hh, hl), level + 1, this);
		_childNodes[2] = new OctTree(new BoundingBox(Vec3f(_region->_point.x, ny, _region->_point.z), hw, hh, hl), level + 1, this);
		_childNodes[3] = new OctTree(new BoundingBox(Vec3f(nx, ny, _region->_point.z), hw, hh, hl), level + 1, this);
		_childNodes[6] = new OctTree(new BoundingBox(Vec3f(_region->_point.x, ny, nz), hw, hh, hl), level + 1, this);
		_childNodes[7] = new OctTree(new BoundingBox(Vec3f(nx, ny, nz), hw, hh, hl), level + 1, this);
	}

	int GetObjectPosition(Vec3f pos, float radius)
	{
		int index = -1;

		float objTop = pos.y + radius;
		float objBottom = pos.y - radius;

		float objRight = pos.x + radius;
		float objLeft = pos.x - radius;

		float objFront = pos.z + radius;
		float objBack = pos.z - radius;

		if (objBack > _region->_center.z)
		{
			//Can be 0,1,2,3
			if (objLeft < _region->_center.x)
			{
				//Can be 0,2
				if (objTop < _region->_center.y)
				{
					//Is 0
					index = 0;
				}
				else if (objBottom > _region->_center.y)
				{
					//Is 2
					index = 2;
				}
			}
			else if(objRight > _region->_center.x)
			{
				//Can be 1,3
				if (objTop < _region->_center.y)
				{
					//Is 1
					index = 1;
				}
				else if(objBottom > _region->_center.y)
				{
					//Is 3
					index = 3;
				}
			}
		}
		else if(objFront < _region->_center.z)
		{
			//Can be 4,5,6,7
			if (objLeft < _region->_center.x)
			{
				//Can be 4,6
				if (objTop < _region->_center.y)
				{
					//Is 4
					index = 4;
				}
				else if (objTop > _region->_center.y)
				{
					//Is 6
					index = 6;
				}
			}
			else if (objRight > _region->_center.x)
			{
				//Can be 5,7
				if (objTop < _region->_center.y)
				{
					//Is 5
					index = 5;
				}
				else if (objBottom > _region->_center.y)
				{
					//Is 7
					index = 7;
				}
			}
		}

		return index;
	}
	
};
#define M_PI 3.141592653589793

GameObject Compare(GameObject g1, GameObject g2)
{
	if(g1.bounds._point.z > g2.bounds._point.z)
	{
		return g1;
	}
	return g2;
}

GameObject Compare(std::vector<GameObject> objs)
{
	GameObject obj = objs[0];
	for (int i = 1; i < objs.size(); i++)
	{
		obj = Compare(obj, objs[i]);
	}
	return obj;
}

std::vector<GameObject> GetIntersectedNodes(const Vec3f rayOrig, Vec3f rayDir, OctTree* tree)
{
	std::vector<GameObject> nodes;

	for (int i = 0; i < 4; i++)
	{
		if (tree->_childNodes[i]->_region->Intersect(rayOrig, rayDir, 0.1))
		{
			if (tree->_childNodes[i]->_objects.size() != 0)
			{
				nodes.insert(nodes.end(), tree->_childNodes[i]->_objects.begin(), tree->_childNodes[i]->_objects.end());
			}
			//Check Children Childrens
			if (tree->_childNodes[i]->_childNodes[0] != nullptr)
			{
				std::vector<GameObject> childenodes = GetIntersectedNodes(Vec3f(0), rayDir, tree->_childNodes[i]);
				nodes.insert(nodes.end(), childenodes.begin(), childenodes.end());
			}
			else
			{
				//Check behind nodes
				//Check level for detail of intersection
				if (tree->level < 2)
				{
					//Check parallel node
					switch (i)
					{
					case 0: //Check 4
					{
						if (tree->_childNodes[4]->_region->Intersect(rayOrig, rayDir, 0.1))
						{
							if (tree->_childNodes[4]->_objects.size() != 0)
							{
								nodes.insert(nodes.end(), tree->_childNodes[4]->_objects.begin(), tree->_childNodes[4]->_objects.end());
							}
							else
							{
								if (tree->_childNodes[4]->_childNodes[0] != nullptr)
								{
									std::vector<GameObject> childenodes = GetIntersectedNodes(Vec3f(0), rayDir, tree->_childNodes[4]);
									nodes.insert(nodes.end(), childenodes.begin(), childenodes.end());
								}
							}
						}
						break;
					}
					case 1:  //Check 5
					{
						if (tree->_childNodes[5]->_region->Intersect(rayOrig, rayDir, 0.1))
						{
							if (tree->_childNodes[5]->_objects.size() != 0)
							{
								nodes.insert(nodes.end(), tree->_childNodes[5]->_objects.begin(), tree->_childNodes[5]->_objects.end());
							}
							else
							{
								if (tree->_childNodes[5]->_childNodes[0] != nullptr)
								{
									std::vector<GameObject> childenodes = GetIntersectedNodes(Vec3f(0), rayDir, tree->_childNodes[5]);
									nodes.insert(nodes.end(), childenodes.begin(), childenodes.end());
								}
							}
						}
						break;
					}
					case 2: //Check 6
					{
						if (tree->_childNodes[6]->_region->Intersect(rayOrig, rayDir, 0.1))
						{
							if (tree->_childNodes[6]->_objects.size() != 0)
							{
								nodes.insert(nodes.end(), tree->_childNodes[6]->_objects.begin(), tree->_childNodes[6]->_objects.end());
							}
							else
							{
								if (tree->_childNodes[6]->_childNodes[0] != nullptr)
								{
									std::vector<GameObject> childenodes = GetIntersectedNodes(Vec3f(0), rayDir, tree->_childNodes[6]);
									nodes.insert(nodes.end(), childenodes.begin(), childenodes.end());
								}
							}
						}
						break;
					}
					case 3:	//Check 7
					{
						if (tree->_childNodes[7]->_region->Intersect(rayOrig, rayDir, 0.1))
						{
							if (tree->_childNodes[7]->_objects.size() != 0)
							{
								nodes.insert(nodes.end(), tree->_childNodes[7]->_objects.begin(), tree->_childNodes[7]->_objects.end());
							}
							else
							{
								if (tree->_childNodes[5]->_childNodes[0] != nullptr)
								{
									std::vector<GameObject> childenodes = GetIntersectedNodes(Vec3f(0), rayDir, tree->_childNodes[7]);
									nodes.insert(nodes.end(), childenodes.begin(), childenodes.end());
								}
							}
						}
						break;
					}
					default:
						break;
					}
				}
				else
				{
					//Check all nodes
				}
				
			}
		}
	}
	
	
	return nodes;
}

GameObject GetClosestObject(const Vec3f rayOrig, Vec3f rayDir, OctTree* tree)
{
	GameObject closest; //TopLeft
	std::vector<GameObject> objectsInNodes;
	if (tree->_objects.size() != 0)
	{
		objectsInNodes.insert(objectsInNodes.end(), tree->_objects.begin(), tree->_objects.end());
	}
	if (tree->_childNodes[0] != nullptr)
	{
		std::vector<GameObject> childrenNodes = GetIntersectedNodes(rayOrig, rayDir, tree);
		objectsInNodes.insert(objectsInNodes.end(), childrenNodes.begin(), childrenNodes.end());
	}

	if (objectsInNodes.size() > 0)
	{
		if (objectsInNodes.size() > 1)
		{
			closest = Compare(objectsInNodes);
		}
		else
		{
			closest = objectsInNodes[0];
		}
	}
	
	return closest;
}

int main()
{
	OctTree tree = OctTree(16);
	
	tree.Add(GameObject(Vec3f(2,2,-12), 1, "FBL - Front Bottom Left")); 
	tree.Add(GameObject(Vec3f(2, 2, -13), 1, "FBL - Front Bottom Left Behind"));
	tree.Add(GameObject(Vec3f(10,10,-20), 1, "BTR - Back Top Rigth"));  
	tree.Add(GameObject(Vec3f(10,2,-12), 1, "FTR - Front Bottom Rigth")); 


	unsigned width = 640, height = 480;
	float invWidth = 1 / float(width), invHeight = 1 / float(height);
	float fov = 30, aspectratio = width / float(height);
	float angle = tan(M_PI * 0.5 * fov / 180.);

	float xx = (2 * ((0 + 0.5) * invWidth) - 1) * angle * aspectratio;
	float yy = (1 - 2 * ((0 + 0.5) * invHeight)) * angle;

	Vec3f raydirTopLeft(xx, yy, -1);
	raydirTopLeft.normalize();

	float xx1 = (2 * ((640 + 0.5) * invWidth) - 1) * angle * aspectratio;
	float yy1 = (1 - 2 * ((0 + 0.5) * invHeight)) * angle;

	Vec3f raydirTopRight(xx1, yy1, -1);
	raydirTopRight.normalize();

	float xx2 = (2 * ((0 + 0.5) * invWidth) - 1) * angle * aspectratio;
	float yy2 = (1 - 2 * ((640 + 0.5) * invHeight)) * angle;

	Vec3f BottomLeft(xx2, yy2, -1);
	BottomLeft.normalize();


	tree.Add(GameObject(Vec3f(8, 8, -11), 5, "RootObject"));
	GameObject Empry = GetClosestObject(Vec3f(8,8,5), raydirTopLeft, &tree); // Have none
	GameObject BTR = GetClosestObject(Vec3f(8, 8, 5), raydirTopRight, &tree); // Have Back Top Right
	GameObject FBL = GetClosestObject(Vec3f(8, 8, 5), BottomLeft, &tree); // Front Bottom Left

	GameObject RootObject = GetClosestObject(Vec3f(8, 8, 5), raydirTopLeft, &tree); // Have none

	return 0;
}
