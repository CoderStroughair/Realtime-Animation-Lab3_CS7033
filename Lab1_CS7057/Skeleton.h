#pragma once
#include "Utilities.h"

class Bone {
private:
	Mesh mesh;
	Bone* parent;
	vector<Bone*> children;
	mat4 model;
public:
	void addChild(Bone* bone)
	{
		children.push_back(bone);
		bone->setParent(this);
	}
	void setParent(Bone* bone)
	{
		parent = bone;
	}
};

class Skeleton {
private:
	Mesh root;
	vector<Bone*> children;
public:
	Skeleton(Mesh& _root) 
	{
		root = _root;
	};
	void addBone(Bone& b)
	{
		children.push_back(&b);
	};

	mat4 model;
};
