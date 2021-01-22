#pragma once
#include "state.h"

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

class HelpState : public State
{
public:
	HelpState(gef::Platform * platform,
			  gef::InputManager * input_manager,
			  gef::AudioManager * audio_manager,
			  gef::Renderer3D * renderer_3D,
			  gef::SpriteRenderer * sprite_renderer,
			  gef::Font * font,
			  StateMachine * state_machine);
	~HelpState();

protected:
	void Init() override;
	bool HandleInput() override;
	void Update(float delta_time) override;
	void Render() override;
	void Release() override;

	gef::Sprite background_sprite_;
	gef::Texture* background_texture_;
};

