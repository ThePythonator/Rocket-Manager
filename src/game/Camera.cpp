#include "Camera.hpp"

Camera::Camera() {

}
Camera::Camera(Framework::vec2 window_size) {
	_window_size = PhysicsEngine::to_phyvec(window_size);
}

void Camera::set_position(phyvec position) {
	_position = position;
}
phyvec Camera::get_position() const {
	return _position;
}

void Camera::set_scale(phyflt scale) {
	_scale = scale;
}
phyflt Camera::get_scale() const {
	return _scale;
}

//Framework::vec2 Camera::get_render_position(const Framework::vec2& position) const {
//	return (position - _position) * _scale + _window_size / 2;
//}

phyvec Camera::get_render_position(const phyvec& position) const {
	return (position - _position) * _scale + _window_size / 2;
}

// Essentially the inverse of get_render_position
phyvec Camera::get_position_from_render_position(const phyvec& render_position) const {
	return _position + (render_position - _window_size / 2) / _scale;
}