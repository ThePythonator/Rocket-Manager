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

	// Set transition
	set_transition(graphics_objects->transition_ptrs[GRAPHICS_OBJECTS::TRANSITIONS::FADE_TRANSITION]);
}

void TitleStage::start() {
	// Were we just in a submenu?
	if (_submenu) {
		_submenu = false;
		transition->set_open();
		
		if (button_selected == BUTTONS::TITLE::EDITOR) {
			transition->close();
		}
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
				// Stop button from appearing pressed/hovered
				button.reset_state();
				finish(new CreditsStage(this), false);
				break;

			case BUTTONS::TITLE::EDITOR:
				_submenu_die = true;
				break;

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
			finish(new EditorStage());
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

Framework::vec2 TitleStage::get_submenu_parent_position() {
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
	buttons = create_submenu_from_constants(graphics_objects, STRINGS::BUTTONS::PLAY_OPTIONS, title_stage->get_submenu_parent_position().y);

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
				// Stop button from seeming to be pressed/hovered:
				button.reset_state();
				finish(new SaveSelectStage(this), false);
				break;

			case BUTTONS::PLAY_OPTIONS::CREATE:
				// Stop button from seeming to be pressed/hovered:
				button.reset_state();
				finish(new NewSaveStage(this), false);
				break;

			case BUTTONS::PLAY_OPTIONS::BACK:
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

void PlayOptionsStage::render() {
	// Render title stage
	title_stage->render();

	// Menu bar
	graphics_objects->graphics_ptr->fill(MENU::SUBMENU_BACKGROUND_RECT, COLOURS::BLACK, MENU::BACKGROUND_ALPHA);

	// Buttons
	for (const Framework::Button& button : buttons) button.render();

	//transition->render();
}

// SaveSelectStage

SaveSelectStage::SaveSelectStage() { }
SaveSelectStage::SaveSelectStage(PlayOptionsStage* _play_options_stage) : play_options_stage(_play_options_stage) { }

void SaveSelectStage::init() {
	// Find valid save files:
	std::vector<std::string> filepaths = find_files_with_extension(PATHS::BASE_PATH + PATHS::SANDBOX_SAVES::LOCATION, PATHS::SANDBOX_SAVES::EXTENSION);

	for (std::string path : filepaths) {
		save_names.push_back(trim_extension(get_filename(path)));
	}

	// Create buttons for selecting save file
	buttons = create_menu_buttons(MENU::OVERLAY_RECT, BUTTONS::WIDE_SIZE, graphics_objects->button_image_groups[GRAPHICS_OBJECTS::BUTTON_IMAGE_GROUPS::DEFAULT], save_names, graphics_objects->font_ptrs[GRAPHICS_OBJECTS::FONTS::MAIN_FONT], COLOURS::WHITE, MENU::OVERLAY_RECT.position.y, BUTTONS::SAVE_SELECT::TOTAL);

	// Create transition
	set_transition(graphics_objects->transition_ptrs[GRAPHICS_OBJECTS::TRANSITIONS::FADE_TRANSITION]);
}

void SaveSelectStage::start() {
	transition->set_open();
}

bool SaveSelectStage::update(float dt) {
	transition->update(dt);

	// If user clicked outside menu overlay, then go back
	if (input->just_down(Framework::MouseHandler::MouseButton::LEFT) && !Framework::colliding(MENU::OVERLAY_RECT, input->get_mouse()->position())) {
		finish(play_options_stage);
	}

	// Update buttons
	for (Framework::Button& button : buttons) {
		button.update(input);

		if (button.pressed() && transition->is_open()) {
			button_selected = button.get_id();

			transition->close();
		}
	}

	if (transition->is_closed()) {
		// delete play_options_stage ptr??

		// Start sandbox!
		finish(new GameStage(save_names[button_selected]));
	}

	return true;
}

void SaveSelectStage::render() {
	// Render play options stage
	play_options_stage->render();

	// Menu overlay
	graphics_objects->graphics_ptr->fill(MENU::OVERLAY_RECT, COLOURS::WHITE, 0x60);
	graphics_objects->graphics_ptr->fill(MENU::OVERLAY_RECT, COLOURS::ATMOSPHERES[GAME::SANDBOX::BODIES::ID::EARTH], 0x80); // TODO change
	graphics_objects->graphics_ptr->fill(MENU::OVERLAY_RECT, COLOURS::BLACK, MENU::OVERLAY_ALPHA);
	graphics_objects->graphics_ptr->render_rect(MENU::OVERLAY_RECT, Framework::Colour(COLOURS::WHITE, MENU::BORDER_ALPHA));

	// Buttons
	for (const Framework::Button& button : buttons) button.render();

	transition->render();
}

// NewSaveStage

NewSaveStage::NewSaveStage() { }
NewSaveStage::NewSaveStage(PlayOptionsStage* _play_options_stage) : play_options_stage(_play_options_stage) { }

void NewSaveStage::init() {
	// Find valid save files:
	std::vector<std::string> filepaths = find_files_with_extension(PATHS::BASE_PATH + PATHS::SANDBOX_SAVES::LOCATION, PATHS::SANDBOX_SAVES::EXTENSION);

	for (std::string path : filepaths) {
		save_names.push_back(trim_extension(get_filename(path)));
	}
	
	// Create button to cycle random names
	Framework::Text text = Framework::Text(graphics_objects->font_ptrs[GRAPHICS_OBJECTS::FONTS::MAIN_FONT], STRINGS::BUTTONS::NEW_SAVE[BUTTONS::NEW_SAVE::NAME], COLOURS::WHITE);
	Framework::Button button = Framework::Button(Framework::Rect(MENU::OVERLAY_RECT.topleft(), BUTTONS::WIDE_SIZE), graphics_objects->button_image_groups[GRAPHICS_OBJECTS::BUTTON_IMAGE_GROUPS::DEFAULT], text, BUTTONS::NEW_SAVE::NAME);
	button.set_text(get_new_button_name());
	buttons.push_back(button);

	// Create 'create' button
	text = Framework::Text(graphics_objects->font_ptrs[GRAPHICS_OBJECTS::FONTS::MAIN_FONT], STRINGS::BUTTONS::NEW_SAVE[BUTTONS::NEW_SAVE::CREATE], COLOURS::WHITE);
	button = Framework::Button(Framework::Rect(MENU::OVERLAY_RECT.bottomleft() - Framework::vec2{ 0.0f, BUTTONS::WIDE_SIZE.y }, BUTTONS::WIDE_SIZE), graphics_objects->button_image_groups[GRAPHICS_OBJECTS::BUTTON_IMAGE_GROUPS::DEFAULT], text, BUTTONS::NEW_SAVE::CREATE);
	buttons.push_back(button);

	// Create transition
	set_transition(graphics_objects->transition_ptrs[GRAPHICS_OBJECTS::TRANSITIONS::FADE_TRANSITION]);
}

void NewSaveStage::start() {
	transition->set_open();
}

bool NewSaveStage::update(float dt) {
	transition->update(dt);

	// If user clicked outside menu overlay, then go back
	if (input->just_down(Framework::MouseHandler::MouseButton::LEFT) && !Framework::colliding(MENU::OVERLAY_RECT, input->get_mouse()->position())) {
		finish(play_options_stage);
	}

	// Update buttons
	for (Framework::Button& button : buttons) {
		button.update(input);

		if (button.pressed() && transition->is_open()) {
			button_selected = button.get_id();

			if (button_selected == BUTTONS::NEW_SAVE::NAME) {
				button.set_text(get_new_button_name());
			}
			else if (button_selected == BUTTONS::NEW_SAVE::CREATE) {
				transition->close();
			}
		}
	}

	if (transition->is_closed()) {
		// Next stage!
		finish(new GameStage(buttons[BUTTONS::NEW_SAVE::NAME].get_text()));
	}

	return true;
}

void NewSaveStage::render() {
	// Render play options stage
	play_options_stage->render();

	// Menu overlay
	//graphics_objects->graphics_ptr->fill(COLOURS::BLACK, MENU::OVERLAY_BACKGROUND_ALPHA);

	graphics_objects->graphics_ptr->fill(MENU::OVERLAY_RECT, COLOURS::WHITE, 0x60);
	graphics_objects->graphics_ptr->fill(MENU::OVERLAY_RECT, COLOURS::ATMOSPHERES[GAME::SANDBOX::BODIES::ID::EARTH], 0x80); // TODO change
	graphics_objects->graphics_ptr->fill(MENU::OVERLAY_RECT, COLOURS::BLACK, MENU::OVERLAY_ALPHA);
	graphics_objects->graphics_ptr->render_rect(MENU::OVERLAY_RECT, Framework::Colour(COLOURS::WHITE, MENU::BORDER_ALPHA));

	Framework::vec2 button_height = { 0.0f, BUTTONS::WIDE_SIZE.y };
	graphics_objects->graphics_ptr->render_line(MENU::OVERLAY_RECT.bottomleft() - button_height, MENU::OVERLAY_RECT.bottomright() - button_height, Framework::Colour(COLOURS::WHITE, MENU::BORDER_ALPHA));

	// Buttons
	for (const Framework::Button& button : buttons) button.render();

	transition->render();
}

std::string NewSaveStage::get_new_button_name() {
	// Randomise for now
	std::string name = random_save_name();
	while (std::count(save_names.begin(), save_names.end(), name)) {
		name = random_save_name();
	}
	return name;
}

// SettingsStage

SettingsStage::SettingsStage() { }
SettingsStage::SettingsStage(TitleStage* _title_stage) : title_stage(_title_stage) { }

void SettingsStage::init() {
	// Create buttons
	buttons = create_submenu_from_constants(graphics_objects, STRINGS::BUTTONS::SETTINGS, title_stage->get_submenu_parent_position().y);

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
	// Create credits

	Framework::vec2 left_position = MENU::WIDE_OVERLAY_RECT.topleft() + Framework::vec2{ 0.0f, BUTTONS::VERY_WIDE_SIZE.y / 2 };
	Framework::vec2 right_position = MENU::WIDE_OVERLAY_RECT.topright() + Framework::vec2{ 0.0f, BUTTONS::VERY_WIDE_SIZE.y / 2 };

	for (uint8_t i = 0; i < STRINGS::CREDITS.size(); i++) {
		credits[left_position + BUTTONS::OFFSET] = Framework::Text(graphics_objects->font_ptrs[GRAPHICS_OBJECTS::FONTS::MAIN_FONT], STRINGS::CREDITS[i].first, COLOURS::WHITE, Framework::Font::CENTER_LEFT);

		for (uint8_t j = 0; j < STRINGS::CREDITS[i].second.size(); j++) {
			credits[right_position - BUTTONS::OFFSET] = Framework::Text(graphics_objects->font_ptrs[GRAPHICS_OBJECTS::FONTS::MAIN_FONT], STRINGS::CREDITS[i].second[j], COLOURS::LIGHT_GREY, Framework::Font::CENTER_RIGHT);

			left_position.y += BUTTONS::VERY_WIDE_SIZE.y;
			right_position.y += BUTTONS::VERY_WIDE_SIZE.y;
		}
	}


	// Create GitHub repo URL button
	// Generation of location of button is a bit hacky
	Framework::Text text = Framework::Text(graphics_objects->font_ptrs[GRAPHICS_OBJECTS::FONTS::MAIN_FONT], "", COLOURS::WHITE);
	buttons.push_back(Framework::Button(Framework::Rect(left_position - Framework::vec2{ 0.0f, BUTTONS::VERY_WIDE_SIZE.y * 1.5f }, BUTTONS::VERY_WIDE_SIZE), graphics_objects->button_image_groups[GRAPHICS_OBJECTS::BUTTON_IMAGE_GROUPS::DEFAULT], text, BUTTONS::CREDITS::GITHUB));
	
	// Create transition
	set_transition(graphics_objects->transition_ptrs[GRAPHICS_OBJECTS::TRANSITIONS::FADE_TRANSITION]);
}

void CreditsStage::start() {
	transition->set_open();
}

bool CreditsStage::update(float dt) {
	transition->update(dt);

	// If user clicked outside menu overlay, then go back
	if (input->just_down(Framework::MouseHandler::MouseButton::LEFT) && !Framework::colliding(MENU::OVERLAY_RECT, input->get_mouse()->position())) {
		finish(title_stage);
	}

	// Update buttons
	for (Framework::Button& button : buttons) {
		button.update(input);

		if (button.pressed() && transition->is_open()) {
			button_selected = button.get_id();

			switch (button_selected) {
			case BUTTONS::CREDITS::GITHUB:
				Framework::URLHandler::open_url(STRINGS::REPO_URL);
				break;

			default:
				break;
			}
		}
	}

	return true;
}

void CreditsStage::render() {
	// Render play options stage
	title_stage->render();

	// Menu overlay
	graphics_objects->graphics_ptr->fill(MENU::WIDE_OVERLAY_RECT, COLOURS::WHITE, 0x60);
	graphics_objects->graphics_ptr->fill(MENU::WIDE_OVERLAY_RECT, COLOURS::ATMOSPHERES[GAME::SANDBOX::BODIES::ID::EARTH], 0x80); // TODO change
	graphics_objects->graphics_ptr->fill(MENU::WIDE_OVERLAY_RECT, COLOURS::BLACK, MENU::OVERLAY_ALPHA);
	graphics_objects->graphics_ptr->render_rect(MENU::WIDE_OVERLAY_RECT, Framework::Colour(COLOURS::WHITE, MENU::BORDER_ALPHA));

	// Buttons
	for (const Framework::Button& button : buttons) button.render();

	for (const std::pair<Framework::vec2, Framework::Text>& p : credits) {
		p.second.render(p.first);
	}

	transition->render();
}