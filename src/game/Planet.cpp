#include "Planet.hpp"


void to_json(json& j, const Planet& p) {
	j = json{
		{"id", p.id},
		{"position", p.position},
		{"velocity", p.velocity}
	};
}
void from_json(const json& j, Planet& p) {
	j.at("id").get_to(p.id);
	j.at("position").get_to(p.position);
	j.at("velocity").get_to(p.velocity);
}