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
	Bone();
	Bone(Mesh m, Bone* parent, mat4 initPosition);
	void addChild(Bone* child);
	void drawBone(mat4 projection, mat4 view, mat4 modelGlobal, GLuint shaderID, EulerCamera cam);
	void bendBone(GLfloat rotation);
	void rollBone(GLfloat rotation);
	void pivotBone(GLfloat rotation);

private:
	vec4 up, right, forward;
};

#pragma region CONSTRUCTORS

Bone::Bone()
{

}

Bone::Bone(Mesh m, Bone* parent, mat4 initPosition)
{
	localTransform = initPosition;
	this->parent = parent;
	up = vec4(0.0, 1.0, 0.0, 0.0);
	right = vec4(1.0, 0.0, 0.0, 0.0);
	forward = vec4(0.0, 0.0, 1.0, 0.0);
}

#pragma endregion

void Bone::addChild(Bone* child)
{
	children.push_back(child);
}

void Bone::drawBone(mat4 projection, mat4 view, mat4 modelGlobal, GLuint shaderID, EulerCamera cam)
{
	mat4 modelLocal = modelGlobal * localTransform * rotMatrix;

	//draw the mesh here
	// light properties
	vec3 Ls = vec3(0.0001f, 0.0001f, 0.0001f);	//Specular Reflected Light
	vec3 Ld = vec3(0.5f, 0.5f, 0.5f);	//Diffuse Surface Reflectance
	vec3 La = vec3(1.0f, 1.0f, 1.0f);	//Ambient Reflected Light
	vec3 light = vec3(5 * sin(0), 10, -5.0f*cos(0));//light source location
	vec3 coneDirection = light + vec3(0.0f, -1.0f, 0.0f);
	float coneAngle = 10.0f;
	// object colour
	vec3 Ks = vec3(0.1f, 0.1f, 0.1f); // specular reflectance
	vec3 Kd = BLUE;
	vec3 Ka = BLUE; // ambient reflectance
	float specular_exponent = 0.000000005f; //specular exponent - size of the specular elements

	drawObject(shaderID, view, projection, modelLocal, light, Ls, La, Ld, Ks, vec3(1.0, 0.0, 1.0), vec3(0.8, 0.0, 0.8), specular_exponent, cam, this->mesh, coneAngle, coneDirection, GL_TRIANGLES);
	
	
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
	Skeleton(Mesh m);
	void drawSkeleton(mat4 view, mat4 projection, GLuint shaderID, EulerCamera cam);

	vector<Bone*> bones;
	Bone root;
};

Skeleton::Skeleton(){}

Skeleton::Skeleton(Mesh m)
{
	root = Bone(m, nullptr, identity_mat4());
	for (int i = 0; i < 15; i++)
	{
		mat4 matrix = translate(identity_mat4(), vec3(1.0, 1.0, 0.0));
		bones.push_back(&Bone(m, &root, matrix));
	}
}

void Skeleton::drawSkeleton(mat4 view, mat4 projection, GLuint shaderID, EulerCamera cam)
{
	root.drawBone(view, projection, identity_mat4(), shaderID, cam);
}