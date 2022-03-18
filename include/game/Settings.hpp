#pragma once

#include "Constants.hpp"

#include "File.hpp"

using Framework::JSONHandler::json;

struct Settings {
	bool fullscreen = false;
	bool show_debug = false;
	bool auto_move_map = true;
};

Settings load_settings();
void save_settings(const Settings& settings);

void to_json(json& j, const Settings& s);
void from_json(const json& j, Settings& s);