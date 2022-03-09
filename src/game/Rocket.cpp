#include "Rocket.hpp"

Rocket::Rocket() {

}

void Rocket::set_name(std::string name) {
	_name = name;
}

std::string Rocket::get_name() const {
	return _name;
}


void Rocket::set_initial_velocity(phyvec initial_velocity) {
	_initial_velocity = initial_velocity;
}
phyvec Rocket::get_initial_velocity() const {
	return _initial_velocity;
}

void Rocket::set_is_template(bool is_template) {
	_is_template = is_template;
}
bool Rocket::get_is_template() const {
	return _is_template;
}



// Conversion methods

void to_json(json& j, const Rocket& r) {
	j = json{
		{"name", r.get_name()},
		{"is_template", r.get_is_template()},
		{"components", r.get_components()},
		{"connections", r.get_connections()}
	};

	if (!r.get_is_template()) {
		// Also add velocity
		j.push_back({"velocity", r.get_initial_velocity()});
	}
}

void from_json(const json& j, Rocket& r) {
	r.set_name(j.at("name").get<std::string>());
	r.set_is_template(j.at("is_template").get<bool>());

	r.set_components(j.at("components").get<std::map<uint32_t, Component>>());
	r.set_connections(j.at("connections").get<std::vector<Connection>>());

	if (!r.get_is_template()) {
		// Also get velocity
		r.set_initial_velocity(j.at("initial_velocity").get<phyvec>());
	}
}