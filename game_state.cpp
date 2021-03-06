#include "game_state.h"

#include <gxm.h>
#include <motion.h>
#include <libdbg.h>
#include <libsmart.h>
#include <sony_sample_framework.h>
#include <sony_tracking.h>
#include <system/platform.h>
#include <input/input_manager.h>
#include <audio/audio_manager.h>
#include <input/sony_controller_input_manager.h>
#include <graphics/renderer_3d.h>
#include <graphics/sprite_renderer.h>
#include <graphics/font.h>
#include <assets/png_loader.h>
#include <graphics/image_data.h>
#include <graphics/texture.h>
#include <graphics/material.h>
#include <graphics/mesh.h>
#include <system/debug_log.h>
#include <stdlib.h>    
#include <time.h>  
#include "primitive_builder.h"
#include "fish.h"
#include "paintball.h"
#include "state_machine.h"

GameState::GameState(gef::Platform * platform,
					 gef::InputManager * input_manager,
					 gef::AudioManager * audio_manager,
					 gef::Renderer3D * renderer_3D,
					 gef::SpriteRenderer * sprite_renderer,
					 gef::Font * font,
					 StateMachine * state_machine) :
	State(platform, input_manager, audio_manager, renderer_3D, sprite_renderer, font, state_machine)
{
	// INITIALIZE SONY FRAMEWORK
	sampleInitialize();
	smartInitialize();

	// PROJECTION, VIEW AND ORTHOGRAPHIC MATRICES
	float camera_image_scale_factor = (960.f / 544.f) / (640.f / 480.f);
	gef::Matrix44 scale_matrix;
	scale_matrix.SetIdentity();
	scale_matrix.Scale(gef::Vector4(1.f, camera_image_scale_factor, 1.f, 1.f));
	projection_matrix_ = platform_->PerspectiveProjectionFov(SCE_SMART_IMAGE_FOV, (float)SCE_SMART_IMAGE_WIDTH / (float)SCE_SMART_IMAGE_HEIGHT, .0001f, 10.f);
	projection_matrix_ = projection_matrix_ * scale_matrix;
	view_matrix_.SetIdentity();
	ortho_matrix_.SetIdentity();
	ortho_matrix_ = platform_->OrthographicFrustum(-1.f, 1.f, -1.f, 1.f, -1.f, 1.f);

	// CAMERA FEED IMAGE
	camera_feed_sprite_.set_width(2.f);
	camera_feed_sprite_.set_height(2.f * camera_image_scale_factor);
	camera_feed_sprite_.set_position(gef::Vector4(0.f, 0.f, 1.f, 1.f));
	sprite_renderer_->set_projection_matrix(ortho_matrix_);

	// MARKERS
	primitive_builder_ = new PrimitiveBuilder(*platform_);
	marker_materials_.push_back(LoadMaterial("y_1.png"));
	marker_materials_.push_back(LoadMaterial("y_2.png"));
	marker_materials_.push_back(LoadMaterial("y_3.png"));
	marker_materials_.push_back(LoadMaterial("y_4.png"));
	marker_materials_.push_back(LoadMaterial("y_5.png"));
	marker_materials_.push_back(LoadMaterial("y_6.png"));
	marker_meshes_.push_back(primitive_builder_->CreatePlaneMesh(gef::Vector4(0.059f / 2, 0, 0.059f / 2), gef::Vector4(0.f, 0.f, 0.f), marker_materials_[0]));
	marker_meshes_.push_back(primitive_builder_->CreatePlaneMesh(gef::Vector4(0.059f / 2, 0, 0.059f / 2), gef::Vector4(0.f, 0.f, 0.f), marker_materials_[1]));
	marker_meshes_.push_back(primitive_builder_->CreatePlaneMesh(gef::Vector4(0.059f / 2, 0, 0.059f / 2), gef::Vector4(0.f, 0.f, 0.f), marker_materials_[2]));
	marker_meshes_.push_back(primitive_builder_->CreatePlaneMesh(gef::Vector4(0.059f / 2, 0, 0.059f / 2), gef::Vector4(0.f, 0.f, 0.f), marker_materials_[3]));
	marker_meshes_.push_back(primitive_builder_->CreatePlaneMesh(gef::Vector4(0.059f / 2, 0, 0.059f / 2), gef::Vector4(0.f, 0.f, 0.f), marker_materials_[4]));
	marker_meshes_.push_back(primitive_builder_->CreatePlaneMesh(gef::Vector4(0.059f / 2, 0, 0.059f / 2), gef::Vector4(0.f, 0.f, 0.f), marker_materials_[5]));
	for (int i = 0; i < number_of_markers_; i++)
	{
		gef::MeshInstance mesh_instance;
		mesh_instance.set_mesh(marker_meshes_[i]);
		marker_mesh_instances_.push_back(mesh_instance);
	}

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

	fish_body_mesh_instance_.set_mesh(fish_body_blue_mesh_);
	fish_tail_mesh_instance_.set_mesh(fish_tail_blue_mesh_);

	// ENVIRONMENT
	//environment_dimensions_ = fishes_.front()->GetEnvironmentDimensions();
	//environment_mesh_ = primitive_builder_->CreateBoxMesh(environment_dimensions_);
	//environment_mesh_instance_.set_mesh(environment_mesh_);

	// PAINTBALL
	for (int i = 0; i < 6; i++)
	{
		paintball_materials_.push_back(LoadMaterial("orange.png"));
	}
	paintball_mesh_ = primitive_builder_->CreateBoxMesh(gef::Vector4(0.002f, 0.002f, 0.002f), gef::Vector4(0.0f, 0.0f, 0.0f), &paintball_materials_[0]);
	paintball_mesh_instance_.set_mesh(paintball_mesh_);

	// LIGHTS
	float light = 0.8f;
	gef::Default3DShaderData& default_shader_data = renderer_3D_->default_shader_data();
	default_shader_data.set_ambient_light_colour(gef::Colour(light, light, light, 1.0f));

	// LOCAL TRANSFORMS
	for (int i = 0; i < 6; i++)
	{
		offset_transforms_.push_back(gef::Matrix44());
		offset_transforms_.back().SetIdentity();
	}

	// SOUNDS
	fire_SFX_ID_ = audio_manager->LoadSample("fire.wav", *platform);
	fire_SFX_ID_ = audio_manager->LoadSample("hit.wav", *platform);
}

