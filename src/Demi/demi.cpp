#include "Demi.h"

#include "constants.h"

#include "Game/Systems/SpriteSystems.h"
#include "Game/Systems/Systems.h"
#include "Systems.h"

Demi::Demi()
  : Game("SAMPLE", SCREEN_WIDTH * SCALE, SCREEN_HEIGHT * SCALE) { }

void Demi::setup() {
  Scene* gameplay = createGamePlayScene();

  setScene(gameplay);
}

Scene* Demi::createGamePlayScene() {
  Scene* scene = new Scene("GAMEPLAY SCENE", r);
  addSetupSystem<PhysicsSetupSystem>(scene);
  addSetupSystem<CharacterSetupSystem>(scene);
  //addSetupSystem<EnemySetupSystem>(scene);

  addSetupSystem<TilemapSetupSystem>(scene, renderer);

  addSetupSystem<SpriteSetupSystem>(scene, renderer);
  addUpdateSystem<SpriteUpdateSystem>(scene);
  addSetupSystem<CollisionEventSetupSystem>(scene, collisionEvent);
  addEventSystem<ContactEventSystem>(scene, collisionEvent);
  addUpdateSystem<HpCheckUpdateSystem>(scene);

  addUpdateSystem<PhysicsUpdateSystem>(scene);
  addUpdateSystem<MovementUpdateSystem>(scene);
  addUpdateSystem<PlayerMovementUpdateSystem>(scene);
  addEventSystem<MovementInputSystem>(scene);
  
  addRenderSystem<TilemapRenderSystem>(scene);
  addRenderSystem<SpriteRenderSystem>(scene);
  //addRenderSystem<FixtureRenderSystem>(scene);
  return scene;
}
