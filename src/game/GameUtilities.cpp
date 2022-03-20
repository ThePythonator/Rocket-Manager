#include "GameUtilities.hpp"

std::vector<Framework::Button> create_menu_buttons(const Framework::Rect& button_area, const Framework::vec2& button_size, const Framework::Button::ButtonImages& button_images, const std::vector<std::string>& button_titles, Framework::Font* font, const Framework::Colour& text_colour, float top_button_y, uint8_t offset_i) {
	std::vector<Framework::Button> buttons;

	float total_height = button_size.y * button_titles.size();
	Framework::vec2 position = Framework::Vec(button_area.position.x, top_button_y);

	for (uint8_t i = 0; i < button_titles.size(); i++) {
		Framework::Text text = Framework::Text(font, button_titles[i], text_colour);
		Framework::Button button = Framework::Button(Framework::Rect(position, button_size), button_images, text, i + offset_i);

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

uint8_t atmosphere_alpha(PhysicsEngine::phyflt height_above_surface, PhysicsEngine::phyflt scale_height) {
	return 0xFF * std::exp(-height_above_surface / scale_height);
}

std::vector<std::string> find_files_with_extension(const std::string& directory, const std::string& extension) {
	std::vector<std::string> filepaths;

	// Load component data: find all json files in component directory and try parsing them
	printf("Finding %s files in %s...\n", extension.c_str(), directory.c_str());

	try {
		for (const std::filesystem::directory_entry& entry : std::filesystem::directory_iterator(directory)) {
			std::filesystem::path path = entry.path();

			// If not .json, skip
			if (path.extension().string() != extension) continue;

			std::string filepath = path.string();

			filepaths.push_back(filepath);
		}
	}
	catch (const std::filesystem::filesystem_error& e) {
		printf("Something went wrong! Error: %s", e.what());
	}

	return filepaths;
}

std::string get_filename(std::string path) {
	return path.substr(path.find_last_of('\\/') + 1);
}

std::string trim_extension(std::string filename) {
	return filename.substr(0, filename.find_last_of('.'));
}


std::string random_save_name() {
	return random_word_pair(STRINGS::RANDOM_SAVE_ADJECTIVES, STRINGS::RANDOM_SAVE_NOUNS);
}

std::string random_rocket_name(const std::vector<std::string>& existing_names) {
	std::string name = STRINGS::RANDOM_ROCKET_NAMES[std::rand() % STRINGS::RANDOM_ROCKET_NAMES.size()] + STRINGS::ROCKET_NAME_SEPARATOR;
	uint8_t i = 1;

	while (std::count(existing_names.begin(), existing_names.end(), name + std::to_string(i))) {
		i++;
	}

	return name + std::to_string(i);
}

std::string random_word_pair(const std::vector<std::string>& first_words, const std::vector<std::string>& second_words) {
	return first_words[std::rand() % first_words.size()] + " " + second_words[std::rand() % second_words.size()];
}