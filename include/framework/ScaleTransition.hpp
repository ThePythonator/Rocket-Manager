#pragma once

// Note: this is not tested

#include "BaseTransition.hpp"
#include "Curves.hpp"

namespace Framework {
	// Used for interpolating between two rects.
	class ScaleTransition : public BaseTransition {
	public:
		ScaleTransition(Graphics* graphics, float scale_time, Rect start_rect, Rect end_rect);

		void update(float dt);
		void render(); // doesn't do anything

		Rect rect();

		// Only meaningful when OPENING/CLOSING
		float percent();

	private:
		Rect _start, _end;

		float _scale_time = 0.0f;
	};
}