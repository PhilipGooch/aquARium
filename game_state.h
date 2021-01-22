#pragma once
#include "state.h"

#include <array>
#include <vector>
#include <graphics\sprite.h>
#include <platform/vita/graphics/texture_vita.h>
#include <graphics/mesh_instance.h>

namespace gef
{
	class Platform;
	class InputManager;
	class AudioManager;
	class SpriteRenderer;
	class Renderer3D;
	class Font;
	class Material;
	class MeshInstance;
}
class PrimitiveBuilder;
class Boid;
class Paintball;
class StateMachine;

class GameState : public State
{
public:
	GameState(gef::Platform * platform,
			  gef::InputManager * input_manager,
			  gef::AudioManager * audio_manager,
			  gef::Renderer3D * renderer_3D,
			  gef::SpriteRenderer * sprite_renderer,
			  gef::Font * font,
			  StateMachine * state_machine);
	~GameState();

protected:
	void Init() override;
	bool HandleInput() override;
	void Update(float delta_time) override;
	void Render() override;
	void Release() override;

	bool AllMarkersDetected();
	void CalculateOffsetTransforms();
	void FirePaintball();
	gef::Material* LoadMaterial(char* file_name);

	PrimitiveBuilder * primitive_builder_;

	// CAMERA SPRITE
	gef::Sprite camera_feed_sprite_;
	gef::TextureVita camera_feed_texture_;

	// MARKERS
	std::vector<gef::Material *> marker_materials_;
	std::vector<gef::Mesh *> marker_meshes_;
	std::vector<gef::MeshInstance> marker_mesh_instances_;
	const int number_of_markers_ = 5;
	bool marker_detected_ = false;
	int anchor_ = 0;
	gef::Matrix44 marker_transform_;
	bool callibrating_ = true;
	std::vector<gef::Matrix44> offset_transforms_;

	// FISHES
	gef::MeshInstance fish_body_mesh_instance_;
	gef::MeshInstance fish_tail_mesh_instance_;
	int number_of_fishes_ = 40;
	std::vector<Boid*> fishes_; 
	int number_of_blue_fishes_ = number_of_fishes_;
	int number_of_orange_fishes_ = 0;
	bool edit_ = false;
	int flocking_variable_ = 0;
	
	// ENVIRONMENT
	gef::Mesh * environment_mesh_;
	gef::MeshInstance environment_mesh_instance_;
	gef::Vector4 environment_dimensions_;

	// PAINTBALLS
	std::vector<gef::Material *> paintball_materials_;
	gef::Mesh * paintball_mesh_;
	gef::MeshInstance paintball_mesh_instance_;
	std::vector<Paintball*> paintballs_;
	int reload_time_ = 0;
	
	// SOUNDS
	int fire_SFX_ID_;
	int hit_SFX_ID_;
};

