#include "state_machine.h"
#include <input/input_manager.h>
#include "game_state.h"
#include "menu_state.h"
#include "help_state.h"
#include "win_state.h"

StateMachine::StateMachine(gef::Platform * platform, 
						   gef::InputManager * input_manager, 
						   gef::AudioManager * audio_manager, 
						   gef::Renderer3D * renderer_3D,
						   gef::SpriteRenderer * sprite_renderer, 
						   gef::Font * font)
{
	game_state_ = new GameState(platform, input_manager, audio_manager, renderer_3D, sprite_renderer, font, this);
	menu_state_ = new MenuState(platform, input_manager, audio_manager, renderer_3D, sprite_renderer, font, this);
	help_state_ = new HelpState(platform, input_manager, audio_manager, renderer_3D, sprite_renderer, font, this);
	win_state_ = new WinState(platform, input_manager, audio_manager, renderer_3D, sprite_renderer, font, this);

	SetState(STATE::MENU);
}

StateMachine::~StateMachine()
{
}

void StateMachine::SetState(STATE state)
{
	if (state_)
	{
		state_->Release();
	}

	switch (state)
	{
	case STATE::GAME:
		state_ = game_state_;
		break;
	case STATE::MENU:
		state_ = menu_state_;
		break;
	case STATE::HELP:
		state_ = help_state_;
		break;
	case STATE::WIN:
		state_ = win_state_;
		break;
	}

	state_->Init();
}
