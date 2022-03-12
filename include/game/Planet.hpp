#pragma once

#include "ExtraMaths.hpp"

#include "File.hpp"

struct Planet {
	uint32_t id = 0;
	phyvec position, velocity;
};

void to_json(json& j, const Planet& p);
void from_json(const json& j, Planet& p);