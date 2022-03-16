#pragma once

#include "BaseStage.hpp"

#include "ExtraMaths.hpp"

#include "GameUtilities.hpp"

#include "Stages.hpp"

#include "Rocket.hpp"
#include "Camera.hpp"

class EditorStage : public Framework::BaseStage {
public:
	void init();

	void start();

	bool update(float dt);
	void render();

private:
	void render_grid();
	void render_ui();
	void render_rocket();
	void render_component(const Component& component);

	void erase_connections(uint32_t component_id);

	//void load_rocket();
	void save_rocket();

	Rocket rocket;
	Camera camera;

	std::map<uint32_t, float> icon_scales;
	std::map<uint32_t, float> component_bounding_radii;
	std::map<uint32_t, Framework::Rect> component_bounding_rects;

	uint32_t component_selected = EDITOR::NO_COMPONENT_SELECTED;
	phyvec click_offset;
};