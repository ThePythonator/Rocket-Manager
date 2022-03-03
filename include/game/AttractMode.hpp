#pragma once

#include "GraphicsObjects.hpp"

#include "Constants.hpp"

class AttractMode {
public:
	AttractMode();
	AttractMode(Framework::GraphicsObjects* _graphics_objects);

	void update(float dt);
	void render();

private:
	Framework::GraphicsObjects* graphics_objects = nullptr;
};