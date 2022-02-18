#pragma once

#include "BaseStage.hpp"

#include "Curves.hpp"

class IntroStage : public Framework::BaseStage {
public:
	void start();

	bool update(float dt);
	void render();

private:
	// We're using a timer rather than a transition since we need delays in between transitions anyways
	Framework::Timer _timer;
};

class TitleStage : public Framework::BaseStage {
public:
	void start();

	bool update(float dt);
	void render();
};

class GameStage : public Framework::BaseStage {
public:
	bool update(float dt);
	void render();
};

class PausedStage : public Framework::BaseStage {
public:
	PausedStage(BaseStage* background_stage);

	bool update(float dt);
	void render();

private:
	BaseStage* _background_stage;
};