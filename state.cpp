#include "state.h"

#include <graphics/scene.h>
#include "state_machine.h"

State::State(gef::Platform * platform,
			 gef::InputManager * input_manager,
			 gef::AudioManager * audio_manager,
			 gef::Renderer3D * renderer_3D,
			 gef::SpriteRenderer * sprite_renderer,
			 gef::Font * font,
			 StateMachine * state_machine) :
	platform_(platform),
	input_manager_(input_manager),
	audio_manager_(audio_manager),
	sprite_renderer_(sprite_renderer),
	renderer_3D_(renderer_3D),
	font_(font),
	state_machine_(state_machine)
{
}

gef::Scene* State::LoadSceneAssets(gef::Platform& platform, const char* filename)
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

gef::Mesh* State::GetMeshFromSceneAssets(gef::Scene* scene)
{
	gef::Mesh* mesh = NULL;

	// if the scene data contains at least one mesh
	// return the first mesh
	if (scene && scene->meshes.size() > 0)
		mesh = scene->meshes.front();

	return mesh;
}