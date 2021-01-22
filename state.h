#pragma once

#include <maths\matrix44.h>

namespace gef
{
	class Platform;
	class InputManager;
	class AudioManager;
	class SpriteRenderer;
	class Renderer3D;
	class Font;
	class Mesh;
	class MeshInstance;
	class Scene;
}
class StateMachine;

class State
{
public:
	State(gef::Platform * platform,
		  gef::InputManager * input_manager,
		  gef::AudioManager * audio_manager,
		  gef::Renderer3D * renderer_3D,
		  gef::SpriteRenderer * sprite_renderer,
		  gef::Font * font,
		  StateMachine * state_machine
	);

	// virtual functions use dynamic dispatch using a v-table.
	// v-tables map virtual functions to the correct overridden function at runtime.
	// they have some overhead as memory is needed to create the v-table and there is some extra processing time for look-up.
	// very convenient. allow for polymorphism.

	// = 0 indicates the function is a pure virtual function.
	// a class with a pure virual function makes it abstract. 
	// abstract classes can not be instantiated.
	// child classes MUST override these functions or they will also be abstract.

	virtual void Init() = 0;
	virtual bool HandleInput() = 0;
	virtual void Update(float delta_time) = 0;
	virtual void Render() = 0;
	virtual void Release() = 0;

protected:
	gef::Scene * LoadSceneAssets(gef::Platform& platform, const char* filename);
	gef::Mesh * GetMeshFromSceneAssets(gef::Scene* scene);

	gef::Platform* platform_;
	gef::InputManager* input_manager_;
	gef::AudioManager* audio_manager_;
	gef::SpriteRenderer* sprite_renderer_;
	gef::Renderer3D* renderer_3D_;
	gef::Font* font_;

	float fps_;

	StateMachine * state_machine_;

	bool up_pressed_ = false;
	bool down_pressed_ = false;
	bool left_pressed_ = false;
	bool right_pressed_ = false;
	bool cross_pressed_ = false;
	bool square_pressed_ = false;
	bool triangle_pressed_ = false;
	bool circle_pressed_ = false;
	bool l1_pressed_ = false;
	bool r1_pressed_ = false;
	bool select_pressed_ = false;

	gef::Matrix44 projection_matrix_;
	gef::Matrix44 view_matrix_;
	gef::Matrix44 ortho_matrix_;

	gef::Scene * fish_body_blue_scene_assets_;
	gef::Scene * fish_tail_blue_scene_assets_;
	gef::Scene * fish_body_orange_scene_assets_;
	gef::Scene * fish_tail_orange_scene_assets_;

	gef::Mesh * fish_body_blue_mesh_;
	gef::Mesh * fish_tail_blue_mesh_;
	gef::Mesh * fish_body_orange_mesh_;
	gef::Mesh * fish_tail_orange_mesh_;
};