GameState::~GameState()
{
	smartRelease();
	sampleRelease();

	marker_materials_.clear();
	marker_meshes_.clear();
	marker_mesh_instances_.clear();

	delete fish_body_blue_scene_assets_;
	fish_body_blue_scene_assets_ = nullptr;

	delete fish_tail_blue_scene_assets_;
	fish_tail_blue_scene_assets_ = nullptr;

	delete fish_body_orange_scene_assets_;
	fish_body_orange_scene_assets_ = nullptr;

	delete fish_tail_orange_scene_assets_;
	fish_tail_orange_scene_assets_ = nullptr;

	delete primitive_builder_;
	primitive_builder_ = nullptr;

	delete environment_mesh_;
	environment_mesh_ = nullptr;

	paintball_materials_.clear();

	delete paintball_mesh_;
	paintball_mesh_ = nullptr;
}

void GameState::Init()
{
	anchor_ = 0;
	reload_time_ = 0;
	edit_ = false;
	flocking_variable_ = 0;
	number_of_blue_fishes_ = number_of_fishes_;
	number_of_orange_fishes_ = 0;

	// RESET SONY TRACKING
	AppData* dat = sampleUpdateBegin();
	smartTrackingReset();
	sampleUpdateEnd(dat);

	// FISHES
	for (int i = 0; i < number_of_fishes_; i++)
	{
		fishes_.push_back(new Fish());
	}

	// DEBUG CUBE
	//debug_cube_mesh_ = primitive_builder_->CreateBoxMesh(gef::Vector4(0.01f, 0.01f, 0.01f), gef::Vector4(0.0f, 0.0f, 0.0f), &paintball_materials_[0]);
	//debug_cube_mesh_instance_.set_mesh(debug_cube_mesh_);
}

