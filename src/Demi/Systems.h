#pragma once
#include <fstream>
#include "Game/Graphics/TextureManager.h"
#include "Game/ContactListener.h"
#include "Demi/Components.h"
#include "constants.h"

#include "ECS/Components.h"
#include "ECS/Entity.h"
#include "ECS/System.h"
#include <SDL_events.h>
#include <SDL_keycode.h>
#include <SDL_render.h>
#include <SDL_stdinc.h>
#include <box2d/b2_body.h>
#include <box2d/b2_fixture.h>
#include <box2d/b2_math.h>
#include <box2d/b2_world.h>
#include <cmath>
#include <cstdint>

#define tau 6.283185307179586


class CharacterSetupSystem : public SetupSystem {
public:
  void run() {
    scene->player = new Entity(scene->r.create(), scene);
    scene->player->addComponent<NameComponent>("PLAYER");
    
    auto transform = scene->player->addComponent<TransformComponent>(0, 0, 3 * SCALE, 3 * SCALE, 45.0);
    scene->player->addComponent<SpriteComponent>(
      "demi-lich.png",
      128, 128,
      0/*start x index*/, 0/*start y index*/,
      8, 2000
    );

    auto controller = scene->player->addComponent<PlayerControllerComponent>(0, 45.0, 45.0, 0.0f, 
        0.0f, 2000.0f, SDL_GetTicks64(), true, false, false);

    auto world = scene->world->get<PhysicsComponent>().b2d;

    float x = 20 * PIXELS_PER_METER; 
    float y = 80 * PIXELS_PER_METER; 
    float hx = (2.0f * PIXELS_PER_METER) / 2.0f;
    float hy = (2.0f * PIXELS_PER_METER) / 2.0f;

    b2BodyDef bodyDef;
    bodyDef.type = b2_dynamicBody;
    bodyDef.position.Set(x + hx, y + hy);

    b2Body* body = world->CreateBody(&bodyDef);

    b2PolygonShape box;
    box.SetAsBox(hx, hy);

    b2FixtureDef fixtureDef;
    fixtureDef.shape = &box;
    fixtureDef.density = 0.0000001f;
    fixtureDef.friction = 0.0f;
    
    body->CreateFixture(&fixtureDef);

    body->GetUserData().pointer = reinterpret_cast<uintptr_t>(scene->player);

    scene->player->addComponent<RigidBodyComponent>(
      bodyDef.type,
      body,
      transform.x,
      transform.y,
      transform.w,
      transform.h,
      SDL_Color{0, 255, 0}
    );

    scene->player->addComponent<LifeComponent>(1);
  }
};

class EnemySetupSystem : public SetupSystem {
public:
    EnemySetupSystem(int x_position, int y_position) :  x_pos(x_position), y_pos(y_position)
    {

    }
  void run() {
    Entity* enemy = new Entity(scene->r.create(), scene);
    enemy->addComponent<NameComponent>("ENEMY");
    
    auto transform = enemy->addComponent<TransformComponent>(0, 0, 5 * SCALE, 5 * SCALE, 0.0);
    enemy->addComponent<SpriteComponent>(
      "purple-kobold.png",
      256, 256,
      0, 0,
      2, 500
    );

    auto world = scene->world->get<PhysicsComponent>().b2d;

    float x = x_pos * SCALE * 4;
    float y = y_pos * SCALE * 4; 
    float hx = (2.0f * PIXELS_PER_METER) / 2.0f;
    float hy = (3.5f * PIXELS_PER_METER) / 2.0f;

    enemy->addComponent<EnemyComponent>(false, true, (int)(x - 0.5 * SCALE * 4),
                                              (int)(x + 0.5 * SCALE * 4));
    
    b2BodyDef bodyDef;
    bodyDef.type = b2_kinematicBody;
    bodyDef.position.Set(x + hx, y + hy);

    b2Body* body = world->CreateBody(&bodyDef);

    b2PolygonShape box;
    box.SetAsBox(hx, hy);

    b2FixtureDef fixtureDef;
    fixtureDef.shape = &box;
    fixtureDef.density = 0.0000001f;
    fixtureDef.friction = 0.0f;
    
    b2Fixture* fixture = body->CreateFixture(&fixtureDef);
    body->GetUserData().pointer = reinterpret_cast<uintptr_t>(enemy);

    enemy->addComponent<RigidBodyComponent>(
        bodyDef.type,
        body,
        transform.x,
        transform.y,
        transform.w,
        transform.h,
        SDL_Color{ 0, 255, 0 },
        1.0f,
        0.3f,
        fixture
    );
  }
private:
    int x_pos;
    int y_pos;
};


