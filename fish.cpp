#include "fish.h"

#include <random>
#include "graphics/mesh_instance.h"


Fish::Fish() :
	Boid(),
	body_angle_(0.0f),
	tail_angle_(0.0f),
	delta_angle_(0.035f),
	max_angle_(10.0f * 3.1415f / 180)
{
	body_angle_ = (rand() % 10) * 3.1415f / 180;
	tail_angle_ = -body_angle_ * 1.5f;
}

Fish::~Fish()
{
}

void Fish::Animate(float delta_time_)
{
	body_angle_ += delta_angle_;
	tail_angle_ -= delta_angle_ * 1.5f;
	if (body_angle_ > max_angle_ || body_angle_ < -max_angle_)
	{
		delta_angle_ = -delta_angle_;
	}

	gef::Matrix44 scale;
	scale.SetIdentity();
	scale.Scale(gef::Vector4(0.001f, 0.001f, 0.001f));

	gef::Matrix44 rotation_x;
	rotation_x.SetIdentity();
	rotation_x.RotationX(3.1415);

	gef::Matrix44 body_rotation_z;
	body_rotation_z.SetIdentity();
	body_rotation_z.RotationZ(body_angle_);

	gef::Matrix44 body_rotation;
	body_rotation.SetIdentity();
	body_rotation = body_rotation_z;

	gef::Matrix44 body_translation;
	body_translation.SetIdentity();
	body_translation.SetTranslation(gef::Vector4(0, 0, 0));

	gef::Matrix44 body_transformation;
	body_transformation = body_rotation * body_translation;

	gef::Matrix44 tail_rotation_z;
	tail_rotation_z.SetIdentity();
	tail_rotation_z.RotationZ(tail_angle_);

	gef::Matrix44 tail_rotation;
	tail_rotation.SetIdentity();
	tail_rotation = tail_rotation_z;

	gef::Matrix44 tail_translation;
	tail_translation.SetIdentity();
	tail_translation.SetTranslation(gef::Vector4(-8, 0, 0));

	body_transform_ = scale * rotation_x * body_rotation * body_translation * world_transform_;

	tail_transform_ = tail_rotation * tail_translation * scale * body_rotation * world_transform_;
}
