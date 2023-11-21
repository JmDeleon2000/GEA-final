#pragma once

#include "Game/Game.h"

class Demi : public Game {
public:
  Demi();

  void setup() override;

private:
  Uint32 collisionEvent;
  Scene* createGamePlayScene();
};

