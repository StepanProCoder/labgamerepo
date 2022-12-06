#ifndef BASEAPP_CPP
#define BASEAPP_CPP
#include "baseapp.h"
#include <SDL2/SDL.h>

namespace app {

GameApp::~GameApp() = default;

void GameApp::Run() {
  Initialize();

  Uint32 time = SDL_GetTicks();

  for (bool exit = false; !exit && !is_over_;) {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
      switch (event.type) {
        case SDL_QUIT:
          exit = true;
          break;
        case SDL_WINDOWEVENT:
          if (event.window.event == SDL_WINDOWEVENT_RESIZED) {
            OnWindowResized(event.window.data1, event.window.data2);
          }
          break;
      }
    }

    Uint32 delta_time = SDL_GetTicks() - time;
    time = SDL_GetTicks();    

    if (delta_time > 0) {
      Update(delta_time);
    }

    MouseState mouse;
    mouse.buttons = SDL_GetMouseState(&mouse.x, &mouse.y);
    ProcessInput(SDL_GetKeyboardState(nullptr), mouse);

    SDL_SetRenderDrawColor(render::GetRenderer(), 0, 0, 0, 255);
    SDL_RenderClear(render::GetRenderer());

    Render();

    SDL_RenderPresent(render::GetRenderer());

    SDL_Delay(1000 / 60);
  }

  Free();

  render::FreeAllResources();
}

void GameApp::GameOver() {
  is_over_ = true;
}

}  // namespace app
#endif
