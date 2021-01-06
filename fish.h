#pragma once
#include "boid.h"

namespace gef
{
	class MeshInstance;
}

class Fish : public Boid
{
public:
	Fish(gef::MeshInstance* body, gef::MeshInstance* tail);
	~Fish();

	inline gef::Matrix44 GetBodyTransform() { return body_transform_; }
	inline gef::Matrix44 GetTailTransform() { return tail_transform_; }

	void Animate(float delta_time_);

	bool alive_ = true;

private:
	gef::MeshInstance* body_;
	gef::MeshInstance* tail_;

	gef::Matrix44 body_transform_;
	gef::Matrix44 tail_transform_;

	float body_angle_;
	float tail_angle_;
	float delta_angle_;
	float max_angle_;

};