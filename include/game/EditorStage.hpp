#pragma once

#include "BaseStage.hpp"

class EditorStage : public Framework::BaseStage {
public:
	bool update(float dt);
	void render();
};