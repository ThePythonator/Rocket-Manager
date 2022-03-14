#include "Particles.hpp"

// Particle
Particle::Particle(phyflt size, phyflt size_change, phyvec centre, phyvec velocity, phyflt age, Framework::Colour colour, phyflt alpha_change) {
	_size = size;
	_size_change = size_change;
	
	_centre = centre;
	_velocity = velocity;

	_age = age;
	
	_colour = colour;
	_alpha_change = alpha_change;
	_alpha = colour.a;
}

void Particle::update(phyflt dt) {
	_centre += _velocity * dt;
	_size += _size_change * dt;
	_age -= dt;
	_alpha += _alpha_change * dt;
}

void Particle::render(Framework::Graphics* graphics, Camera camera) const {
	graphics->render_filled_rect(Framework::centred_rect(PhysicsEngine::to_fvec(camera.get_render_position(_centre)), _size * camera.get_scale()), Framework::Colour(_colour, _alpha));
}


phyflt Particle::size() {
	return _size;
}
phyflt Particle::age() {
	return _age;
}



// Particles

Particles::Particles() {

}
Particles::Particles(Framework::Graphics* graphics) {
	_graphics = graphics;
}

void Particles::add_particle(Particle particle) {
	_particles.push_back(particle);
}

void Particles::update(phyflt dt) {
	for (Particle& particle : _particles) {
		particle.update(dt);
	}

	_particles.erase(std::remove_if(_particles.begin(), _particles.end(), [](Particle& particle) { return particle.age() <= 0.0f || particle.size() <= 0.0f; }), _particles.end());
}

void Particles::render(Camera camera) {
	for (const Particle& particle : _particles) {
		particle.render(_graphics, camera);
	}
}