bool GameState::HandleInput()
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
				if (!(controller->buttons_down() & gef_SONY_CTRL_LEFT))
				{
					left_pressed_ = false;
				}
				if (!(controller->buttons_down() & gef_SONY_CTRL_RIGHT))
				{
					right_pressed_ = false;
				}
				if (!(controller->buttons_down() & gef_SONY_CTRL_SELECT))
				{
					select_pressed_ = false;
				}
				if (!(controller->buttons_down() & gef_SONY_CTRL_CROSS))
				{
					cross_pressed_ = false;
				}
				
				// SELECT
				if (!select_pressed_ && (controller->buttons_down() & gef_SONY_CTRL_SELECT))
				{
					select_pressed_ = true;
					edit_ = !edit_;
				}
				// RT
				if (controller->buttons_down() & gef_SONY_CTRL_R2)
				{
					if (reload_time_ > 5)
					{
						FirePaintball();
						audio_manager_->PlaySample(fire_SFX_ID_, false);
						reload_time_ = 0;
					}
				}
				// Down
				if (!down_pressed_ && (controller->buttons_down() & gef_SONY_CTRL_DOWN))
				{
					down_pressed_ = true;
					flocking_variable_++;
					if (flocking_variable_ > 8)
					{
						flocking_variable_ = 0;
					}
				}
				// Up
				if (!up_pressed_ && (controller->buttons_down() & gef_SONY_CTRL_UP))
				{					
					up_pressed_ = true;
					flocking_variable_--;
					if (flocking_variable_ < 0)
					{
						flocking_variable_ = 8;
					}
				}
				float delta = 0.01f;
				// Left
				if (controller->buttons_down() & gef_SONY_CTRL_LEFT)
				{
					switch (flocking_variable_)
					{
					case 0:
						for (Boid* boid : fishes_)
						{
							Fish* fish = (Fish*)boid;
							fish->perception_ -= delta;
						}
						break;
					case 1:
						for (Boid* boid : fishes_)
						{
							Fish* fish = (Fish*)boid;
							fish->max_force_ -= delta;
						}
						break;
					case 2:
						for (Boid* boid : fishes_)
						{
							Fish* fish = (Fish*)boid;
							fish->max_speed_ -= delta;
						}
						break;
					case 3:
						for (Boid* boid : fishes_)
						{
							Fish* fish = (Fish*)boid;
							fish->min_speed_ -= delta;
						}
						break;
					case 4:
						for (Boid* boid : fishes_)
						{
							Fish* fish = (Fish*)boid;
							fish->separation_weight_ -= delta;
						}
						break;
					case 5:
						for (Boid* boid : fishes_)
						{
							Fish* fish = (Fish*)boid;
							fish->alignment_weight_ -= delta;
						}
						break;
					case 6:
						for (Boid* boid : fishes_)
						{
							Fish* fish = (Fish*)boid;
							fish->cohesion_weight_ -= delta;
						}
						break;
					case 7:
						for (Boid* boid : fishes_)
						{
							Fish* fish = (Fish*)boid;
							fish->edges_weight_ -= delta;
						}
						break;
					}
				}
				// Right
				if (controller->buttons_down() & gef_SONY_CTRL_RIGHT)
				{
					switch (flocking_variable_)
					{
					case 0:
						for (Boid* boid : fishes_)
						{
							Fish* fish = (Fish*)boid;
							fish->perception_ += delta;
						}
						break;
					case 1:
						for (Boid* boid : fishes_)
						{
							Fish* fish = (Fish*)boid;
							fish->max_force_ += delta;
						}
						break;
					case 2:
						for (Boid* boid : fishes_)
						{
							Fish* fish = (Fish*)boid;
							fish->max_speed_ += delta;
						}
						break;
					case 3:
						for (Boid* boid : fishes_)
						{
							Fish* fish = (Fish*)boid;
							fish->min_speed_ += delta;
						}
						break;
					case 4:
						for (Boid* boid : fishes_)
						{
							Fish* fish = (Fish*)boid;
							fish->separation_weight_ += delta;
						}
						break;
					case 5:
						for (Boid* boid : fishes_)
						{
							Fish* fish = (Fish*)boid;
							fish->alignment_weight_ += delta;
						}
						break;
					case 6:
						for (Boid* boid : fishes_)
						{
							Fish* fish = (Fish*)boid;
							fish->cohesion_weight_ += delta;
						}
						break;
					case 7:
						for (Boid* boid : fishes_)
						{
							Fish* fish = (Fish*)boid;
							fish->edges_weight_ += delta;
						}
						break;
					}
				}
				// CROSS
				if (!cross_pressed_ && (controller->buttons_down() & gef_SONY_CTRL_CROSS))
				{
					if (edit_ && flocking_variable_ == 8)
					{
						for (Boid* boid : fishes_)
						{
							Fish* fish = (Fish*)boid;
							fish->perception_ = 0.5f;
							fish->max_force_ = 0.06f;
							fish->max_speed_ = 0.1f;
							fish->min_speed_ = 0.0f;
							fish->separation_weight_ = 1.4f;
							fish->alignment_weight_ = 1.0f;
							fish->cohesion_weight_ = 1.1f;
							fish->edges_weight_ = 1.5f;
						}
					}
				}
				// TRIANGLE
				if (controller->buttons_down() & gef_SONY_CTRL_TRIANGLE)
				{
					state_machine_->SetState(StateMachine::MENU);
				}
			}
		}
	}
	return true;
}

