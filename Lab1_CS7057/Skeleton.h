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

class Hand {
public:
	Hand();
	Hand(Mesh mRoot, Mesh mFinger);
	void drawSkeleton(mat4 view, mat4 projection, GLuint shaderID, EulerCamera cam);
	void formFist();
	void thumbsUp();
	void oneFinger();
	void oneJoint();

	Bone* list[15];

	Bone* index[3];
	Bone* middle[3];
	Bone* ring[3];
	Bone* pinky[3];
	Bone* thumb[3];

	Bone* palm;
	float radians = 0;
};

Hand::Hand(){}

Hand::Hand(Mesh mRoot, Mesh mFinger)
{
	palm = new Bone(mRoot, nullptr, identity_mat4(), true);

	//Finger 1
	mat4 matrix = rotate_z_deg(identity_mat4(), -15);
	matrix = translate(matrix, vec3(2.0*cos(ONE_DEG_IN_RAD * -15), 2.0*sin(ONE_DEG_IN_RAD * -15), 0.0));
	list[0] = new Bone(mFinger, palm, matrix);
	thumb[0] = list[0];
	for (int i = 1; i < 3; i++)
	{
		matrix = scale(identity_mat4(), vec3(0.9, 0.9, 0.9));
		matrix = translate(matrix, vec3(1.5, 0.0, 0.0));
		list[i] = new Bone(mFinger, list[i-1], matrix);
		thumb[i] = list[i];
	}

	//Finger 2
	matrix = rotate_z_deg(identity_mat4(), 45);
	matrix = translate(matrix, vec3(2.0*cos(ONE_DEG_IN_RAD*45), 2.0*sin(ONE_DEG_IN_RAD*45), 0.0));
	list[3] = new Bone(mFinger, palm, matrix);
	index[0] = list[3];
	for (int i = 4; i < 6; i++)
	{
		matrix = scale(identity_mat4(), vec3(0.9, 0.9, 0.9));
		matrix = translate(matrix, vec3(1.5, 0.0, 0.0));
		list[i] = new Bone(mFinger, list[i - 1], matrix);
		index[i - 3] = list[i];
	}

	//Finger 3
	matrix = rotate_z_deg(identity_mat4(), 75);
	matrix = translate(matrix, vec3(2.0*cos(ONE_DEG_IN_RAD * 75), 2.0*sin(ONE_DEG_IN_RAD * 75), 0.0));
	list[6] = new Bone(mFinger, palm, matrix);
	middle[0] = list[6];
	for (int i = 7; i < 9; i++)
	{
		matrix = scale(identity_mat4(), vec3(0.9, 0.9, 0.9));
		matrix = translate(matrix, vec3(1.5, 0.0, 0.0));
		list[i] = new Bone(mFinger, list[i - 1], matrix);
		middle[i - 6] = list[i];
	}

	//Finger 4
	matrix = rotate_z_deg(identity_mat4(), 115);
	matrix = translate(matrix, vec3(2.0*cos(ONE_DEG_IN_RAD * 115), 2.0*sin(ONE_DEG_IN_RAD * 115), 0.0));
	list[9] = new Bone(mFinger, palm, matrix);
	ring[0] = list[9];
	for (int i = 10; i < 12; i++)
	{
		matrix = scale(identity_mat4(), vec3(0.9, 0.9, 0.9));
		matrix = translate(matrix, vec3(1.5, 0.0, 0.0));
		list[i] = new Bone(mFinger, list[i - 1], matrix);
		ring[i - 9] = list[i];
	}

	//Finger 5
	matrix = rotate_z_deg(identity_mat4(), 140);
	matrix = translate(matrix, vec3(2.0*cos(ONE_DEG_IN_RAD * 140), 2.0*sin(ONE_DEG_IN_RAD * 140), 0.0));
	list[12] = new Bone(mFinger, palm, matrix);
	pinky[0] = list[12];
	for (int i = 13; i < 15; i++)
	{
		matrix = scale(identity_mat4(), vec3(0.9, 0.9, 0.9));
		matrix = translate(matrix, vec3(1.5, 0.0, 0.0));
		list[i] = new Bone(mFinger, list[i - 1], matrix);
		pinky[i - 12] = list[i];
	}
}

void Hand::drawSkeleton(mat4 view, mat4 projection, GLuint shaderID, EulerCamera cam)
{
	palm->drawBone(projection, view, identity_mat4(), shaderID, cam);
}

void Hand::formFist()
{
	static bool bend = true;
	if (bend)
	{
		radians += 0.01;
		for (int i = 0; i < 15; i++)
		{
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
			list[i]->pivotBone(-0.01);
		}
		if (radians <= -1.2)
		{
			bend = true;
		}
	}
}

void Hand::thumbsUp()
{
	static bool bending = true;
	if (bending)
	{
		radians += 0.01;
		for (int i = 0; i < 3; i++)
		{
			index[i]->pivotBone(0.01);
			middle[i]->pivotBone(0.01);
			ring[i]->pivotBone(0.01);
			pinky[i]->pivotBone(0.01);
			palm->bendBone(-0.0057);
		}
		if (radians >= 0.0)
		{
			bending = false;
		}
	}
	else
	{
		radians -= 0.01;
		for (int i = 0; i < 3; i++)
		{
			index[i]->pivotBone(-0.01);
			middle[i]->pivotBone(-0.01);
			ring[i]->pivotBone(-0.01);
			pinky[i]->pivotBone(-0.01);
			palm->bendBone(0.0057);
		}
		if (radians <= -1.0)
		{
			bending = true;
		}
	}
}

void Hand::oneFinger()
{
	static bool bend = true;

	if (bend)
	{
		radians += 0.01;
		for (int i = 0; i < 3; i++)
		{
			thumb[i]->pivotBone(0.01);
		}
		if (radians > 0.0)
		{
			bend = false;
		}
	}
	else
	{
		radians -= 0.01;
		for (int i = 0; i < 3; i++)
		{
			thumb[i]->pivotBone(-0.01);
		}
		if (radians <= -1.0)
		{
			bend = true;
		}
	}
}

void Hand::oneJoint()
{
	static bool bend = true;

	if (bend)
	{
		radians += 0.01;
		thumb[0]->pivotBone(0.01);
		if (radians > 0.0)
		{
			bend = false;
		}
	}
	else
	{
		radians -= 0.01;
		thumb[0]->pivotBone(-0.01);
		if (radians <= -1.0)
		{
			bend = true;
		}
	}
}