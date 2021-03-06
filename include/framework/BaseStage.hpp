#pragma once

#include "Constants.hpp"

#include "Input.hpp"
#include "GraphicsObjects.hpp"
#include "Button.hpp"
#include "BaseTransition.hpp"

namespace Framework {
	class BaseStage {
	public:
		BaseStage();
		virtual ~BaseStage();

		// Called only once. Use instead of constructor if you need access to graphics_objects or input
		virtual void init();

		// Called when stage becomes the current stage
		virtual void start();
		// Called when stage stops being the current stage
		virtual void end();

		virtual bool update(float dt) = 0;
		virtual void render() = 0;

		BaseStage* next();

		bool finished();
		bool delete_me();

		void _init(GraphicsObjects* _graphics_objects, InputHandler* _input);

		void set_transition(BaseTransition* _transition);
		void clear_transition();

	protected:
		void finish(BaseStage* next, bool can_delete_me = true);

		std::vector<Framework::Button> buttons;
		uint8_t button_selected = BUTTONS::NONE;

		GraphicsObjects* graphics_objects = nullptr;
		InputHandler* input = nullptr;

		BaseTransition* transition = nullptr;

	private:
		bool _finished = false;
		bool _delete_me = false;
		BaseStage* _next = nullptr;

		bool _initialised = false;
	};
}