void GameState::Update(float delta_time)
{
	fps_ = 1.f / delta_time;

	if (reload_time_ <= 5)
	{
		reload_time_++;
	}

	AppData* dat = sampleUpdateBegin();
	smartUpdate(dat->currentImage);

	if (callibrating_)
	{
		if (AllMarkersDetected())
		{
			CalculateOffsetTransforms();

			marker_detected_ = true;
			callibrating_ = false;
		}
		else
		{
			sampleUpdateEnd(dat);
			return;
		}
	}

	if (sampleIsMarkerFound(anchor_))
	{
		marker_detected_ = true;
		sampleGetTransform(anchor_, &marker_transform_);

		for (Boid* boid : fishes_)
		{
			Fish* fish = (Fish*)boid;
			fish->setParentTransform(marker_transform_);
		}
	}
	else
	{
		marker_detected_ = false;
		for (int i = 0; i < number_of_markers_; i++)
		{
			if (sampleIsMarkerFound(i))
			{
				marker_detected_ = true;
				anchor_ = i;
				break;
			}
		}
	}
	sampleGetTransform(anchor_, &marker_transform_);
	for (Boid* boid : fishes_)
	{
		Fish* fish = (Fish*)boid;
		fish->setParentTransform(marker_transform_);
		fish->setOffsetTransform(offset_transforms_[anchor_]);
	}

	gef::Matrix44 marker_rotation;
	marker_rotation.SetIdentity();
	marker_rotation.RotationX(3.1415f / 2);
	marker_mesh_instances_[anchor_].set_transform(marker_rotation * marker_transform_);

	for (Boid* boid : fishes_)
	{
		Fish* fish = (Fish*)boid;
		fish->Update(fishes_, delta_time);
		fish->Animate(delta_time);
	}

	for (Paintball* paintball : paintballs_)
	{
		if (paintball->alive_)
		{
			paintball->Update(delta_time);
			if (paintball->position_.z() < -0.5f)
			{
				paintball->alive_ = false;
			}
		}
	}
	

	// COLLISIONS
	for (Paintball* paintball : paintballs_)
	{
		if (paintball->alive_)
		{
			for (Boid* boid : fishes_)
			{
				Fish* fish = (Fish*)boid;
				if (fish->alive_)
				{
					if ((fish->GetWorldTransform().GetTranslation() - paintball->GetWorldTransform().GetTranslation()).Length() < 0.014f)
					{
						audio_manager_->PlaySample(hit_SFX_ID_, false);
						paintball->alive_ = false;
						fish->alive_ = false;
						number_of_blue_fishes_--;
						number_of_orange_fishes_++;
						if (number_of_orange_fishes_ >= 10)
						{
							state_machine_->SetState(StateMachine::WIN);
						}
						break;
					}
				}
			}
		}
	}

	gef::Matrix44 environment_translation;
	environment_translation.SetIdentity();
	environment_translation.SetTranslation(gef::Vector4(-environment_dimensions_.x(), -environment_dimensions_.y(), environment_dimensions_.z()));
	environment_mesh_instance_.set_transform(environment_translation * fishes_.front()->GetOffsetTransform() * fishes_.front()->GetParentTransform());
	
	//debug_cube_mesh_instance_.set_transform(fishes_.front()->GetLocalTransform() * fishes_.front()->GetOffsetTransform() * fishes_.front()->GetParentTransform());

	sampleUpdateEnd(dat);
}

