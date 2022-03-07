#pragma once

#include "File.hpp"

using Framework::JSONHandler::json;

struct Settings {
	bool show_debug = false;
	bool auto_move_map = true;
};

void to_json(json& j, const Settings& s);
void from_json(const json& j, Settings& s);