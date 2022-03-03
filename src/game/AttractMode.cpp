#include "AttractMode.hpp"

AttractMode::AttractMode() {

}
AttractMode::AttractMode(Framework::GraphicsObjects* _graphics_objects) {
	graphics_objects = _graphics_objects;
}

void AttractMode::update(float dt) {

}

void AttractMode::render() {
	graphics_objects->graphics_ptr->fill(COLOURS::WHITE);

	graphics_objects->graphics_ptr->render_circle(Framework::Vec(800, 200), 100, COLOURS::BLACK);
	graphics_objects->graphics_ptr->render_rect(Framework::Rect(400, 200, 100, 300), COLOURS::BLACK);

	graphics_objects->spritesheet_ptrs[GRAPHICS_OBJECTS::SPRITESHEETS::MAIN_SPRITESHEET]->sprite(0, Framework::Vec(64, 48));

	graphics_objects->spritesheet_ptrs[GRAPHICS_OBJECTS::SPRITESHEETS::MAIN_SPRITESHEET]->sprite(2, Framework::Vec(32, 48), Framework::SpriteTransform::ROTATE_90_ACW);

	graphics_objects->spritesheet_ptrs[GRAPHICS_OBJECTS::SPRITESHEETS::MAIN_SPRITESHEET]->sprite(1, Framework::Vec(96, 48), SPRITES::SCALE, SDL_GetTicks() / 10);
}