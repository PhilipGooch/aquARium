#include "options_state.h"

#include "state_machine.h"

OptionsState::OptionsState(gef::Platform * platform,
						   gef::InputManager * input_manager,
						   gef::AudioManager * audio_manager,
						   gef::Renderer3D * renderer_3D,
						   gef::SpriteRenderer * sprite_renderer,
						   gef::Font * font,
						   StateMachine * state_machine) :
	State(platform, input_manager, audio_manager, renderer_3D, sprite_renderer, font, state_machine)
{
}


OptionsState::~OptionsState()
{
}

bool OptionsState::HandleInput()
{
	return false;
}

void OptionsState::Update(float delta_time)
{
}

void OptionsState::Render()
{
}
