#pragma once
#include "Utilities.h"

class Bone {
private:
	Mesh mesh;
	Bone* parent;
	vector<Bone*> children;
	versor curr_rotation;
	mat4 model;
public:
	Bone(Mesh m)
	{
		model = identity_mat4();
		mesh = m;
	}
	void addChild(Bone* bone)
	{
		children.push_back(bone);
		bone->setParent(this);
	}
	void setParent(Bone* bone)
	{
		parent = bone;
	}
	void draw(GLuint shaderID, mat4 root)
	{
		if (mesh.tex)
		{
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, mesh.tex);
			glUniform1i(glGetUniformLocation(shaderID, "texture_primary"), 0);

		}
		model = model * root;
		glUniformMatrix4fv(glGetUniformLocation(shaderID, "model"), 1, GL_FALSE, model.m);
		glDrawArrays(GL_TRIANGLES, 0, mesh.mesh_vertex_count);
		for (int i = 0; i < children.size(); i++)
		{
			children[i]->draw(shaderID, model);
		}
	}
	void rotateBone(vec3 rotation)
	{
		versor quat = quat_from_axis_deg(rotation.v[0], 1, 0.0, 0.0);
		quat = quat_from_axis_deg(rotation.v[1], 0.0, 1.0, 0.0)*quat;
		quat = quat_from_axis_deg(rotation.v[2], 0.0, 0.0, 1.0)*quat;
		curr_rotation = quat*curr_rotation;
		model = quat_to_mat4(curr_rotation);
		for (int i = 0; i < children.size(); i++)
		{
			children[i]->rotateBone(rotation);
		}
	}
};

class Skeleton {
private:
	Mesh root;
	vector<Bone*> children;
public:
	Skeleton() {};
	mat4 model;
	Skeleton(Mesh& _root) 
	{
		root = _root;
	};
	void addBone(Bone& b)
	{
		children.push_back(&b);
	};
	void draw(GLuint shaderID)
	{
		if (root.tex)
		{
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, root.tex);
			glUniform1i(glGetUniformLocation(shaderID, "texture_primary"), 0);

		}
		glUniformMatrix4fv(glGetUniformLocation(shaderID, "model"), 1, GL_FALSE, model.m);
		glDrawArrays(GL_TRIANGLES, 0, root.mesh_vertex_count);
		for (int i = 0; i < children.size(); i++)
		{
			children[i]->draw(shaderID, model);
		}
	}
};
