#include "Button.hpp"

namespace Framework {
	Button::Button() {

	}

	Button::Button(Rect rect, ButtonImages images, Text text, uint8_t id) {
		_render_rect = rect;
		_collider_rect = rect;

		_images = images;
		_text = text;
		_id = id;
		// TODO: support different images for hover
	}

	Button::Button(Rect render_rect, Rect collider_rect, ButtonImages images, Text text, uint8_t id) {
		_render_rect = render_rect;
		_collider_rect = collider_rect;

		_initial_position = render_rect.position;

		_images = images;
		_text = text;
		_id = id;
		// TODO: support different images for hover
	}

	Button::ButtonState Button::state() const {
		return _state;
	}

	bool Button::pressed() const {
		return _state == ButtonState::JUST_PRESSED;
	}

	bool Button::down() const {
		return _state == ButtonState::JUST_PRESSED || _state == ButtonState::STILL_DOWN;
	}

	bool Button::hovered() const {
		return _mouse_over;
	}

	void Button::update(InputHandler* input) {
		if (_state == ButtonState::JUST_PRESSED) {
			_state = ButtonState::STILL_DOWN;
		}
		else if (_state == ButtonState::JUST_RELEASED) {
			_state = ButtonState::STILL_UP;
		}

		// THIS VERSION DOESN'T RELEASE WHEN CURSOR MOVES OFF IT
		/*if (input->just_up(MouseHandler::MouseButton::LEFT)) {
			_state = ButtonState::JUST_RELEASED;
		}
		else if (input->just_down(MouseHandler::MouseButton::LEFT)) {
			if (colliding(_collider_rect, input->mouse_position())) {
				_state = ButtonState::JUST_PRESSED;
			}
		}*/

		// THIS VERSION RELEASES WHEN CURSOR MOVES OFF IT
		_mouse_over = false;
		if (colliding(_collider_rect, input->get_mouse()->position())) {
			_mouse_over = true;
			if (input->just_up(MouseHandler::MouseButton::LEFT)) {
				_state = ButtonState::JUST_RELEASED;
			}
			else if (input->just_down(MouseHandler::MouseButton::LEFT)) {
				_state = ButtonState::JUST_PRESSED;
			}
		}
		else if (down()) {
			_state = ButtonState::JUST_RELEASED;
		}
	}

	void Button::render() const {
		// Only render image if it's been assigned
		Image* image = down() ? _images.selected : hovered() ? _images.hovered : _images.unselected;
		if (image != nullptr) image->render(_render_rect);

		_text.render(_render_rect.centre());
	}

	void Button::set_position(vec2 position) {
		_render_rect.position = position;
	}

	uint8_t Button::get_id() const {
		return _id;
	}

	vec2 Button::initial_position() const {
		return _initial_position;
	}

	vec2 Button::position() const {
		return _render_rect.position;
	}

	// Reset state if button needs to no longer be pressed, or appear as if it is no longer being pressed/hovered
	void Button::reset_state() {
		_state = ButtonState::STILL_UP;
		_mouse_over = false;
	}
}