class BulletSetupSystem : public SetupSystem {
public:
    void run() {
        scene->bullet = new Entity(scene->r.create(), scene);
        scene->bullet->addComponent<NameComponent>("BULLET");

        scene->bullet->addComponent<BulletComponent>(0.0, false);

        auto transform = scene->bullet->addComponent<TransformComponent>(0, 0, 1 * SCALE, 1 * SCALE, 0.0);
        scene->bullet->addComponent<SpriteComponent>(
            "Laser.png",
            256, 256,
            0, 0,
            2, 500
        );

        auto world = scene->world->get<PhysicsComponent>().b2d;

        float x = 50 * SCALE * 4;
        float y = 50 * SCALE * 4;
        float hx = (1.0f * PIXELS_PER_METER) / 2.0f;
        float hy = (1.0f * PIXELS_PER_METER) / 2.0f;

        b2BodyDef bodyDef;
        bodyDef.type = b2_dynamicBody;
        bodyDef.position.Set(x + hx, y + hy);
        bodyDef.bullet = true;

        b2Body* body = world->CreateBody(&bodyDef);

        b2PolygonShape box;
        box.SetAsBox(hx, hy);

        b2FixtureDef fixtureDef;
        fixtureDef.shape = &box;
        fixtureDef.density = 0.0000001f;
        fixtureDef.friction = 0.0f;

        body->CreateFixture(&fixtureDef);
        body->GetUserData().pointer = reinterpret_cast<uintptr_t>(scene->bullet);
        body->SetAwake(false);

        scene->bullet->addComponent<RigidBodyComponent>(
            bodyDef.type,
            body,
            transform.x,
            transform.y,
            transform.w,
            transform.h,
            SDL_Color{ 0, 255, 0 }
        );
    }
};

class BgColorSystem : public RenderSystem {
public:
  void run(SDL_Renderer* renderer) {
    SDL_SetRenderDrawColor(renderer, 155, 155, 155, 255);
    SDL_Rect rect = {0, 0, SCREEN_WIDTH * SCALE, SCREEN_HEIGHT * SCALE};
    SDL_RenderFillRect(renderer, &rect);
  }
};

class PhysicsSetupSystem : public SetupSystem {
public:
  void run() {
    if (scene->world == nullptr) {
      scene->world = new Entity(scene->r.create(), scene);
    }
    b2Vec2 gravity(0.0f, 50.0f * PIXELS_PER_METER);  // 100 p/m    9.8 m/s * 10 p/m = 98 p/s;
    scene->world->addComponent<PhysicsComponent>(new b2World(gravity));
  }
};

class PhysicsUpdateSystem : public UpdateSystem {
public:
  void run(double dT) {
    const int velocityIterations = 6;
    const int positionIterations = 2;

    auto world = scene->world->get<PhysicsComponent>().b2d;
    world->Step(dT, velocityIterations, positionIterations);
  }
};

class MovementUpdateSystem : public UpdateSystem {
public:
  void run(double dT) {
    const auto view = scene->r.view<RigidBodyComponent, TransformComponent, NameComponent>();

    for (const auto e : view) {
      const auto rb = view.get<RigidBodyComponent>(e);
      auto& transform = view.get<TransformComponent>(e);
      auto& name = view.get<NameComponent>(e);


      
      b2Vec2 position = rb.body->GetPosition(); // x, y meters


      // Convert the Box2D position (center of the body) to screen coordinates
      float centerX = position.x * SCALE / PIXELS_PER_METER; // 20 pixels * 8 pixels / 10 meters  
      float centerY = position.y * SCALE / PIXELS_PER_METER;

      // Adjust for the entity's dimensions to get the top-left corner
      transform.x = centerX - static_cast<float>(transform.w)/2.0f;
      transform.y = centerY - static_cast<float>(transform.h)/2.0f;
    }
  }
};