void GameState::Render()
{
	renderer_3D_->set_projection_matrix(projection_matrix_);
	renderer_3D_->set_view_matrix(view_matrix_);

	AppData* dat = sampleRenderBegin();
	ortho_matrix_ = platform_->OrthographicFrustum(-1.f, 1.f, -1.f, 1.f, -1.f, 1.f);		
	sprite_renderer_->set_projection_matrix(ortho_matrix_);
	if (dat->currentImage)
	{
		camera_feed_texture_.set_texture(dat->currentImage->tex_yuv);
		camera_feed_sprite_.set_texture(&camera_feed_texture_);
	}
	sprite_renderer_->Begin(true);
	sprite_renderer_->DrawSprite(camera_feed_sprite_);
	sprite_renderer_->End();

	renderer_3D_->Begin(false);

	if (marker_detected_)
	{
		// FISHES
		for (Boid* boid : fishes_)
		{
			Fish* fish = (Fish*)boid;
			if (fish->alive_)
			{
				fish_body_mesh_instance_.set_mesh(fish_body_blue_mesh_);
				fish_tail_mesh_instance_.set_mesh(fish_tail_blue_mesh_);
			}
			else
			{
				fish_body_mesh_instance_.set_mesh(fish_body_orange_mesh_);
				fish_tail_mesh_instance_.set_mesh(fish_tail_orange_mesh_);
			}

			fish_body_mesh_instance_.set_transform(fish->GetBodyTransform());
			fish_tail_mesh_instance_.set_transform(fish->GetTailTransform());
			renderer_3D_->DrawMesh(fish_body_mesh_instance_);
			renderer_3D_->DrawMesh(fish_tail_mesh_instance_);
		}

		// MARKER
		renderer_3D_->DrawMesh(marker_mesh_instances_[anchor_]);
		
		// ENVIRONMENT
		//renderer_3D_->SetFillMode(gef::Renderer3D::FillMode::kWireframe);
		//renderer_3D_->DrawMesh(environment_mesh_instance_);
		//renderer_3D_->SetFillMode(gef::Renderer3D::FillMode::kSolid);
		

		// PAINTBALL

		gef::Matrix44 marker_rotation;
		marker_rotation.SetIdentity();
		marker_rotation.RotationX(3.1415f / 2);
		marker_mesh_instances_[anchor_].set_transform(marker_rotation * marker_transform_);

		for (Paintball* paintball : paintballs_)
		{
			if (paintball->alive_)
			{
				paintball_mesh_instance_.set_transform(paintball->GetWorldTransform());
				renderer_3D_->DrawMesh(paintball_mesh_instance_);
			}
		}
	}

	//renderer_3D_->DrawMesh(debug_cube_mesh_instance_);

	renderer_3D_->End();

	if (font_)
	{
		gef::Matrix44 projection_2D;
		projection_2D = platform_->OrthographicFrustum(0.0f, (float)platform_->width(), 0.0f, (float)platform_->height(), -1.0f, 1.0f);
		sprite_renderer_->set_projection_matrix(projection_2D);
		sprite_renderer_->Begin(false);
		font_->RenderText(sprite_renderer_, gef::Vector4(850.0f, 510.0f, -0.9f), 1.0f, 0xffffffff, gef::TJ_LEFT, "FPS: %.1f", fps_);

		font_->RenderText(sprite_renderer_, gef::Vector4(760.0f, 10.0f, -0.9f), 3.0f, 0xffff0000, gef::TJ_CENTRE, "%i", number_of_blue_fishes_);
		font_->RenderText(sprite_renderer_, gef::Vector4(820.0f, 10.0f, -0.9f), 3.0f, 0xff000000, gef::TJ_CENTRE, " /");
		font_->RenderText(sprite_renderer_, gef::Vector4(900.0f, 10.0f, -0.9f), 3.0f, 0xff277fff, gef::TJ_CENTRE, "%i", number_of_orange_fishes_);

		if (edit_)
		{
			font_->RenderText(sprite_renderer_, gef::Vector4(20.0f, 20.0f + flocking_variable_ * 40, -0.9f), 1.0f, 0xffffffff, gef::TJ_LEFT, ">");
			font_->RenderText(sprite_renderer_, gef::Vector4(50.0f, 20.0f, -0.9f), 1.0f, 0xffffffff, gef::TJ_LEFT, "Perception: %.3f", fishes_.front()->perception_);
			font_->RenderText(sprite_renderer_, gef::Vector4(50.0f, 60.0f, -0.9f), 1.0f, 0xffffffff, gef::TJ_LEFT, "Max Force:  %.3f", fishes_.front()->max_force_);
			font_->RenderText(sprite_renderer_, gef::Vector4(50.0f, 100.0f, -0.9f), 1.0f, 0xffffffff, gef::TJ_LEFT, "Max Speed:  %.3f", fishes_.front()->max_speed_);
			font_->RenderText(sprite_renderer_, gef::Vector4(50.0f, 140.0f, -0.9f), 1.0f, 0xffffffff, gef::TJ_LEFT, "Min Speed:  %.3f", fishes_.front()->min_speed_);
			font_->RenderText(sprite_renderer_, gef::Vector4(50.0f, 180.0f, -0.9f), 1.0f, 0xffffffff, gef::TJ_LEFT, "Separation: %.3f", fishes_.front()->separation_weight_);
			font_->RenderText(sprite_renderer_, gef::Vector4(50.0f, 220.0f, -0.9f), 1.0f, 0xffffffff, gef::TJ_LEFT, "Alignment:  %.3f", fishes_.front()->alignment_weight_);
			font_->RenderText(sprite_renderer_, gef::Vector4(50.0f, 260.0f, -0.9f), 1.0f, 0xffffffff, gef::TJ_LEFT, "Cohesion:   %.3f", fishes_.front()->cohesion_weight_);
			font_->RenderText(sprite_renderer_, gef::Vector4(50.0f, 300.0f, -0.9f), 1.0f, 0xffffffff, gef::TJ_LEFT, "Edges:      %.3f", fishes_.front()->edges_weight_);
			font_->RenderText(sprite_renderer_, gef::Vector4(50.0f, 340.0f, -0.9f), 1.0f, 0xffffffff, gef::TJ_LEFT, "Defaults", fishes_.front()->edges_weight_);
		}

		//font_->RenderText(sprite_renderer_, gef::Vector4(50.0f, 300.0f, -0.9f), 1.0f, 0xffffffff, gef::TJ_LEFT, "%f %f %f", fishes_.front()->GetWorldTransform().GetTranslation().x(), fishes_.front()->GetWorldTransform().GetTranslation().y(), fishes_.front()->GetWorldTransform().GetTranslation().z());

		if (!marker_detected_)
		{
			// DISPLAY MESSAGE
			//font_->RenderText(sprite_renderer_, gef::Vector4((float)platform_->width() / 2, (float)platform_->height() / 2, -0.9f), 1.5f, 0xff00ffff, gef::TJ_CENTRE, "TARGET MARKER %i", anchor_ + 1);
		}
		sprite_renderer_->End();
	}
}

