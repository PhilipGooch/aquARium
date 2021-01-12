#include "menu_state.h"

#include <gxm.h>
#include <motion.h>
#include <libdbg.h>
#include <libsmart.h>
#include <sony_sample_framework.h>
#include <sony_tracking.h>

#include <graphics/sprite_renderer.h>
#include <graphics/font.h>
#include <system/platform.h>
#include <graphics/renderer_3d.h>

#include <assets/png_loader.h>
#include <graphics/image_data.h>
#include <graphics/texture.h>

#include <input/input_manager.h>
#include <input/sony_controller_input_manager.h>

#include "state_machine.h"

MenuState::MenuState(gef::Platform * platform,
					 gef::InputManager * input_manager,
					 gef::AudioManager * audio_manager,
					 gef::Renderer3D * renderer_3D,
					 gef::SpriteRenderer * sprite_renderer,
					 gef::Font * font,
					 StateMachine * state_machine) :
	State(platform, input_manager, audio_manager, renderer_3D, sprite_renderer, font, state_machine)
{
	// LIGHTS
	float light = 0.8f;
	gef::Default3DShaderData& default_shader_data = renderer_3D_->default_shader_data();
	default_shader_data.set_ambient_light_colour(gef::Colour(light, light, light, 1.0f));

	// CAMERA
	float aspect_ratio = (float)platform_->width() / (float)platform_->height();
	float field_of_view = 3.141592f / 4;
	float near_plane_ = 0.01f;
	float far_plane_ = 1000.f;
	projection_matrix_ = platform_->PerspectiveProjectionFov(field_of_view, aspect_ratio, near_plane_, far_plane_);
	view_matrix_.SetIdentity();

	// SPRITES
	gef::PNGLoader png_loader;
	gef::ImageData image_data;

	gef::Texture* background_texture = nullptr;
	png_loader.Load("menu.png", *platform_, image_data);
	background_texture = gef::Texture::Create(*platform_, image_data);

	background_sprite_.set_width(960);
	background_sprite_.set_height(544);
	background_sprite_.set_position(gef::Vector4(960 / 2, 544 / 2, 1.f, 1.f));
	background_sprite_.set_texture(background_texture);

	gef::Texture* indicator_texture = nullptr;
	png_loader.Load("indicator.png", *platform_, image_data);
	indicator_texture = gef::Texture::Create(*platform_, image_data);
	indicator_sprite_.set_width(40);
	indicator_sprite_.set_height(41);
	indicator_sprite_.set_texture(indicator_texture);
}


MenuState::~MenuState()
{
}

bool MenuState::HandleInput()
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
				if (controller->buttons_down() & gef_SONY_CTRL_UP)
				{
					if (selection_ > 0)
					{
						selection_--;
					}
				}
				if (controller->buttons_down() & gef_SONY_CTRL_DOWN)
				{
					if (selection_ < 1)
					{
						selection_++;
					}
				}
				if (controller->buttons_down() & gef_SONY_CTRL_CROSS)
				{
					if (selection_ == 0)
					{
						state_machine_->SetState(StateMachine::GAME);
					}
					else if (selection_ == 1)
					{
						state_machine_->SetState(StateMachine::OPTIONS);
					}
				}
			}
		}
	}

	return true;
}

void MenuState::Update(float delta_time)
{
	if (selection_ == 0)
	{
		indicator_sprite_.set_position(gef::Vector4(370, 275, 1, 1));
	}
	else if (selection_ == 1)
	{
		indicator_sprite_.set_position(gef::Vector4(370, 375, 1, 1));
	}
}

void MenuState::Render()
{
	gef::Matrix44 proj_matrix2d;
	proj_matrix2d = platform_->OrthographicFrustum(0.0f, platform_->width(), 0.0f, platform_->height(), -1.0f, 1.0f);
	sprite_renderer_->set_projection_matrix(proj_matrix2d);																	// <--- every frame?

	renderer_3D_->set_projection_matrix(projection_matrix_);
	renderer_3D_->set_view_matrix(view_matrix_);


	sprite_renderer_->Begin(true);
	sprite_renderer_->DrawSprite(background_sprite_);
	sprite_renderer_->DrawSprite(indicator_sprite_);
	sprite_renderer_->End();

	
	/*renderer_3D_->Begin(false);


	renderer_3D_->End();

	if (font_)
	{
		sprite_renderer_->Begin(false);
		font_->RenderText(sprite_renderer_, gef::Vector4(480.f, 200.f, -0.9f), 2.0f, 0xff000000, gef::TJ_CENTRE, "PLAY");
		font_->RenderText(sprite_renderer_, gef::Vector4(480.f, 275.f, -0.9f), 2.0f, 0xff000000, gef::TJ_CENTRE, "OPTIONS");
		font_->RenderText(sprite_renderer_, gef::Vector4(850.0f, 510.0f, -0.9f), 1.0f, 0xffffffff, gef::TJ_LEFT, "FPS: %.1f", fps_);
		sprite_renderer_->End();
	}*/
}
