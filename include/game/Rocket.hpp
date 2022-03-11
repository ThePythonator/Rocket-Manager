#pragma once

#include "Component.hpp"

class Rocket : public ComponentManager {
public:
	struct InitialData {
		phyvec position, velocity;
		phyflt angle = 0.0f;
	};

	Rocket();

	void set_name(std::string);
	std::string get_name() const;

	void set_initial_data(InitialData initial_data);
	InitialData get_initial_data() const;

	void set_is_template(bool is_template);
	bool get_is_template() const;

private:
	std::string _name;

	InitialData _initial_data;

	bool _is_template = true;
};


// Conversion methods
void to_json(json& j, const Rocket& r);
void from_json(const json& j, Rocket& r);

void to_json(json& j, const Rocket::InitialData& d);
void from_json(const json& j, Rocket::InitialData& d);