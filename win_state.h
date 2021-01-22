#pragma once
#include "state.h"

#include <graphics\sprite.h>
#include <platform/vita/graphics/texture_vita.h>
#include <graphics/mesh_instance.h>
#include <vector>

namespace gef
{
	class Platform;
	class InputManager;
	class AudioManager;
	class SpriteRenderer;
	class Renderer3D;
	class Font;
}
class StateMachine;
class Boid;

class WinState : public State
{
public:
	WinState(gef::Platform * platform,
		gef::InputManager * input_manager,
		gef::AudioManager * audio_manager,
		gef::Renderer3D * renderer_3D,
		gef::SpriteRenderer * sprite_renderer,
		gef::Font * font,
		StateMachine * state_machine);
	~WinState();

protected:
	void Init() override;
	bool HandleInput() override;
	void Update(float delta_time) override;
	void Render() override;
	void Release() override;

	gef::Sprite background_sprite_;

	gef::Texture* background_texture_;

	gef::MeshInstance fish_body_orange_mesh_instance_;
	gef::MeshInstance fish_tail_orange_mesh_instance_;

	int number_of_fishes_ = 1;
	std::vector<Boid*> fishes_;
};

