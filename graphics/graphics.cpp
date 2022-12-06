#ifndef GRAPHICS_CPP
#define GRAPHICS_CPP
#include "graphics.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include <exception>
#include <filesystem>
#include <stdexcept>
#include <unordered_map>

namespace render {

SDL_Renderer* GetRenderer() {
  if (!RenderWindow::sdl_renderer_)
    throw std::logic_error("Initialize RenderWindow first.");
  return RenderWindow::sdl_renderer_;
}

SDL_Window* RenderWindow::sdl_window_ = nullptr;
SDL_Renderer* RenderWindow::sdl_renderer_ = nullptr;

RenderWindow::RenderWindow(int width, int height)
    : width_(width), height_(height) {
  if (sdl_window_ || sdl_renderer_)
    throw std::logic_error("Renderer window already exist");

  SDL_Init(SDL_INIT_EVERYTHING);

  sdl_window_ =
      SDL_CreateWindow("GAME", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                       width_, height_, SDL_WINDOW_RESIZABLE);

  sdl_renderer_ = SDL_CreateRenderer(sdl_window_, -1, SDL_RENDERER_ACCELERATED);
}

RenderWindow::~RenderWindow() {
  SDL_DestroyRenderer(sdl_renderer_);
  SDL_DestroyWindow(sdl_window_);
  SDL_Quit();
}

}  // namespace render

namespace render {

class ResourceManager {
 public:
  static ResourceManager& GetInstance() {
    static ResourceManager instance;
    return instance;
  }

  void LoadResource(const std::filesystem::path& path,
                    const std::string& name) {
    SDL_Texture* texture =
        IMG_LoadTexture(GetRenderer(), path.string().c_str());
    if (!texture)
      throw std::invalid_argument("Can't find resource: " + path.string());
    textures_[name] = texture;
  }

  void FreeAllResources() {
    for (auto& [name, texture] : textures_) {
      SDL_DestroyTexture(texture);
    }
    textures_.clear();
  }

  SDL_Texture* GetTexture(const std::string& name) {
    auto fnd = textures_.find(name);
    if (fnd == textures_.end())
      return nullptr;
    return fnd->second;
  }

 private:
  ResourceManager() {
    if (!RenderWindow::sdl_window_)
      throw std::logic_error("Initialize RenderWindow first");
  }

  ~ResourceManager() { FreeAllResources(); }

  std::unordered_map<std::string, SDL_Texture*> textures_;
};

void LoadResource(const std::filesystem::path& path, const std::string& name) {
  if (!name.empty()) {
    ResourceManager::GetInstance().LoadResource(path, name);
  } else {
    ResourceManager::GetInstance().LoadResource(path, path.filename().string());
  }
}

void DrawImage(const std::string& name, int x, int y, int w, int h, const double angle, const SDL_RendererFlip flip) {
  auto* texture = ResourceManager::GetInstance().GetTexture(name);
  if (!texture)
    throw std::invalid_argument("Texture " + name + " is not loaded.");

  SDL_Rect rect = {x, y, w, h};
  if (w == 0 || h == 0) {
    SDL_QueryTexture(texture, nullptr, nullptr, &rect.w, &rect.h);
  }

  SDL_RenderCopyEx(GetRenderer(), texture, nullptr, &rect, 0, NULL, SDL_FLIP_NONE);
  SDL_RenderCopyEx(GetRenderer(), texture, nullptr, &rect, angle, NULL, flip);
}

void FreeAllResources() {
  ResourceManager::GetInstance().FreeAllResources();
}

const SDL_Rect* MakeRect(int x, int y, int w, int h) {
  static SDL_Rect r[8];
  static int i = -1;
  i = (i + 1) % std::size(r);
  r[i].x = x;
  r[i].y = y;
  r[i].w = w;
  r[i].h = h;
  return &r[i];
}

}  // namespace render
#endif
