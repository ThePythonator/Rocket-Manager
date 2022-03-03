#include "ScaleTransition.hpp"

namespace Framework {
	ScaleTransition::ScaleTransition(Graphics* graphics, float scale_time, Rect start_rect, Rect end_rect) : BaseTransition(graphics) {
		_scale_time = scale_time;

		_start = start_rect;
		_end = end_rect;
	}

	void ScaleTransition::update(float dt) {
		if (_state == TransitionState::OPENING || _state == TransitionState::CLOSING) {
			_timer.update(dt);

			if (_timer.time() > _scale_time) {
				_state = _state == TransitionState::OPENING ? TransitionState::OPEN : TransitionState::CLOSED;
			}
		}
	}

	void ScaleTransition::render() {
		// Doesn't do anything
	}

	Rect ScaleTransition::rect() {
		// lerp centres and sizes
		vec2 centre = Curves::linear(_start.centre(), _end.centre(), percent());
		vec2 size = Curves::linear(_start.size, _end.size, percent());
		return Rect(centre - size / 2, size);
	}

	float ScaleTransition::percent() {
		return _timer.time() / _scale_time;
	}
}