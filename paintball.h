#pragma once
#include "game_object.h"

class Paintball : public GameObject
{
public:
	Paintball();
	~Paintball();

	void Update(float delta_time);

	gef::Vector4 position_;
	gef::Vector4 velocity_;
	gef::Vector4 acceleration_;


};