class EnemyUpdateSystem : public UpdateSystem {
public:
    void run(double dT) {
        const auto view = scene->r.view<RigidBodyComponent, EnemyComponent, SpriteComponent>();

        for (const auto e : view) {
            const auto rb = view.get<RigidBodyComponent>(e);
            auto& controller = view.get<EnemyComponent>(e);
            auto& sprite = view.get<SpriteComponent>(e);

            b2Vec2 position = rb.body->GetPosition(); // x, y meters


            // Convert the Box2D position (center of the body) to screen coordinates
            float centerX = position.x * SCALE / PIXELS_PER_METER;  
            float centerY = position.y * SCALE / PIXELS_PER_METER;

            if (controller.going_right)
            {
                if (centerX < controller.right_limit)
                {
                    rb.body->SetLinearVelocity(b2Vec2(0.5 * SCALE * PIXELS_PER_METER, 0));
                }
                else
                {
                    rb.body->SetLinearVelocity(b2Vec2(0, 0));
                    controller.going_right = false;
                    sprite.yIndex = 1;
                }
            }
            else
            {
                if (centerX > controller.left_limit)
                    rb.body->SetLinearVelocity(b2Vec2(-0.5 * SCALE * PIXELS_PER_METER, 0));
                else
                {
                    rb.body->SetLinearVelocity(b2Vec2(0, 0));
                    controller.going_right = true;
                    sprite.yIndex = 0;
                }
            }
        }
    }
};

class EnemyDeathCheckUpSystem : public UpdateSystem {
public:
    void run(double dT) {
        const auto view = scene->r.view<RigidBodyComponent, EnemyComponent>();
        int aliveAmount = 0;
        for (const auto e : view) {
            const auto rb = view.get<RigidBodyComponent>(e);
            auto& controller = view.get<EnemyComponent>(e);

            if (controller.isDead)
            {
                rb.body->DestroyFixture(rb.fixture);
                scene->r.destroy(e);
                continue;
            }
            aliveAmount += 1;
        }
        if (aliveAmount < 1)
        {
            print("You got rid of all the kobolds");
            print("But why are you in this situation?");
            exit(1);
        }
    }
};

class PlayerMovementUpdateSystem : public UpdateSystem {
public:
    void run(double dT) {
        const auto view = scene->r.view<RigidBodyComponent, TransformComponent, PlayerControllerComponent>();

        for (const auto e : view) {
            const auto rb = view.get<RigidBodyComponent>(e);
            auto& transform = view.get<TransformComponent>(e);
            auto& controller = view.get<PlayerControllerComponent>(e);

            b2Vec2 position = rb.body->GetPosition(); // x, y meters
            
            if (controller.isJumping)
            {
                auto grav = scene->world->get<PhysicsComponent>().b2d->GetGravity();
                controller.Yv -= grav.y * dT;
                if (controller.inputAngle < 90)
                    controller.actualAngle = atanf(controller.Yv / controller.Xv) * 360 / tau;
                else
                    controller.actualAngle = 180 - atanf(controller.Yv / (-controller.Xv)) * 360 / tau;
                
                rb.body->SetLinearVelocity(b2Vec2(controller.Xv,
                    -controller.Yv));

            }

            if (controller.inputDirection && controller.canJump)
            {
                if (controller.inputDirection > 0)
                    controller.inputAngle -= 180 * dT;
                if (controller.inputDirection < 0)
                    controller.inputAngle += 180 * dT;
                if (controller.inputAngle < 45)
                    controller.inputAngle = 45;
                if (controller.inputAngle > 135)
                    controller.inputAngle = 135;
                controller.actualAngle = controller.inputAngle;
            }
            transform.angle = controller.actualAngle;
        }
    }
};

class BulletUpdateSystem : public UpdateSystem {
public:
    void run(double dT) {
        const auto view = scene->r.view<RigidBodyComponent, BulletComponent>();

        for (const auto e : view) {
            const auto rb = view.get<RigidBodyComponent>(e);
            auto& controller = view.get<BulletComponent>(e);

            if (!controller.onFlight) continue;
            
            const float x = cosf(((float)(controller.inputAngle) * tau) / 360.0f);
            const float y = sinf(((float)(controller.inputAngle) * tau) / 360.0f);
            rb.body->SetLinearVelocity(b2Vec2(x * 500 * SCALE * PIXELS_PER_METER, 
                                         -y * 500 * SCALE * PIXELS_PER_METER));
        }
    }
};

