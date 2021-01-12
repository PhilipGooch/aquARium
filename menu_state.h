#pragma once
#include "state.h"

#include <maths\matrix44.h>
#include <graphics\sprite.h>
#include <platform/vita/graphics/texture_vita.h>

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

class MenuState : public State
{
public:
	MenuState(gef::Platform * platform,
			  gef::InputManager * input_manager,
			  gef::AudioManager * audio_manager,
			  gef::Renderer3D * renderer_3D,
			  gef::SpriteRenderer * sprite_renderer,
			  gef::Font * font,
			  StateMachine * state_machine);
	~MenuState();

protected:
	bool HandleInput() override;
	void Update(float delta_time) override;
	void Render() override;

	gef::Matrix44 projection_matrix_;
	gef::Matrix44 view_matrix_;
	gef::Matrix44 ortho_matrix_;

	gef::Sprite background_sprite_;
	gef::Sprite indicator_sprite_;

	int selection_;
};

