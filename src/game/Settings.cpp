#include "Settings.hpp"

void to_json(json& j, const Settings& s) {
	j = json{
		{"show_debug", s.show_debug},
		{"auto_move_map", s.auto_move_map}
	};
}

void from_json(const json& j, Settings& s) {
	j.at("show_debug").get_to(s.show_debug);
	j.at("auto_move_map").get_to(s.auto_move_map);
}