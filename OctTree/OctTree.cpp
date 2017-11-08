// OctTree.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <vector>
#include <DirectXCollision.h>


class GameObjects
{
public:
	GameObjects() {

	}
};

using namespace DirectX;
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
		//_region = new BoundingBox(Vector3f(1,1,1), Vector3();
	}
	OctTree(BoundingBox* region, std::vector<GameObjects>* objects)
	{
		_region = region;
		_objects = objects;

	}
	

	void BuildTree()
	{
		
	}
};

int main()
{

    return 0;
}