class FixtureRenderSystem : public RenderSystem {
public:
  void run(SDL_Renderer* renderer) {
    auto world = scene->world->get<PhysicsComponent>().b2d;

    for (b2Body* body = world->GetBodyList(); body != nullptr ; body = body->GetNext()) {
      for (b2Fixture* fixture = body->GetFixtureList(); fixture; fixture = fixture->GetNext()) {
        b2Shape* shape = fixture->GetShape();

        if (fixture->GetType() == b2Shape::e_polygon)
        {
          b2PolygonShape* polygonShape = (b2PolygonShape*)fixture->GetShape();

          // Ensure the rectangle has 4 vertices
          int vertexCount = polygonShape->m_count;
          if(vertexCount == 4)
          {
            // Considering vertices[1] and vertices[3] are opposite corners of the rectangle.
            b2Vec2 vertex1 = polygonShape->m_vertices[1];
            b2Vec2 vertex3 = polygonShape->m_vertices[3];

            // Calculating the width and height in meters
            float width = abs(vertex1.x - vertex3.x);
            float height = abs(vertex1.y - vertex3.y);

            // Getting the position of the body (center of mass) in meters
            b2Body* body = fixture->GetBody();
            b2Vec2 position = body->GetPosition();

            // Convert the values into pixel coordinates
            int PPM = PIXELS_PER_METER; // Pixels Per Meter scale
            float x_px = position.x / PPM;   // m  / (m / p)
            float y_px = position.y / PPM;
            float width_px = width / PPM;
            float height_px = height / PPM;

            // Create an SDL_Rect and initialize it
            SDL_Rect renderRect;
            renderRect.x = x_px - width_px/2.0f;  // Adjust for center of mass
            renderRect.y = y_px - height_px/2.0f; // Adjust for center of mass
            renderRect.w = width_px;
            renderRect.h = height_px;

            renderRect.x *= SCALE;
            renderRect.y *= SCALE;
            renderRect.w *= SCALE;
            renderRect.h *= SCALE;

            SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
            SDL_RenderDrawRect(renderer, &renderRect);
          }
        }
      }
    }
  }
};

class PlayerInputSystem : public EventSystem {
public:
  void run(SDL_Event event) {
    if (event.type == SDL_KEYDOWN) {
        switch (event.key.keysym.sym)
        {
        case SDLK_a:
            moveCharacter(-1);
            break;
        case SDLK_d:
            moveCharacter(1);
            break;
        case SDLK_SPACE:
            jumpCharacter();
            break;
        case SDLK_f:
            shootLaser();
            break;
        }
    } else if (event.type == SDL_KEYUP) {
        switch (event.key.keysym.sym)
        {
        case SDLK_a:
            stopCharacter();
            break;
        case SDLK_d:
            stopCharacter();
            break;
        }
    }
  }

private:
  float hForceMagnitude = 1000.0f * SCALE * PIXELS_PER_METER;
  float vForceMagnitude = 50000.0f * SCALE * PIXELS_PER_METER;

  void moveCharacter(int direction) {
      auto& playerController = scene->player->get<PlayerControllerComponent>();
      if (playerController.canJump)
        playerController.inputDirection = direction;
  }

  void stopCharacter() {
       auto& playerController = scene->player->get<PlayerControllerComponent>();
    playerController.inputDirection = 0;
  }


  void jumpCharacter() {
      auto& playerController = scene->player->get<PlayerControllerComponent>();
      auto& transform = scene->player->get<TransformComponent>();
    const auto playerBody = scene->player->get<RigidBodyComponent>().body;
    if (!playerController.canJump) return;
    
    const float x = cosf(((float)(playerController.inputAngle) * tau) / 360.0f);
    const float y = sinf(((float)(playerController.inputAngle) * tau) / 360.0f);


    playerController.Xv = x * 6 * SCALE * PIXELS_PER_METER;
    playerController.Yv = y * 6 * SCALE * PIXELS_PER_METER;

    
    playerController.canJump = false;
    playerController.isJumping = true;
  }

