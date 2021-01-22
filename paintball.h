#pragma once
#include "game_object.h"

class Paintball : public GameObject
{
public:
	Paintball();
	~Paintball();

	void Update(float delta_time);

	//gef::Matrix44 spawn_transform_;

	gef::Vector4 position_;
	gef::Vector4 velocity_;
	gef::Vector4 acceleration_;

	bool alive_;


};

