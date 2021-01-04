#include "game_object.h"

GameObject::GameObject()
{
	local_transform_.SetIdentity();
	offset_transform_.SetIdentity();
	parent_transform_.SetIdentity();
	world_transform_.SetIdentity();
}

GameObject::~GameObject()
{
}
