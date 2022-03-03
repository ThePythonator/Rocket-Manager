#include "GameUtilities.hpp"

std::vector<Framework::Button> create_menu_buttons(const Framework::Rect& button_area, const Framework::vec2& button_size, const Framework::Button::ButtonImages& button_images, const std::vector<std::string>& button_titles, Framework::Font* font, const Framework::Colour& text_colour, float top_button_y) {
	std::vector<Framework::Button> buttons;

	float total_height = button_size.y * button_titles.size();
	Framework::vec2 position = Framework::Vec(button_area.position.x, top_button_y);

	for (uint8_t i = 0; i < button_titles.size(); i++) {
		Framework::Text text = Framework::Text(font, button_titles[i], text_colour);
		Framework::Button button = Framework::Button(Framework::Rect(position, button_size), button_images, text, i);

		buttons.push_back(button);

		position.y += button_size.y;
	}

	return buttons;
}

std::vector<Framework::Button> create_menu_buttons_centralised(const Framework::Rect& button_area, const Framework::vec2& button_size, const Framework::Button::ButtonImages& button_images, const std::vector<std::string>& button_titles, Framework::Font* font, const Framework::Colour& text_colour) {
	std::vector<Framework::Button> buttons;

	float total_height = button_size.y * button_titles.size();

	float position_y = button_area.centre().y - total_height / 2;

	return create_menu_buttons(button_area, button_size, button_images, button_titles, font, text_colour, position_y);
}

std::vector<Framework::Button> create_submenu_from_constants(Framework::GraphicsObjects* graphics_objects, const std::vector<std::string>& button_titles, float top_button_y) {
	return create_menu_buttons(MENU::SUBMENU_BACKGROUND_RECT, BUTTONS::SIZE, graphics_objects->button_image_groups[GRAPHICS_OBJECTS::BUTTON_IMAGE_GROUPS::DEFAULT], button_titles, graphics_objects->font_ptrs[GRAPHICS_OBJECTS::FONTS::MAIN_FONT], COLOURS::WHITE, top_button_y);
}

std::vector<Framework::Button> create_menu_from_constants(Framework::GraphicsObjects* graphics_objects, const std::vector<std::string>& button_titles) {
	return create_menu_buttons_centralised(MENU::BACKGROUND_RECT, BUTTONS::SIZE, graphics_objects->button_image_groups[GRAPHICS_OBJECTS::BUTTON_IMAGE_GROUPS::DEFAULT], button_titles, graphics_objects->font_ptrs[GRAPHICS_OBJECTS::FONTS::MAIN_FONT], COLOURS::WHITE);
}


//void render_trimmed_circle_to_window(Framework::Graphics* graphics, const Framework::vec2& centre, float radius, const Framework::Colour& colour) {
//	graphics->render_trimmed_circle(centre, radius, Framework::Rect(Framework::VEC_NULL, WINDOW::SIZE), colour);
//}