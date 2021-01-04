#pragma once
#include "boid.h"

namespace gef
{
	class MeshInstance;
}

class Bee : public Boid
{
public:
	Bee(gef::MeshInstance* body, gef::MeshInstance* right_wing, gef::MeshInstance* left_wing);
	~Bee();

	inline gef::Matrix44 GetBodyTransform() { return body_transform_; }
	inline gef::Matrix44 GetRightWingTransform() { return right_wing_transform_; }
	inline gef::Matrix44 GetLeftWingTransform() { return left_wing_transform_; }

	void Animate(float delta_time_);

private:
	gef::MeshInstance* body_;
	gef::MeshInstance* right_wing_;
	gef::MeshInstance* left_wing_;

	gef::Matrix44 body_transform_;
	gef::Matrix44 right_wing_transform_;
	gef::Matrix44 left_wing_transform_;

	float wing_angle_;
	float delta_wing_angle_;
};