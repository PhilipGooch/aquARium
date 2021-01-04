#pragma once
#include "state.h"

#include <array>
#include <vector>
#include <maths\matrix44.h>
#include <graphics\sprite.h>
#include <platform/vita/graphics/texture_vita.h>
#include <graphics/mesh_instance.h>
#include "graphics/scene.h"

namespace gef
{
	class Platform;
	class InputManager;
	class AudioManager;
	class SpriteRenderer;
	class Renderer3D;
	class Font;
	class Material;
	class Mesh;
	class MeshInstance;
}
class PrimitiveBuilder;
class Boid;
class Paintball;

class GameState : public State
{
public:
	GameState(gef::Platform * platform,
			  gef::InputManager * input_manager,
			  gef::AudioManager * audio_manager,
			  gef::Renderer3D * renderer_3D,
			  gef::SpriteRenderer * sprite_renderer,
			  gef::Font * font);
	~GameState();

protected:
	bool HandleInput() override;
	void Update(float delta_time) override;
	void Render() override;

	bool AllMarkersDetected();
	void CalculateOffestTransforms();
	void FirePaintball();

	gef::Scene * LoadSceneAssets(gef::Platform& platform, const char* filename);
	gef::Mesh * GetMeshFromSceneAssets(gef::Scene* scene);
	gef::Material* GameState::LoadMaterial(char* file_name);

	PrimitiveBuilder * primitive_builder_;
	std::vector<gef::Material *> marker_materials_;
	std::vector<gef::Mesh *> marker_meshes_;
	std::vector<gef::MeshInstance> marker_mesh_instances_;

	gef::Matrix44 projection_matrix_;
	gef::Matrix44 view_matrix_;
	gef::Matrix44 ortho_matrix_;
	gef::Sprite camera_feed_sprite_;
	gef::TextureVita* camera_feed_texture_;

	gef::Scene * fish_body_scene_assets_;
	gef::Scene * fish_tail_scene_assets_;
	gef::MeshInstance fish_body_mesh_instance_;
	gef::MeshInstance fish_tail_mesh_instance_;

	gef::Mesh * environment_mesh_;
	gef::MeshInstance environment_mesh_instance_;
	gef::Vector4 environment_dimensions_;

	bool marker_detected_ = false;
	int anchor_ = 0;
	gef::Matrix44 marker_transform_;

	int number_of_fishes_ = 30;
	std::vector<Boid*> fishes_;

	std::vector<gef::Material *> paintball_materials_;
	gef::Mesh * paintball_mesh_;
	gef::MeshInstance paintball_mesh_instance_;
	std::vector<Paintball*> paintballs_;


	int reload_time_ = 0;

	int flocking_variable_ = 0;

	int delay_ = 0;

	bool callibrating_ = true;
	const int number_of_markers_ = 5;

	std::vector<gef::Matrix44> offset_transforms_;
};
