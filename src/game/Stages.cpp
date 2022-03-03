#include "Stages.hpp"

// IntroStage

void IntroStage::init() {
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

void TitleStage::init() {
	// Initialise 'attract mode'
	_attract = AttractMode(graphics_objects);

	// Create buttons
	buttons = create_menu_from_constants(graphics_objects, STRINGS::BUTTONS::TITLE);

	// Start transition
	set_transition(graphics_objects->transition_ptrs[GRAPHICS_OBJECTS::TRANSITIONS::FADE_TRANSITION]);
}

void TitleStage::start() {
	// Were we just in a submenu?
	if (_submenu) {
		transition->set_open();
		_submenu = false;
	}
	else {
		transition->open();
	}

	_submenu_die = false;
}

bool TitleStage::update(float dt) {
	transition->update(dt);

	// Update 'attract mode' background
	_attract.update(dt);

	// Update buttons
	for (Framework::Button& button : buttons) {
		button.update(input);

		if (button.pressed() && transition->is_open()) {
			button_selected = button.get_id();

			// Next stage!
			switch (button_selected) {
			case BUTTONS::TITLE::PLAY:
				sub_menu_init();
				finish(new PlayOptionsStage(this), false);
				break;

			case BUTTONS::TITLE::SETTINGS:
				sub_menu_init();
				finish(new SettingsStage(this), false);
				break;

			case BUTTONS::TITLE::CREDITS:
				sub_menu_init();
				finish(new CreditsStage(this), false);
				break;

			case BUTTONS::TITLE::EDITOR:
			case BUTTONS::TITLE::QUIT:
				transition->close();
				break;

			default:
				break;
			}
		}
	}

	if (transition->is_closed()) {
		// Next stage!
		switch (button_selected) {
		case BUTTONS::TITLE::EDITOR:
			//finish(new EditorStage());
			break;

		case BUTTONS::TITLE::QUIT:
			// Returning false causes program to exit
			return false;

		default:
			break;
		}
	}

	return true;
}

void TitleStage::render() {
	// Render 'attract mode' background
	_attract.render();

	// Menu bar
	graphics_objects->graphics_ptr->fill(MENU::BACKGROUND_RECT, COLOURS::BLACK, MENU::BACKGROUND_ALPHA);

	// Buttons
	for (const Framework::Button& button : buttons) button.render();

	// Transition
	if (!_submenu) transition->render();
}

Framework::vec2 TitleStage::get_submenu_root_position() {
	return buttons[button_selected].position();
}

bool TitleStage::submenu_must_die() {
	return _submenu_die;
}

void TitleStage::submenu_killed() {
	_submenu_die = false;
}

Framework::BaseStage* TitleStage::get_finish_target() {
	switch (button_selected) {
	case BUTTONS::TITLE::PLAY:
		sub_menu_init();
		return new PlayOptionsStage(this);

	case BUTTONS::TITLE::SETTINGS:
		sub_menu_init();
		return new SettingsStage(this);

	case BUTTONS::TITLE::CREDITS:
		sub_menu_init();
		return new CreditsStage(this);

	case BUTTONS::TITLE::EDITOR:
	case BUTTONS::TITLE::QUIT:
		// We want to show the transitions, so go back here first!
		return this;

	default:
		// Should never happen!
		return this;
	}
}

// Handle anything we need before starting a submenu
void TitleStage::sub_menu_init() {
	_submenu_die = _submenu;

	_submenu = true;
}

// PlayOptionsStage

PlayOptionsStage::PlayOptionsStage() { }
PlayOptionsStage::PlayOptionsStage(TitleStage* _title_stage) : title_stage(_title_stage) { }

void PlayOptionsStage::init() {
	// Create buttons
	buttons = create_submenu_from_constants(graphics_objects, STRINGS::BUTTONS::PLAY_OPTIONS, title_stage->get_submenu_root_position().y);

	// Create transition
	set_transition(graphics_objects->transition_ptrs[GRAPHICS_OBJECTS::TRANSITIONS::FADE_TRANSITION]);
}

void PlayOptionsStage::start() {
	transition->set_open();
}

bool PlayOptionsStage::update(float dt) {
	bool running = title_stage->update(dt);

	// Update buttons
	for (Framework::Button& button : buttons) {
		button.update(input);

		if (button.pressed() && transition->is_open()) {
			button_selected = button.get_id();

			// Next stage!
			switch (button_selected) {
			case BUTTONS::PLAY_OPTIONS::PLAY:
			case BUTTONS::PLAY_OPTIONS::CREATE:
				transition->close();
				break;

			case BUTTONS::PLAY_OPTIONS::BACK:
				finish(title_stage);
				break;

			default:
				break;
			}
		}
	}

	if (transition->is_closed()) {
		// Next stage!
		switch (button_selected) {
		case BUTTONS::PLAY_OPTIONS::PLAY:
			// TODO: delete title_stage???

			finish(new GameStage()); // TEMP: remove later?
			break;

		default:
			break;
		}
	}

	// If we need to kill this submenu, we need to know where to go next
	if (title_stage->submenu_must_die()) {
		finish(title_stage->get_finish_target());
		// Tell the title stage that this submenu has been killed
		title_stage->submenu_killed();
	}

	return running;
}

void PlayOptionsStage::render() {
	// Render title stage
	title_stage->render();

	// Menu bar
	graphics_objects->graphics_ptr->fill(MENU::SUBMENU_BACKGROUND_RECT, COLOURS::BLACK, MENU::BACKGROUND_ALPHA);

	// Buttons
	for (const Framework::Button& button : buttons) button.render();

	transition->render();
}

// SettingsStage

SettingsStage::SettingsStage() { }
SettingsStage::SettingsStage(TitleStage* _title_stage) : title_stage(_title_stage) { }

void SettingsStage::init() {
	// Create buttons
	buttons = create_submenu_from_constants(graphics_objects, STRINGS::BUTTONS::SETTINGS, title_stage->get_submenu_root_position().y);

	// Create transition
	set_transition(graphics_objects->transition_ptrs[GRAPHICS_OBJECTS::TRANSITIONS::FADE_TRANSITION]);
}

void SettingsStage::start() {
	transition->set_open();
}

bool SettingsStage::update(float dt) {
	bool running = title_stage->update(dt);

	// Update buttons
	for (Framework::Button& button : buttons) {
		button.update(input);

		if (button.pressed() && transition->is_open()) {
			button_selected = button.get_id();

			// Next stage!
			switch (button_selected) {
			/*case BUTTONS::SETTINGS::PLAY:
			case BUTTONS::SETTINGS::CREATE:
				transition->close();
				break;*/

			case BUTTONS::SETTINGS::BACK:
				finish(title_stage);
				break;

			default:
				break;
			}
		}
	}

	// If we need to kill this submenu, we need to know where to go next
	if (title_stage->submenu_must_die()) {
		finish(title_stage->get_finish_target());
		// Tell the title stage that this submenu has been killed
		title_stage->submenu_killed();
	}

	return running;
}

void SettingsStage::render() {
	// Render title stage
	title_stage->render();

	// Menu bar
	graphics_objects->graphics_ptr->fill(MENU::SUBMENU_BACKGROUND_RECT, COLOURS::BLACK, MENU::BACKGROUND_ALPHA);

	// Buttons
	for (const Framework::Button& button : buttons) button.render();

	transition->render();
}

// CreditsStage

CreditsStage::CreditsStage() { }
CreditsStage::CreditsStage(TitleStage* _title_stage) : title_stage(_title_stage) { }

void CreditsStage::init() {
	// Create buttons
	buttons = create_submenu_from_constants(graphics_objects, STRINGS::BUTTONS::CREDITS, title_stage->get_submenu_root_position().y);

	// Create transition
	set_transition(graphics_objects->transition_ptrs[GRAPHICS_OBJECTS::TRANSITIONS::FADE_TRANSITION]);
}

void CreditsStage::start() {
	transition->set_open();
}

bool CreditsStage::update(float dt) {
	bool running = title_stage->update(dt);

	// Update buttons
	for (Framework::Button& button : buttons) {
		button.update(input);

		if (button.pressed() && transition->is_open()) {
			button_selected = button.get_id();

			// Next stage!
			switch (button_selected) {
			/*case BUTTONS::CREDITS::PLAY:
			case BUTTONS::PLAY_OPTIONS::CREATE:
				transition->close();
				break;*/

			case BUTTONS::CREDITS::BACK:
				finish(title_stage);
				break;

			default:
				break;
			}
		}
	}

	// If we need to kill this submenu, we need to know where to go next
	if (title_stage->submenu_must_die()) {
		finish(title_stage->get_finish_target());
		// Tell the title stage that this submenu has been killed
		title_stage->submenu_killed();
	}

	return running;
}

void CreditsStage::render() {
	// Render title stage
	title_stage->render();

	// Menu bar
	graphics_objects->graphics_ptr->fill(MENU::SUBMENU_BACKGROUND_RECT, COLOURS::BLACK, MENU::BACKGROUND_ALPHA);

	// Buttons
	for (const Framework::Button& button : buttons) button.render();

	transition->render();
}