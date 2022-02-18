#include "Stages.hpp"

// IntroStage

void IntroStage::start() {
	// Start timer for fading
	_timer.start();
}

bool IntroStage::update(float dt) {
	_timer.update(dt);

	if (_timer.time() > TIMINGS::INTRO::CUMULATIVE::FADE_OUT) {
		// Finished, start main title scene
		finish(new TitleStage());
	}

	return true;
}

void IntroStage::render() {
	graphics_objects->graphics_ptr->fill(COLOURS::BLACK);

	Framework::Image* logo_ptr = graphics_objects->image_ptrs[GRAPHICS_OBJECTS::IMAGES::LOGO];
	Framework::vec2 logo_scaled_size = logo_ptr->get_size() * SCALES::LOGO;

	logo_ptr->render(Framework::Rect(WINDOW::SIZE_HALF - logo_scaled_size / 2, logo_scaled_size));

	if (_timer.time() < TIMINGS::INTRO::CUMULATIVE::INITIAL_DELAY) {
		// All black
		graphics_objects->graphics_ptr->fill(COLOURS::BLACK);
	}
	else if (_timer.time() < TIMINGS::INTRO::CUMULATIVE::FADE_IN) {
		// Fade in
		graphics_objects->graphics_ptr->fill(COLOURS::BLACK, Framework::Curves::linear(0xFF, 0x00, (_timer.time() - TIMINGS::INTRO::CUMULATIVE::INITIAL_DELAY) / TIMINGS::INTRO::DURATION::FADE_LENGTH));
	}
	else if (_timer.time() < TIMINGS::INTRO::CUMULATIVE::INTRO_DELAY) {
		// No overlay
	}
	else if (_timer.time() < TIMINGS::INTRO::CUMULATIVE::FADE_OUT) {
		// Fade out
		graphics_objects->graphics_ptr->fill(COLOURS::BLACK, Framework::Curves::linear(0x00, 0xFF, (_timer.time() - TIMINGS::INTRO::CUMULATIVE::INTRO_DELAY) / TIMINGS::INTRO::DURATION::FADE_LENGTH));
	}
	else {
		// All black
		graphics_objects->graphics_ptr->fill(COLOURS::BLACK);
	}
}

// TitleStage

void TitleStage::start() {
	// Start transition
	set_transition(graphics_objects->transition_ptrs[GRAPHICS_OBJECTS::TRANSITIONS::FADE_TRANSITION]);
	transition->open();
}

bool TitleStage::update(float dt) {
	transition->update(dt);

	// Update buttons
	for (Framework::Button& button : buttons) {
		button.update(input);

		if (button.pressed() && transition->is_open()) {
			button_selected = button.get_id();
			transition->close();
		}
	}

	if (transition->is_closed()) {
		// Next stage!
		switch (button_selected) {
		case BUTTONS::TITLE::PLAY:
			//finish(new GameStage());
			break;

		case BUTTONS::TITLE::SETTINGS:
			//finish(new SettingsStage());
			break;

		case BUTTONS::TITLE::QUIT:
			// Returning false causes program to exit
			return false;
			// (so we don't need the break)

		default:
			break;
		}
	}

	return true;
}

void TitleStage::render() {
	graphics_objects->graphics_ptr->fill(COLOURS::WHITE);

	graphics_objects->graphics_ptr->render_circle(Framework::Vec(20, 20), 10, COLOURS::BLACK);
	graphics_objects->graphics_ptr->render_rect(Framework::Rect(40, 20, 10, 30), COLOURS::BLACK);

	graphics_objects->spritesheet_ptrs[GRAPHICS_OBJECTS::SPRITESHEETS::MAIN_SPRITESHEET]->sprite(0, Framework::Vec(64, 48));

	graphics_objects->spritesheet_ptrs[GRAPHICS_OBJECTS::SPRITESHEETS::MAIN_SPRITESHEET]->sprite(2, Framework::Vec(32, 48), Framework::SpriteTransform::ROTATE_90_ACW);

	graphics_objects->spritesheet_ptrs[GRAPHICS_OBJECTS::SPRITESHEETS::MAIN_SPRITESHEET]->sprite(1, Framework::Vec(96, 48), SPRITES::SCALE, SDL_GetTicks() / 10);

	transition->render(graphics_objects->graphics_ptr);
}

// GameStage

bool GameStage::update(float dt) {
	transition->update(dt);

	return true;
}

void GameStage::render() {
	transition->render(graphics_objects->graphics_ptr);
}

// PausedStage

PausedStage::PausedStage(BaseStage* background_stage) : BaseStage() {
	// Save the background stage so we can still render it, and then go back to it when done
	_background_stage = background_stage;
}

bool PausedStage::update(float dt) {
	transition->update(dt);

	if (input->just_down(Framework::KeyHandler::Key::ESCAPE) || input->just_down(Framework::KeyHandler::Key::P)) {
		transition->close();
	}

	if (transition->is_closed()) {
		if (button_selected == BUTTONS::PAUSED::EXIT) {
			delete _background_stage;
			finish(new TitleStage());
		}
		else {
			// Return to game (exit pause)
			finish(_background_stage);
		}
	}

	return true;
}

void PausedStage::render() {
	// Render background stage
	_background_stage->render();

	// Render pause menu
}