#pragma once

#include "Component.hpp"

class Rocket : public ComponentManager {
public:
	Rocket();

	void set_name(std::string);
	std::string get_name() const;

	void set_initial_velocity(phyvec initial_velocity);
	phyvec get_initial_velocity() const;

	void set_is_template(bool is_template);
	bool get_is_template() const;

private:
	std::string _name;

	phyvec _initial_velocity;

	bool _is_template = true;
};


// Conversion methods
void to_json(json& j, const Rocket& r);
void from_json(const json& j, Rocket& r);