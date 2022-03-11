#include "Rocket.hpp"

Rocket::Rocket() {

}

void Rocket::set_name(std::string name) {
	_name = name;
}

std::string Rocket::get_name() const {
	return _name;
}

void Rocket::set_initial_data(InitialData initial_data) {
	_initial_data = initial_data;
}
Rocket::InitialData Rocket::get_initial_data() const {
	return _initial_data;
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
		// Also add initial data
		j["initial_data"] = r.get_initial_data();
	}
}

void from_json(const json& j, Rocket& r) {
	r.set_name(j.value("name", "unnamed_rocket"));
	r.set_is_template(j.at("is_template").get<bool>());

	r.set_components(j.at("components").get<std::map<uint32_t, Component>>());
	r.set_connections(j.at("connections").get<std::vector<Connection>>());

	if (!r.get_is_template()) {
		// Also get initial data
		r.set_initial_data(j.at("initial_data").get<Rocket::InitialData>());
	}
}

void to_json(json& j, const Rocket::InitialData& d) {
	j = json{
		{"position", d.position},
		{"velocity", d.velocity},
		{"angle", d.angle}
	};
}
void from_json(const json& j, Rocket::InitialData& d) {
	d.position = j.at("position").get<phyvec>();
	d.velocity = j.at("velocity").get<phyvec>();
	d.angle = j.at("angle").get<phyflt>();
}