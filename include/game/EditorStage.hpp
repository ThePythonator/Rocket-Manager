#pragma once

#include "BaseStage.hpp"

#include "Rocket.hpp"
#include "Camera.hpp"

class EditorStage : public Framework::BaseStage {
public:
	void start();

	bool update(float dt);
	void render();

private:
	void render_rocket(const Rocket& r);
	void render_component(const std::pair<uint32_t, Component>& c);

	Rocket rocket;
	Camera camera;
};