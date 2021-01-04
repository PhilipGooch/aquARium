#include "paintball.h"



Paintball::Paintball()
{
	position_ = gef::Vector4(0.0f, 0.0f, 0.0f);
	velocity_ = gef::Vector4(0.0f, 0.0f, -0.2f);
	acceleration_ = gef::Vector4(0.0f, 0.0f, 0.0f);
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

	world_transform_ = local_transform_ * offset_transform_ * parent_transform_;
}