void GameState::Release()
{
	fishes_.clear();
	paintballs_.clear();
}

bool GameState::AllMarkersDetected()
{
	bool all_markers_detected = true;
	for (int i = 0; i < number_of_markers_; i++)
	{
		if (!sampleIsMarkerFound(i))
		{
			all_markers_detected = false;
			break;
		}
	}
	return all_markers_detected;
}

void GameState::CalculateOffsetTransforms()
{
	gef::Matrix44 main_marker;
	sampleGetTransform(0, &main_marker);

	for (int i = 1; i < number_of_markers_; i++)
	{
		gef::Matrix44 secondary_marker;
		sampleGetTransform(i, &secondary_marker);

		gef::Matrix44 inverse_secondary_marker;
		inverse_secondary_marker.AffineInverse(secondary_marker);

		offset_transforms_[i] = main_marker * inverse_secondary_marker;
	}
}

void GameState::FirePaintball()
{
	gef::Matrix44 inverse_marker_transform;
	inverse_marker_transform.AffineInverse(marker_transform_);

	Paintball* paintball = new Paintball();
	//paintball->spawn_transform_ = inverse_marker_transform;
	//paintball->setParentTransform(inverse_marker_transform);
	//paintball->setOffsetTransform(offset_transforms_[anchor_]);
	paintballs_.push_back(paintball);
}

gef::Material* GameState::LoadMaterial(char* file_name)
{
	gef::ImageData image_data;
	gef::PNGLoader png_loader;
	png_loader.Load(file_name, *platform_, image_data);
	gef::Texture* texture = gef::Texture::Create(*platform_, image_data);  
	gef::Material* material = new gef::Material();
	material->set_texture(texture);
	return material;
}

