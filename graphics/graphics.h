#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <SDL2/SDL.h>

#include <filesystem>
#include <string>

namespace render {

class RenderWindow {
 public:
  RenderWindow(int width, int height);
  virtual ~RenderWindow();

 public:
  friend SDL_Renderer* GetRenderer();

  int width_ = 0;
  int height_ = 0;
  static SDL_Window* sdl_window_;
  static SDL_Renderer* sdl_renderer_;
};

SDL_Renderer* GetRenderer();

void LoadResource(const std::filesystem::path& path,
                  const std::string& name = {});
void FreeAllResources();

void DrawImage(const std::string& name, int x, int y, int w = 0, int h = 0, const double angle = 0, const SDL_RendererFlip flip = SDL_FLIP_NONE);

const SDL_Rect* MakeRect(int x, int y, int w, int h);

}  // namespace render
#endif