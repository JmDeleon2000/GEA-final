#include "SpriteSystems.h"
#include <fstream>

#include <SDL_stdinc.h>
#include <SDL_timer.h>
#include <print.h>
#include <constants.h>
#include <SDL.h>
#include <SDL_ttf.h>

#include "ECS/Entity.h"
#include "ECS/Components.h"
#include "Game/Graphics/TextureManager.h"


void SpriteRenderSystem::run(SDL_Renderer* renderer) {
    auto view = scene->r.view<TransformComponent, SpriteComponent>();
    for (auto entity : view) {
        const auto spriteComponent = view.get<SpriteComponent>(entity);
        const auto transformComponent = view.get<TransformComponent>(entity);
        Texture* texture = TextureManager::GetTexture(spriteComponent.name, spriteComponent.shader.name);

        const int width = spriteComponent.w > 0 ? spriteComponent.w : texture->width;
        const int height = spriteComponent.h > 0 ? spriteComponent.h : texture->height;

        SDL_Rect clip = {
            spriteComponent.xIndex * width,
            spriteComponent.yIndex * height,
            width,
            height
        };

        double angle = -transformComponent.angle;
        SDL_RendererFlip flip = SDL_FLIP_NONE;

        if (transformComponent.angle > 90.0)
        {
            flip = SDL_FLIP_HORIZONTAL;
            angle = 180-transformComponent.angle;
        }

        texture->render(
            transformComponent.x,
            transformComponent.y,
            transformComponent.w,
            transformComponent.h,
            &clip,
            angle,
            0,
            flip
        );
    }
}

SpriteSetupSystem::~SpriteSetupSystem() {
    auto view = scene->r.view<SpriteComponent>();

    for(auto entity : view) {
        const auto spriteComponent = view.get<SpriteComponent>(entity);
        TextureManager::UnloadTexture(spriteComponent.name, spriteComponent.shader.name);
    }
}

SpriteSetupSystem::SpriteSetupSystem(SDL_Renderer* renderer)
    : renderer(renderer) { }

void SpriteSetupSystem::run() {
    auto view = scene->r.view<SpriteComponent>();

    for(auto entity : view) {
        const auto spriteComponent = view.get<SpriteComponent>(entity);
        TextureManager::LoadTexture(spriteComponent.name, renderer, spriteComponent.shader);
    }
}

void SpriteUpdateSystem::run(double dT) {
    auto view = scene->r.view<SpriteComponent>();

    Uint32 now = SDL_GetTicks();

    for(auto entity : view) {
        auto& spriteComponent = view.get<SpriteComponent>(entity);

        if (spriteComponent.animationFrames > 0) {
            if (!spriteComponent.lastUpdate) {
                spriteComponent.lastUpdate = SDL_GetTicks();
                continue;
            }

            float timeSinceLastUpdate = now - spriteComponent.lastUpdate;

            if (spriteComponent.delay != 0) {
                if (timeSinceLastUpdate < spriteComponent.delay) {
                    continue;
                } else {
                    spriteComponent.delay = 0;
                    spriteComponent.lastUpdate = 0;
                    timeSinceLastUpdate = 0;
                }
            }

            int framesToUpdate = static_cast<int>(
                timeSinceLastUpdate / 
                spriteComponent.animationDuration * spriteComponent.animationFrames
            );

            if (framesToUpdate > 0) {
                if (!spriteComponent.once || spriteComponent.xIndex < (spriteComponent.animationFrames - 1)) {
                    spriteComponent.xIndex += framesToUpdate;
                    spriteComponent.xIndex %= spriteComponent.animationFrames;
                    spriteComponent.lastUpdate = now;
                }
            }
        }
    }
}












void TilemapRenderSystem::run(SDL_Renderer* renderer) {
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

TilemapSetupSystem::~TilemapSetupSystem() {
    auto view = scene->r.view<TilemapComponent>();

    for (auto entity : view) {
        const auto tilemapComponent = view.get<TilemapComponent>(entity);
        TextureManager::UnloadTexture(tilemapComponent.name);
    }
}

TilemapSetupSystem::TilemapSetupSystem(SDL_Renderer* renderer)
    : renderer(renderer) { }


void TilemapSetupSystem::run() {
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

    for (y = 0; y < 25; y++)
        for (x = 0; x < 25; x++)
            if (layout[x + y * 25])
            {
                int mask = 0;
                if (x > 0 && x < 24)
                {
                    if (layout[x - 1 + y * 25])
                        mask |= 2;
                    if (layout[x + 1 + y * 25])
                        mask |= 4;
                }
                if (y > 0 && y < 24)
                {
                    if (layout[x + (y - 1) * 25])
                        mask |= 1;
                    if (layout[x + (y + 1) * 25])
                        mask |= 8;
                }
    
    
                SDL_Rect renderQuad = { (mask % 4) * 128, (mask / 4) * 128, 128, 128 };
                canvas->drawOnTexture(tilemap->texture,
                    x * SCALE * tile_scale,
                    y * SCALE * tile_scale,
                    SCALE * tile_scale,
                    SCALE * tile_scale,
                    &renderQuad);
            }
            else
                canvas->drawOnTexture(backgroundTile->texture,
                    x * SCALE * 4,
                    y * SCALE * 4,
                    SCALE * 4,
                    SCALE * 4);

    //for (x = 0; x < SCREEN_WIDTH; x++)
    //    for(y = 0; y < SCREEN_WIDTH; y++)
    //        canvas->drawOnTexture(backgroundTile->texture,
    //            x * SCALE * 4,
    //            y * SCALE * 4,
    //            SCALE * 4,
    //            SCALE * 4);
    canvas->lockTexture();

}
