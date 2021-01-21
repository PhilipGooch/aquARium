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
#include <graphics/scene.h>

#include <input/input_manager.h>
#include <input/sony_controller_input_manager.h>

#include "state_machine.h"
#include "fish.h"
#include "boid.h"

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

// FISHES
const char* fish_body_blue_filename = "fish/blue/body.scn";
fish_body_blue_scene_assets_ = LoadSceneAssets(*platform_, fish_body_blue_filename);
const char* fish_tail_blue_filename = "fish/blue/tail.scn";
fish_tail_blue_scene_assets_ = LoadSceneAssets(*platform_, fish_tail_blue_filename);
const char* fish_body_orange_filename = "fish/orange/body.scn";
fish_body_orange_scene_assets_ = LoadSceneAssets(*platform_, fish_body_orange_filename);
const char* fish_tail_orange_filename = "fish/orange/tail.scn";
fish_tail_orange_scene_assets_ = LoadSceneAssets(*platform_, fish_tail_orange_filename);

fish_body_blue_mesh_ = GetMeshFromSceneAssets(fish_body_blue_scene_assets_);
fish_tail_blue_mesh_ = GetMeshFromSceneAssets(fish_tail_blue_scene_assets_);
fish_body_orange_mesh_ = GetMeshFromSceneAssets(fish_body_orange_scene_assets_);
fish_tail_orange_mesh_ = GetMeshFromSceneAssets(fish_tail_orange_scene_assets_);

fish_body_blue_mesh_instance_.set_mesh(fish_body_blue_mesh_);
fish_tail_blue_mesh_instance_.set_mesh(fish_tail_blue_mesh_);
fish_body_orange_mesh_instance_.set_mesh(fish_body_orange_mesh_);
fish_tail_orange_mesh_instance_.set_mesh(fish_tail_orange_mesh_);

for (int i = 0; i < number_of_fishes_; i++)
{
	fishes_.push_back(new Fish());
}

gef::Matrix44 scale;
scale.SetIdentity();

gef::Matrix44 rotation_x;
rotation_x.SetIdentity();
rotation_x.RotationX(-3.1415 / 2);

gef::Matrix44 rotation_y;
rotation_y.SetIdentity();
rotation_y.RotationY(-3.1415 / 8);

gef::Matrix44 rotation;
rotation = rotation_x * rotation_y;

gef::Matrix44 translation;
translation.SetIdentity();
translation.SetTranslation(gef::Vector4(-0.016, -0.007, -0.04));

gef::Matrix44 transform;
transform = rotation * translation;

fishes_[0]->setWorldTransform(transform);

rotation_y.RotationY(3.1415 + 3.1415 / 8);
rotation = rotation_x * rotation_y;
translation.SetTranslation(gef::Vector4(0.016, -0.007, -0.04));
transform = rotation * translation;

fishes_[1]->setWorldTransform(transform);

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
				if (!(controller->buttons_down() & gef_SONY_CTRL_UP))
				{
					up_pressed_ = false;
				}
				if (!(controller->buttons_down() & gef_SONY_CTRL_DOWN))
				{
					down_pressed_ = false;
				}
				if (!up_pressed_ && (controller->buttons_down() & gef_SONY_CTRL_UP))
				{
					up_pressed_ = true;
					if (selection_ > 0)
					{
						selection_--;
					}
				}
				if (!down_pressed_ && (controller->buttons_down() & gef_SONY_CTRL_DOWN))
				{
					down_pressed_ = true;
					if (selection_ < 2)
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
					else if (selection_ == 2)
					{
						exit(0);
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
		indicator_sprite_.set_position(gef::Vector4(375, 275, 1, 1));
	}
	else if (selection_ == 1)
	{
		indicator_sprite_.set_position(gef::Vector4(375, 375, 1, 1));
	}
	else if (selection_ == 2)
	{
		indicator_sprite_.set_position(gef::Vector4(375, 475, 1, 1));
	}

	for (Boid* boid : fishes_)
	{
		Fish* fish = (Fish*)boid;
		fish->Animate(delta_time);
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

	renderer_3D_->Begin(false);

	Fish* blue_fish = (Fish*)fishes_[0];
	Fish* orange_fish = (Fish*)fishes_[1];

	fish_body_blue_mesh_instance_.set_transform(blue_fish->GetBodyTransform());
	fish_tail_blue_mesh_instance_.set_transform(blue_fish->GetTailTransform());
	fish_body_orange_mesh_instance_.set_transform(orange_fish->GetBodyTransform());
	fish_tail_orange_mesh_instance_.set_transform(orange_fish->GetTailTransform());

	renderer_3D_->DrawMesh(fish_body_blue_mesh_instance_);
	renderer_3D_->DrawMesh(fish_tail_blue_mesh_instance_);
	renderer_3D_->DrawMesh(fish_body_orange_mesh_instance_);
	renderer_3D_->DrawMesh(fish_tail_orange_mesh_instance_);

	renderer_3D_->End();

	/*if (font_)
	{
		sprite_renderer_->Begin(false);
		font_->RenderText(sprite_renderer_, gef::Vector4(480.f, 200.f, -0.9f), 2.0f, 0xff000000, gef::TJ_CENTRE, "PLAY");
		font_->RenderText(sprite_renderer_, gef::Vector4(480.f, 275.f, -0.9f), 2.0f, 0xff000000, gef::TJ_CENTRE, "OPTIONS");
		font_->RenderText(sprite_renderer_, gef::Vector4(850.0f, 510.0f, -0.9f), 1.0f, 0xffffffff, gef::TJ_LEFT, "FPS: %.1f", fps_);
		sprite_renderer_->End();
	}*/
}

gef::Scene* MenuState::LoadSceneAssets(gef::Platform& platform, const char* filename)
{
	gef::Scene* scene = new gef::Scene();

	if (scene->ReadSceneFromFile(platform, filename))
	{
		// if scene file loads successful
		// create material and mesh resources from the scene data
		scene->CreateMaterials(platform);
		scene->CreateMeshes(platform);
	}
	else
	{
		delete scene;
		scene = NULL;
	}

	return scene;
}

gef::Mesh* MenuState::GetMeshFromSceneAssets(gef::Scene* scene)
{
	gef::Mesh* mesh = NULL;

	// if the scene data contains at least one mesh
	// return the first mesh
	if (scene && scene->meshes.size() > 0)
		mesh = scene->meshes.front();

	return mesh;
}