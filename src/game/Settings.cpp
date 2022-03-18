#include "Settings.hpp"

Settings load_settings() {
	// Set defaults
	Settings settings = Settings();

	try {
		// Read json
		json j = Framework::JSONHandler::read(PATHS::BASE_PATH + PATHS::DATA::LOCATION + PATHS::DATA::SETTINGS);

		// Fill settings struct
		j.get_to(settings);
	}
	catch (const json::exception& e) {
		// Ignore - we set defaults already so it's not the end of the world
		printf("Couldn't load settings file. Error: %s\n", e.what());
	}

	return settings;
}
void save_settings(const Settings& settings) {
	Framework::JSONHandler::write(PATHS::BASE_PATH + PATHS::DATA::LOCATION + PATHS::DATA::SETTINGS, settings);
}

void to_json(json& j, const Settings& s) {
	j = json{
		{"fullscreen", s.fullscreen},
		{"show_debug", s.show_debug},
		{"auto_move_map", s.auto_move_map}
	};
}

void from_json(const json& j, Settings& s) {
	/*j.at("fullscreen").get_to(s.fullscreen);
	j.at("show_debug").get_to(s.show_debug);
	j.at("auto_move_map").get_to(s.auto_move_map);*/

	// Set, but allow defaults
	s.fullscreen = j.value("fullscreen", s.fullscreen);
	s.show_debug = j.value("show_debug", s.show_debug);
	s.auto_move_map = j.value("auto_move_map", s.auto_move_map);
}