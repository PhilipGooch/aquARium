#include "bee.h"

#include "graphics\mesh_instance.h"
#include <random>

Bee::Bee(gef::MeshInstance* body, gef::MeshInstance* right_wing, gef::MeshInstance* left_wing) :
	Boid(),
	body_(body),
	right_wing_(right_wing),
	left_wing_(left_wing),
	delta_wing_angle_(0.4f)
{
	wing_angle_ = -(rand() % 40) * 3.1415f / 180;
}


Bee::~Bee()
{
}

void Bee::Animate(float delta_time_)
{
	gef::Matrix44 bee_scale;
	bee_scale.SetIdentity();
	bee_scale.Scale(gef::Vector4(0.0002f, 0.0002f, 0.0002f));

	gef::Matrix44 bee_rotation;
	bee_rotation.SetIdentity();
	bee_rotation.RotationX(3.1415 / 2);

	gef::Matrix44 right_wing_translation;
	right_wing_translation.SetIdentity();
	right_wing_translation.SetTranslation(gef::Vector4(2.0f, 0.15f, 1.0f));

	gef::Matrix44 left_wing_translation;
	left_wing_translation.SetIdentity();
	left_wing_translation.SetTranslation(gef::Vector4(2.0f, 0.15f, -1.0f));

	gef::Matrix44 body_translation;
	body_translation.SetIdentity();

	wing_angle_ += delta_wing_angle_;
	if (wing_angle_ >= 0 || wing_angle_ <= -40 * 3.1415 / 180)
	{
		delta_wing_angle_ = -delta_wing_angle_;
	}

	gef::Matrix44 right_wing_rotation;
	right_wing_rotation.SetIdentity();
	right_wing_rotation.RotationX(wing_angle_);

	gef::Matrix44 left_wing_rotation;
	left_wing_rotation.SetIdentity();
	left_wing_rotation.RotationX(-wing_angle_);

	gef::Matrix44 left_wing_scale;
	left_wing_scale.SetIdentity();
	left_wing_scale.Scale(gef::Vector4(1.0f, 1.0f, -1.0f));

	right_wing_transform_ = right_wing_rotation * right_wing_translation * bee_scale * bee_rotation * world_transform_;

	left_wing_transform_ = left_wing_scale * left_wing_rotation * left_wing_translation * bee_scale * bee_rotation * world_transform_;

	body_transform_ = bee_scale * bee_rotation * body_translation * world_transform_;

	//body_->set_transform(body_transform);
	//right_wing_->set_transform(right_wing_transform);
	//left_wing_->set_transform(left_wing_transform);
}

