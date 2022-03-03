#include "Camera.hpp"

Camera::Camera() {

}
Camera::Camera(Framework::vec2 window_size) {
	_window_size = window_size;
}

void Camera::set_position(Framework::vec2 position) {
	_position = position;
}
Framework::vec2 Camera::get_position() const {
	return _position;
}

void Camera::set_scale(float scale) {
	_scale = scale;
}
float Camera::get_scale() const {
	return _scale;
}

Framework::vec2 Camera::get_render_position(Framework::vec2 position) const {
	return (position - _position) * _scale + _window_size / 2;
}