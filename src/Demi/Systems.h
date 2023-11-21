#pragma once

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
    
    auto transform = scene->player->addComponent<TransformComponent>(0, 0, 4 * SCALE, 4 * SCALE, 45.0);
    scene->player->addComponent<SpriteComponent>(
      "demi-lich.png",
      128, 128,
      0/*start x index*/, 0/*start y index*/,
      8, 500
    );

    auto controller = scene->player->addComponent<PlayerControllerComponent>(0, 45.0, 45.0, 0.0f, 0.0f, 1000.0f, 0.0f, true);

    auto world = scene->world->get<PhysicsComponent>().b2d;

    float x = 20 * PIXELS_PER_METER; 
    float y = 62 * PIXELS_PER_METER; 
    float hx = (4.0f * PIXELS_PER_METER) / 2.0f;
    float hy = (4.0f * PIXELS_PER_METER) / 2.0f;

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

    scene->player->addComponent<LifeComponent>(10);
  }
};

class EnemySetupSystem : public SetupSystem {
public:
  void run() {
    Entity* enemy = new Entity(scene->r.create(), scene);
    enemy->addComponent<NameComponent>("ENEMY");
    
    auto transform = enemy->addComponent<TransformComponent>(0, 0, 16 * SCALE, 16 * SCALE, 0.0);
    enemy->addComponent<SpriteComponent>(
      "purple-kobold.png",
      256, 256,
      0, 0,
      1, 250
    );

    auto world = scene->world->get<PhysicsComponent>().b2d;

    float x = 60 * PIXELS_PER_METER; 
    float y = 35 * PIXELS_PER_METER; 
    float hx = (16.0f * PIXELS_PER_METER) / 2.0f;
    float hy = (16.0f * PIXELS_PER_METER) / 2.0f;

    b2BodyDef bodyDef;
    bodyDef.type = b2_staticBody;
    bodyDef.position.Set(x + hx, y + hy);

    b2Body* body = world->CreateBody(&bodyDef);

    b2PolygonShape box;
    box.SetAsBox(hx, hy);

    b2FixtureDef fixtureDef;
    fixtureDef.shape = &box;
    fixtureDef.density = 0.0000001f;
    fixtureDef.friction = 0.0f;
    
    body->CreateFixture(&fixtureDef);
    body->GetUserData().pointer = reinterpret_cast<uintptr_t>(enemy);

    enemy->addComponent<RigidBodyComponent>(
      bodyDef.type,
      body,
      transform.x,
      transform.y,
      transform.w,
      transform.h,
      SDL_Color{0, 255, 0}
    );
  }
};


class BgSetupSystem : public SetupSystem {

private:
    int x;
    int y;
    int w;
    int h;


public:
    BgSetupSystem(int pos_x, int pos_y, int width, int height)
    {
        this->x = pos_x;
        this->y = pos_y;
        this->w = width;
        this->h = height;
    }
  void run() {
    if (scene->world == nullptr) {
      scene->world = new Entity(scene->r.create(), scene);
    }
    const auto transform = scene->world->addComponent<TransformComponent>(this->x * SCALE, this->y * SCALE, 
        this->w * SCALE, this->h * SCALE, 0.0);
    scene->world->addComponent<SpriteComponent>(
      "background.png",
      440, 330,
      0, 0
    );

    auto world = scene->world->get<PhysicsComponent>().b2d;

    float x = this->x * PIXELS_PER_METER; 
    float y = this->y * PIXELS_PER_METER; 
    float hx = (this->w * PIXELS_PER_METER) / 2.0f;  // this is half width
    float hy = (this->h * PIXELS_PER_METER) / 2.0f;

    b2BodyDef bodyDef;
    bodyDef.type = b2_staticBody;
    bodyDef.position.Set(x + hx, y + hy);
    
    b2Body* body = world->CreateBody(&bodyDef);

    b2PolygonShape groundBox;
    groundBox.SetAsBox(hx, hy);

    body->CreateFixture(&groundBox, 0.0f);
    
    scene->world->addComponent<RigidBodyComponent>(
      bodyDef.type,
      body,
      transform.x,
      transform.y,
      transform.w,
      transform.h,
      SDL_Color{0, 0, 255}
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
    const auto view = scene->r.view<RigidBodyComponent, TransformComponent>();

    for (const auto e : view) {
      const auto rb = view.get<RigidBodyComponent>(e);
      auto& transform = view.get<TransformComponent>(e);
      
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
                
                rb.body->SetLinearVelocity(b2Vec2(controller.Xv * SCALE, -controller.Yv * SCALE));

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

            // Convert the Box2D position (center of the body) to screen coordinates
            float centerX = position.x * SCALE / PIXELS_PER_METER; // 20 pixels * 8 pixels / 10 meters  
            float centerY = position.y * SCALE / PIXELS_PER_METER;

            // Adjust for the entity's dimensions to get the top-left corner
            transform.x = centerX - static_cast<float>(transform.w) / 2.0f;
            transform.y = centerY - static_cast<float>(transform.h) / 2.0f;
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

class MovementInputSystem : public EventSystem {
public:
  void run(SDL_Event event) {
    if (event.type == SDL_KEYDOWN) {
      // move
      if (event.key.keysym.sym == SDLK_LEFT) {
        moveCharacter(-1);
      } else if (event.key.keysym.sym == SDLK_RIGHT) {
        moveCharacter(1);
      } else if (event.key.keysym.sym == SDLK_SPACE) {
        jumpCharacter();
      }
    } else if (event.type == SDL_KEYUP) {
      // stop movement
      if (event.key.keysym.sym == SDLK_LEFT) {
        stopCharacter();
      } else if (event.key.keysym.sym == SDLK_RIGHT) {
        stopCharacter();
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
    
    const float x = cosf((transform.angle * tau) / 360.0f);
    const float y = sinf((transform.angle * tau) / 360.0f);


    playerController.Xv = x * 4 * SCALE * PIXELS_PER_METER;
    playerController.Yv = y * 4 * SCALE * PIXELS_PER_METER;

    
    playerController.canJump = false;
    playerController.isJumping = true;
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
      if (event.user.data1 && event.user.data2) {
        print("Collision! hp reduced by one");
        auto& life = scene->player->get<LifeComponent>();
        life.hp -= 1;


        rb->SetLinearVelocity(b2Vec2(0, 0));


        Entity* firstEntity = (Entity*)event.user.data1;
        Entity* secondEntity = (Entity*)event.user.data2;
        Entity* enemy = nullptr; 

        if (firstEntity->get<NameComponent>().tag == "ENEMY") {
          enemy = firstEntity;
        }
        if (secondEntity->get<NameComponent>().tag == "ENEMY") {
          enemy = secondEntity;
        }
        if (enemy != nullptr) {
          print("PUSH PLAYER LEFT!");
          rb->ApplyForce(b2Vec2{-100.0f, 0}, rb->GetLocalCenter(), true);
        }
        
        print("Hp Remaining:", life.hp);
      }
      else 
      {
          playerController.canJump = true;
          playerController.isJumping = false;
          playerController.actualAngle = playerController.inputAngle;
          rb->SetLinearVelocity(b2Vec2(0, 0));
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


