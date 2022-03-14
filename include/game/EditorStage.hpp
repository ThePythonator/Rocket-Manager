#pragma once

#include "BaseStage.hpp"

#include "ExtraMaths.hpp"

#include "GameUtilities.hpp"

#include "Rocket.hpp"
#include "Camera.hpp"

class EditorStage : public Framework::BaseStage {
public:
	void init();

	void start();

	bool update(float dt);
	void render();

private:
	void render_palette();
	void render_rocket(const Rocket& rocket);
	void render_component(const Component& component);

	Rocket rocket;
	Camera camera;

	std::map<uint32_t, float> icon_scales;
	std::map<uint32_t, float> component_bounding_radii;

	uint32_t component_selected = EDITOR::NO_COMPONENT_SELECTED;
};