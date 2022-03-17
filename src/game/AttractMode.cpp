#include "AttractMode.hpp"

AttractMode::AttractMode() {

}
AttractMode::AttractMode(Framework::GraphicsObjects* _graphics_objects) {
	graphics_objects = _graphics_objects;
}

void AttractMode::update(float dt) {

}

void AttractMode::render() {
	graphics_objects->graphics_ptr->fill(COLOURS::BLACK);

	uint8_t earth = GAME::SANDBOX::BODIES::ID::EARTH;
	float radius = GAME::SANDBOX::BODIES::RADII[earth] / GAME::SANDBOX::UI::DEFAULT_CAMERA_SCALE;

	graphics_objects->graphics_ptr->fill(COLOURS::ATMOSPHERES[earth]); // TODO: change?


	Framework::SDLUtils::SDL_RenderDrawArcInWindow(graphics_objects->graphics_ptr->get_renderer(), { WINDOW::SIZE_HALF.x, WINDOW::SIZE.y * 0.7f + radius }, radius, WINDOW::SIZE, COLOURS::PLANETS[earth]);
}