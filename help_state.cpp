#include "help_state.h"

#include <gxm.h>
#include <motion.h>
#include <libdbg.h>
#include <libsmart.h>
#include <sony_sample_framework.h>
#include <sony_tracking.h>

#include <graphics/sprite_renderer.h>
#include <graphics/font.h>
#include <system/platform.h>

#include <assets/png_loader.h>
#include <graphics/image_data.h>
#include <graphics/texture.h>

#include <input/input_manager.h>
#include <input/sony_controller_input_manager.h>

#include "state_machine.h"

HelpState::HelpState(gef::Platform * platform,
				     gef::InputManager * input_manager,
				     gef::AudioManager * audio_manager,
				     gef::Renderer3D * renderer_3D,
				     gef::SpriteRenderer * sprite_renderer,
				     gef::Font * font,
				     StateMachine * state_machine) :
	State(platform, input_manager, audio_manager, renderer_3D, sprite_renderer, font, state_machine)
{
	// SPRITES
	gef::PNGLoader png_loader;
	gef::ImageData image_data;

	background_texture_ = nullptr;
	png_loader.Load("help.png", *platform_, image_data);
	background_texture_ = gef::Texture::Create(*platform_, image_data);

	background_sprite_.set_width(960);
	background_sprite_.set_height(544);
	background_sprite_.set_position(gef::Vector4(960 / 2, 544 / 2, 1.f, 1.f));
	background_sprite_.set_texture(background_texture_);
}


HelpState::~HelpState()
{
	delete background_texture_;
	background_texture_ = nullptr;
}

void HelpState::Init()
{

}

bool HelpState::HandleInput()
{
	if (input_manager_)
	{
		input_manager_->Update();

		gef::SonyControllerInputManager* controller_input = input_manager_->controller_input();

		if (controller_input)
		{
			const gef::SonyController* controller = controller_input->GetController(0);

			if (controller)
			{
				if (controller->buttons_down() & gef_SONY_CTRL_TRIANGLE)
				{
					state_machine_->SetState(StateMachine::MENU);
				}
			}
		}
	}
	return true;
}

void HelpState::Update(float delta_time)
{
}

void HelpState::Render()
{
	gef::Matrix44 proj_matrix2d;
	proj_matrix2d = platform_->OrthographicFrustum(0.0f, platform_->width(), 0.0f, platform_->height(), -1.0f, 1.0f);
	sprite_renderer_->set_projection_matrix(proj_matrix2d);																	// <--- every frame?

	sprite_renderer_->Begin(true);
	sprite_renderer_->DrawSprite(background_sprite_);
	sprite_renderer_->End();
}

void HelpState::Release()
{

}