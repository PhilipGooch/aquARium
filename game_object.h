#pragma once

#include <maths\matrix44.h>

class GameObject
{
public:
	GameObject();
	~GameObject();

	inline gef::Matrix44 GetLocalTransform() { return local_transform_; }
	inline gef::Matrix44 GetOffsetTransform() { return offset_transform_; }
	inline gef::Matrix44 GetParentTransform() { return parent_transform_; }
	inline gef::Matrix44 GetWorldTransform() { return world_transform_; }

	inline void setLocalTransform(gef::Matrix44 local_transform) { offset_transform_ = local_transform; }
	inline void setOffsetTransform(gef::Matrix44 offset_transform) { offset_transform_ = offset_transform; }
	inline void setParentTransform(gef::Matrix44 parent_transform) { parent_transform_ = parent_transform; }

protected:
	gef::Matrix44 local_transform_;
	gef::Matrix44 offset_transform_;
	gef::Matrix44 parent_transform_;
	gef::Matrix44 world_transform_;
};