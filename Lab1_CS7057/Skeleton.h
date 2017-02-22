#pragma once
#include "Utilities.h"

#pragma region BONE
class Bone {
public:
	Mesh mesh;
	versor orientation;
	mat4 rotMatrix;
	mat4 localTransform;
	Bone* parent;
	vector<Bone*> children;
	bool root = false;
	Bone();
	Bone(Mesh m, Bone* parent, mat4 initPosition);
	Bone(Mesh m, Bone* parent, mat4 initPosition, bool root);
	void addChild(Bone* child);
	void drawBone(mat4 projection, mat4 view, mat4 modelGlobal, GLuint shaderID, EulerCamera cam);
	void bendBone(GLfloat rotation);
	void rollBone(GLfloat rotation);
	void pivotBone(GLfloat rotation);
	bool initialised = false;

private:
	vec4 up, right, forward;
};

#pragma region CONSTRUCTORS

Bone::Bone()
{
	initialised = false;
}

Bone::Bone(Mesh m, Bone* parent, mat4 initPosition)
{
	initialised = true;
	mesh = m;
	localTransform = initPosition;
	this->parent = parent;
	if (parent)
		parent->addChild(this);
	up = vec4(0.0, 1.0, 0.0, 0.0);
	right = vec4(1.0, 0.0, 0.0, 0.0);
	forward = vec4(0.0, 0.0, 1.0, 0.0);
	orientation = quat_from_axis_rad(0, this->right.v[0], this->right.v[1], this->right.v[2]);
	rotMatrix = identity_mat4();
}

Bone::Bone(Mesh m, Bone* parent, mat4 initPosition, bool root)
{
	if (root)
		this->root = true;
	initialised = true;
	mesh = m;
	localTransform = initPosition;
	this->parent = parent;
	if (parent)
		parent->addChild(this);
	up = vec4(0.0, 1.0, 0.0, 0.0);
	right = vec4(1.0, 0.0, 0.0, 0.0);
	forward = vec4(0.0, 0.0, 1.0, 0.0);
	orientation = quat_from_axis_rad(0, this->right.v[0], this->right.v[1], this->right.v[2]);
	rotMatrix = identity_mat4();
}

#pragma endregion

void Bone::addChild(Bone* child)
{
	children.push_back(child);
}

void Bone::drawBone(mat4 projection, mat4 view, mat4 modelGlobal, GLuint shaderID, EulerCamera cam)
{
	mat4 modelLocal = modelGlobal * localTransform * rotMatrix;

	// light properties
	vec3 Ls = vec3(0.0001f, 0.0001f, 0.0001f);	//Specular Reflected Light
	vec3 Ld = vec3(0.5f, 0.5f, 0.5f);	//Diffuse Surface Reflectance
	vec3 La = vec3(1.0f, 1.0f, 1.0f);	//Ambient Reflected Light
	vec3 light = vec3(5, 10, -5.0f);//light source location
	vec3 coneDirection = light + vec3(0.0f, -1.0f, 0.0f);
	float coneAngle = 10.0f;
	// object colour
	vec3 Ks = vec3(0.1f, 0.1f, 0.1f); // specular reflectance
	vec3 Kd = BLUE;
	vec3 Ka = BLUE; // ambient reflectance
	float specular_exponent = 0.000000005f; //specular exponent - size of the specular elements

	drawObjectDebug(shaderID, view, projection, modelLocal, light, Ls, La, Ld, Ks, Kd, Ka, specular_exponent, cam, mesh, coneAngle, coneDirection, GL_TRIANGLES);
	
	
	for (GLuint i = 0; i < this->children.size(); i++)
	{
		this->children[i]->drawBone(projection, view, modelLocal, shaderID, cam);
	}
}

#pragma region ROTATION

void Bone::bendBone(GLfloat rotation)
{
	versor quat = quat_from_axis_rad(rotation, this->right.v[0], this->right.v[1], this->right.v[2]);
	orientation = orientation * quat;
	this->rotMatrix = quat_to_mat4(this->orientation);
	this->forward = this->rotMatrix * vec4(1.0f, 0.0f, 0.0f, 0.0f);
	this->right = this->rotMatrix * vec4(0.0f, 0.0f, 1.0f, 0.0f);
	this->up = this->rotMatrix * vec4(0.0f, 1.0f, 0.0f, 0.0f);
}

void Bone::rollBone(GLfloat rotation)
{
	versor quat = quat_from_axis_rad(rotation, this->forward.v[0], this->forward.v[1], this->forward.v[2]);
	orientation = orientation * quat;
	this->rotMatrix = quat_to_mat4(this->orientation);
	this->forward = this->rotMatrix * vec4(1.0f, 0.0f, 0.0f, 0.0f);
	this->right = this->rotMatrix * vec4(0.0f, 0.0f, 1.0f, 0.0f);
	this->up = this->rotMatrix * vec4(0.0f, 1.0f, 0.0f, 0.0f);
}

