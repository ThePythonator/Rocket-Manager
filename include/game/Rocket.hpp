#pragma once

#include "Component.hpp"

class Rocket : public ComponentManager {
public:
	Rocket();
	Rocket(std::string name);

	std::string name() const;

private:
	std::string _name;
};