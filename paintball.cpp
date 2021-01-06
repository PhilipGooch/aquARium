#include "paintball.h"



Paintball::Paintball()
{
	position_ = gef::Vector4(0.0f, 0.0f, 0.0f);
	velocity_ = gef::Vector4(0.0f, 0.0f, -1.0f);
	acceleration_ = gef::Vector4(0.0f, 0.0f, 0.0f);

	alive_ = true;
}

Paintball::~Paintball()
{
}

void Paintball::Update(float delta_time)
{
	position_ += velocity_ * delta_time;

	gef::Matrix44 translation;
	translation.SetIdentity();
	translation.SetTranslation(position_);

	local_transform_ = translation;

	gef::Matrix44 inverse_offset_transform;
	inverse_offset_transform.AffineInverse(offset_transform_);

	gef::Matrix44 inverse_parent_transform;
	inverse_parent_transform.AffineInverse(parent_transform_);

	world_transform_ = local_transform_ * offset_transform_ * parent_transform_;
}
