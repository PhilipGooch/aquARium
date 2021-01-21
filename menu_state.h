#pragma once
#include "state.h"

#include <maths\matrix44.h>
#include <graphics\sprite.h>
#include <platform/vita/graphics/texture_vita.h>
#include <graphics/mesh_instance.h>
#include <vector>

namespace gef
{
	class Platform;
	class InputManager;
	class AudioManager;
	class SpriteRenderer;
	class Renderer3D;
	class Font;
	class Scene;
}
class StateMachine;
class Boid;

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

	gef::Scene * LoadSceneAssets(gef::Platform& platform, const char* filename);
	gef::Mesh * GetMeshFromSceneAssets(gef::Scene* scene);

	gef::Matrix44 projection_matrix_;
	gef::Matrix44 view_matrix_;
	gef::Matrix44 ortho_matrix_;

	gef::Sprite background_sprite_;
	gef::Sprite indicator_sprite_;

	int selection_;

	gef::Scene * fish_body_blue_scene_assets_;
	gef::Scene * fish_tail_blue_scene_assets_;
	gef::Scene * fish_body_orange_scene_assets_;
	gef::Scene * fish_tail_orange_scene_assets_;

	gef::Mesh * fish_body_blue_mesh_;
	gef::Mesh * fish_tail_blue_mesh_;
	gef::Mesh * fish_body_orange_mesh_;
	gef::Mesh * fish_tail_orange_mesh_;

	gef::MeshInstance fish_body_blue_mesh_instance_;
	gef::MeshInstance fish_tail_blue_mesh_instance_;
	gef::MeshInstance fish_body_orange_mesh_instance_;
	gef::MeshInstance fish_tail_orange_mesh_instance_;

	int number_of_fishes_ = 2;
	std::vector<Boid*> fishes_;
};