  void shootLaser() {
      auto& playerController = scene->player->get<PlayerControllerComponent>();
      auto& sprite = scene->player->get<SpriteComponent>();
      const auto playerBody = scene->player->get<RigidBodyComponent>().body;

      Uint64 now = SDL_GetTicks64();
      float elapsed = now - playerController.ticksSinceLaser;
      if (playerController.laserCD > elapsed) return;
      playerController.ticksSinceLaser = now;
      playerController.isShooting = true;

      sprite.yIndex = 1;
      sprite.animationDuration = 50;

      print("shoot!");
      auto& bulletRB = scene->bullet->get<RigidBodyComponent>().body;

      auto bodyPos = playerBody->GetTransform().p;
      float new_pos_x = bodyPos.x;
      float new_pos_y = bodyPos.y;

      float new_vel_x;
      float new_vel_y;

      auto& bulletController = scene->bullet->get<BulletComponent>();

      bulletController.onFlight = true;
      if (playerController.isJumping)
          bulletController.inputAngle = playerController.actualAngle;
      else
          bulletController.inputAngle = playerController.inputAngle;

      new_vel_x = cosf(((float)(bulletController.inputAngle) * tau) / 360.0f);
      new_vel_y = sinf(((float)(bulletController.inputAngle) * tau) / 360.0f);
      
      bulletRB->SetTransform(b2Vec2(new_pos_x + new_vel_x * 3*SCALE, new_pos_y - new_vel_y * 3*SCALE), 0);
  }
};





class CollisionEventSetupSystem : public SetupSystem {
public:
  CollisionEventSetupSystem(Uint32& collisionEvent)
    : collisionEvent(collisionEvent) { }

  void run() override {
    collisionEvent = SDL_RegisterEvents(1);  

    auto world = scene->world->get<PhysicsComponent>().b2d;
    world->SetContactListener(new ContactListener(collisionEvent));
    
  }
private:
  Uint32& collisionEvent;
};

class ContactEventSystem :  public EventSystem {
public:
  ContactEventSystem(Uint32& collisionEvent)
    : collisionEvent(collisionEvent) {}

  void run(SDL_Event event) {
    if (event.type == collisionEvent) {
        auto& playerController = scene->player->get<PlayerControllerComponent>();
        auto rb = scene->player->get<RigidBodyComponent>().body;

        std::string firstTag = "";
        std::string secondTag = "";

        Entity* firstEntity = (Entity*)event.user.data1;
        Entity* secondEntity = (Entity*)event.user.data2;
        bool playerCollided = false;
        bool enemyCollided = false;
        bool bulletCollided = false;

        if(event.user.data1)
            firstTag  = firstEntity->get<NameComponent>().tag;
        if (event.user.data2)
            secondTag = secondEntity->get<NameComponent>().tag;

        auto velocity = rb->GetLinearVelocity();

        if (firstTag == "PLAYER" || secondTag == "PLAYER")
        {
            playerController.canJump = true;
            playerController.isJumping = false;
            playerController.actualAngle = playerController.inputAngle;
            rb->SetLinearVelocity(b2Vec2(0, 0));
            playerCollided = true;
        }
        if (firstTag == "BULLET" || secondTag == "BULLET") {
            print("Bullet collided");
            auto bulletRB = scene->bullet->get<RigidBodyComponent>().body;
            bulletRB->SetTransform(b2Vec2(50 * SCALE * 4, 50 * SCALE * 4), 0);
            bulletRB->SetAwake(false);
            bulletCollided = true;
            scene->bullet->get<BulletComponent>().onFlight = false;

            auto& playerSprite = scene->player->get<SpriteComponent>();
            playerSprite.yIndex = 0;
            playerSprite.animationDuration = 2000.0f;
        }

        if (firstTag == "ENEMY" || secondTag == "ENEMY") {
            enemyCollided = true;
        }

        if (enemyCollided && bulletCollided) {
            if (firstTag == "ENEMY") {
                firstEntity->get<EnemyComponent>().isDead = true;
                //scene->r.destroy((Entity*)firstEntity);
            }
            if (secondTag == "ENEMY") {
                secondEntity->get<EnemyComponent>().isDead = true;
            }
        }

        if (enemyCollided && playerCollided) {
          rb->ApplyForce(b2Vec2{-velocity.x, -velocity.y}, rb->GetLocalCenter(), true);
          print("Collision! hp reduced by one");
          auto& life = scene->player->get<LifeComponent>();
          life.hp -= 1;
        print("Hp Remaining:", life.hp);
        }
        

      


      
    }
  }
private:
  Uint32& collisionEvent;
};

