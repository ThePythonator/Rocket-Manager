#pragma once

#include "ExtraMaths.hpp"

#include "Graphics.hpp"

#include "Camera.hpp"

class Particle {
public:
	Particle(phyflt size = 1.0f, phyflt size_change = 0.0f, phyvec centre = {}, phyvec velocity = {}, phyflt age = {}, Framework::Colour colour = Framework::Colour(), phyflt alpha_change = 0.0f);

	void update(phyflt dt);
	void render(Framework::Graphics* graphics, Camera camera) const;

	phyflt size();
	phyflt age();

private:
	phyflt _size = 1.0f;
	phyflt _size_change = -1.0f;
	phyvec _centre, _velocity;
	phyflt _age = 5.0f;

	Framework::Colour _colour;
	phyflt _alpha_change = 0.0f;

	phyflt _alpha = 0xFF; // Need float version since otherwise rounding causes big alpha issues
};

class Particles {
public:
	Particles();
	Particles(Framework::Graphics* graphics);

	void add_particle(Particle particle);

	void update(phyflt dt);
	void render(Camera camera);

private:
	Framework::Graphics* _graphics = nullptr;

	std::vector<Particle> _particles;
};