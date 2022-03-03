#include "GameStage.hpp"

// GameStage

bool GameStage::update(float dt) {
	transition->update(dt);

	return true;
}

void GameStage::render() {
	transition->render();
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