class HpCheckUpdateSystem : public UpdateSystem {
public:
  void run(double dT) {
    auto& life = scene->player->get<LifeComponent>();
    if (life.hp <= 0) {
      print("YOU DIED");
      exit(1);
    }
  }
};


class TilemapSetupSystem : public SetupSystem {
public:
    TilemapSetupSystem(SDL_Renderer* renderer)
        : renderer(renderer) { }
    ~TilemapSetupSystem() {
        auto view = scene->r.view<TilemapComponent>();

        for (auto entity : view) {
            const auto tilemapComponent = view.get<TilemapComponent>(entity);
            TextureManager::UnloadTexture(tilemapComponent.name);
        }
    }

    void run() override {
        using namespace std;
        auto& tilemapComponent = scene->world->get<TilemapComponent>();
        int width = SCREEN_WIDTH;
        int height = SCREEN_HEIGHT;
        int size = 128;
        int tile_scale = 4;
        tilemapComponent.name = "Background";

        Texture* tilemap = TextureManager::LoadTexture("tiles/tiles.png", renderer);
        Texture* backgroundTile = TextureManager::LoadTexture("tiles/books.png", renderer);

        Texture* canvas = TextureManager::MakeEmpty("Background", renderer,
            SCREEN_WIDTH * SCALE, SCREEN_HEIGHT * SCALE, 255, 255, 255);
        canvas->unlockTexture();
        int x;
        int y = 0;
        bool layout[25 * 25] = { 0 };
        ifstream layoutFile("assets/tiles/foo.csv", ios::in);
        string line;
        while (getline(layoutFile, line))
        {
            x = 0;
            while (!line.empty())
            {
                char b = line.front();
                line.erase(0, 2);
                layout[x + y * 25] = b == '1';
                x++;
            }
            y++;
        }

        auto world = scene->world->get<PhysicsComponent>().b2d;

        for (y = 0; y < 25; y++)
            for (x = 0; x < 25; x++)
            {
                canvas->drawOnTexture(backgroundTile->texture,
                    x * SCALE * 4,
                    y * SCALE * 4,
                    SCALE * 4,
                    SCALE * 4);
                if (layout[x + y * 25])
                {
                    int mask = 0;
                    if (x == 0 || layout[x - 1 + y * 25])
                        mask |= 2;
                    if (x == 24 || layout[x + 1 + y * 25])
                        mask |= 4;

                    if (y == 0 || layout[x + (y - 1) * 25])
                        mask |= 1;
                    if (y == 24 || layout[x + (y + 1) * 25])
                        mask |= 8;


                    float tile_x = x * SCALE * tile_scale;
                    float tile_y = y * SCALE * tile_scale;
                    float tile_w = SCALE * tile_scale / 2;
                    float tile_h = SCALE * tile_scale / 2;

                    SDL_Rect renderQuad = { (mask % 4) * 128, (mask / 4) * 128, 128, 128 };
                    canvas->drawOnTexture(tilemap->texture,
                        tile_x,
                        tile_y,
                        tile_w * 2,
                        tile_h * 2,
                        &renderQuad);

                    b2BodyDef bodyDef;
                    bodyDef.type = b2_staticBody;
                    bodyDef.position.Set(tile_x + tile_w, tile_y + tile_h);

                    b2Body* body = world->CreateBody(&bodyDef);

                    b2PolygonShape groundBox;
                    groundBox.SetAsBox(tile_w, tile_h);

                    body->CreateFixture(&groundBox, 0.0f);

                    scene->world->addComponent<RigidBodyComponent>(
                        bodyDef.type,
                        body,
                        (int)tile_x,
                        (int)tile_y,
                        (int)tile_w,
                        (int)tile_h,
                        SDL_Color{ 0, 0, 255 }
                    );
                }
            }
                    
        canvas->lockTexture();
    }

private:
    SDL_Renderer* renderer;
};

class TilemapRenderSystem : public RenderSystem {
public:
 
    void run(SDL_Renderer* renderer) override {
        auto view = scene->r.view<TilemapComponent>();
        for (auto entity : view) {
            const auto tilemapComponent = view.get<TilemapComponent>(entity);
            Texture* texture = TextureManager::GetTexture(tilemapComponent.name);

            texture->render(
                0,
                0,
                SCALE * SCREEN_WIDTH,
                SCALE * SCREEN_HEIGHT
            );
        }
    }
};