void Bone::pivotBone(GLfloat rotation)
{
	versor quat = quat_from_axis_rad(rotation, this->up.v[0], this->up.v[1], this->up.v[2]);
	orientation = orientation * quat;
	this->rotMatrix = quat_to_mat4(this->orientation);
	this->forward = this->rotMatrix * vec4(1.0f, 0.0f, 0.0f, 0.0f);
	this->right = this->rotMatrix * vec4(0.0f, 0.0f, 1.0f, 0.0f);
	this->up = this->rotMatrix * vec4(0.0f, 1.0f, 0.0f, 0.0f);
}

#pragma endregion

#pragma endregion

class Skeleton {
public:
	Skeleton();
	Skeleton(Mesh mRoot, Mesh mFinger);
	void drawSkeleton(mat4 view, mat4 projection, GLuint shaderID, EulerCamera cam);
	void bendFingers();

	Bone* list[15];
	Bone* root;
	float radians = 0;
	bool bend = true;
};

Skeleton::Skeleton(){}

Skeleton::Skeleton(Mesh mRoot, Mesh mFinger)
{
	root = new Bone(mRoot, nullptr, identity_mat4(), true);

	//int x = 0;
	//int y = x + 3;

	//for (int z = 0; z < 5; z++)
	//{
	//	mat4 matrix
	//}
	//Finger 1
	mat4 matrix = translate(identity_mat4(), vec3(2.0*cos(ONE_DEG_IN_RAD * 0), 2.0*sin(ONE_DEG_IN_RAD * 0), 0.0));
	list[0] = new Bone(mFinger, root, matrix);
	for (int i = 1; i < 3; i++)
	{
		matrix = translate(identity_mat4(), vec3(1.2, 0.0, 0.0));
		list[i] = new Bone(mFinger, list[i-1], matrix);
	}

	//Finger 2
	matrix = rotate_z_deg(identity_mat4(), 45);
	matrix = translate(matrix, vec3(2.0*cos(ONE_DEG_IN_RAD*45), 2.0*sin(ONE_DEG_IN_RAD*45), 0.0));
	list[3] = new Bone(mFinger, root, matrix);
	for (int i = 4; i < 6; i++)
	{
		matrix = translate(identity_mat4(), vec3(1.5, 0.0, 0.0));
		list[i] = new Bone(mFinger, list[i - 1], matrix);
	}

	//Finger 3
	matrix = rotate_z_deg(identity_mat4(), 75);
	matrix = translate(matrix, vec3(2.0*cos(ONE_DEG_IN_RAD * 75), 2.0*sin(ONE_DEG_IN_RAD * 75), 0.0));
	list[6] = new Bone(mFinger, root, matrix);
	for (int i = 7; i < 9; i++)
	{
		matrix = translate(identity_mat4(), vec3(1.5, 0.0, 0.0));
		list[i] = new Bone(mFinger, list[i - 1], matrix);
	}

	//Finger 4
	matrix = rotate_z_deg(identity_mat4(), 115);
	matrix = translate(matrix, vec3(2.0*cos(ONE_DEG_IN_RAD * 115), 2.0*sin(ONE_DEG_IN_RAD * 115), 0.0));
	list[9] = new Bone(mFinger, root, matrix);
	for (int i = 10; i < 12; i++)
	{
		matrix = translate(identity_mat4(), vec3(1.5, 0.0, 0.0));
		list[i] = new Bone(mFinger, list[i - 1], matrix);
	}

	//Finger 5
	matrix = rotate_z_deg(identity_mat4(), 140);
	matrix = translate(matrix, vec3(2.0*cos(ONE_DEG_IN_RAD * 140), 2.0*sin(ONE_DEG_IN_RAD * 140), 0.0));
	list[12] = new Bone(mFinger, root, matrix);
	for (int i = 13; i < 15; i++)
	{
		matrix = translate(identity_mat4(), vec3(1.5, 0.0, 0.0));
		list[i] = new Bone(mFinger, list[i - 1], matrix);
	}
}

void Skeleton::drawSkeleton(mat4 view, mat4 projection, GLuint shaderID, EulerCamera cam)
{
	root->drawBone(projection, view, identity_mat4(), shaderID, cam);
}

void Skeleton::bendFingers()
{
	if (bend)
	{
		radians += 0.01;
		for (int i = 0; i < 15; i++)
		{
			if (list[i]->root)
				cout << "";
			list[i]->pivotBone(0.01);
		}
		if (radians > 0.0)
		{
			bend = false;
		}
	}
	else
	{
		radians -= 0.01;
		for (int i = 0; i < 15; i++)
		{
			if (list[i]->root)
				cout << "";
			list[i]->pivotBone(-0.01);
		}
		if (radians <= -1.0)
		{
			bend = true;
		}
	}
}