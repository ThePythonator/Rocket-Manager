#pragma once

#include "BaseStage.hpp"

#include "Curves.hpp"

#include "GameUtilities.hpp"
#include "AttractMode.hpp"

//#include "GameStage.hpp"
#include "ProtoGame.hpp"
#include "EditorStage.hpp"

#include "Settings.hpp"

#include "URL.hpp"

class IntroStage : public Framework::BaseStage {
public:
	void init();

	bool update(float dt);
	void render();

private:
	// We're using a timer rather than a transition since we need delays in between transitions anyways
	Framework::Timer _timer;
};

class TitleStage : public Framework::BaseStage {
public:
	void init();

	void start();

	bool update(float dt);
	void render();

	Framework::vec2 get_submenu_parent_position();

	bool submenu_must_die();
	void submenu_killed();

	BaseStage* get_finish_target();

private:
	void sub_menu_init();

	AttractMode _attract = nullptr;

	bool _submenu = false;
	bool _submenu_die = false;
};

class PlayOptionsStage : public Framework::BaseStage {
public:
	PlayOptionsStage();
	PlayOptionsStage(TitleStage* _title_stage);

	void init();

	void start();

	bool update(float dt);
	void render();

private:
	TitleStage* title_stage = nullptr;
};

class SaveSelectStage : public Framework::BaseStage {
public:
	SaveSelectStage();
	SaveSelectStage(PlayOptionsStage* _play_options_stage);

	void init();

	void start();

	bool update(float dt);
	void render();

private:
	PlayOptionsStage* play_options_stage = nullptr;

	std::vector<std::string> save_names;
};

class NewSaveStage : public Framework::BaseStage {
public:
	NewSaveStage();
	NewSaveStage(PlayOptionsStage* _play_options_stage);

	void init();

	void start();

	bool update(float dt);
	void render();

private:
	std::string get_new_button_name();

	PlayOptionsStage* play_options_stage = nullptr;

	std::vector<std::string> save_names;
};

class SettingsStage : public Framework::BaseStage {
public:
	SettingsStage();
	SettingsStage(TitleStage* _title_stage);

	void init();

	void start();

	bool update(float dt);
	void render();

private:
	TitleStage* title_stage = nullptr;
};

class GameSettingsStage : public Framework::BaseStage {
public:
	GameSettingsStage();
	GameSettingsStage(SettingsStage* settings_stage);

	void init();

	void start();
	void end();

	bool update(float dt);
	void render();

private:
	SettingsStage* _settings_stage = nullptr;

	Settings _settings;
};

class GraphicsSettingsStage : public Framework::BaseStage {
public:
	GraphicsSettingsStage();
	GraphicsSettingsStage(SettingsStage* settings_stage);

	void init();

	void start();
	void end();

	bool update(float dt);
	void render();

private:
	SettingsStage* _settings_stage = nullptr;

	Settings _settings;
};

class CreditsStage : public Framework::BaseStage {
public:
	CreditsStage();
	CreditsStage(TitleStage* _title_stage);

	void init();

	void start();

	bool update(float dt);
	void render();

private:
	TitleStage* title_stage = nullptr;

	std::map<Framework::vec2, Framework::Text> credits;
};