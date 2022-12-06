#ifndef BASEAPP_H
#define BASEAPP_H
#include "../graphics/graphics.cpp"

namespace app {

class GameApp : public render::RenderWindow {
 public:
  struct MouseState {
    int x = 0;
    int y = 0;
    Uint32 buttons = 0;
  };

  using render::RenderWindow::RenderWindow;
  ~GameApp() override;

  void Run();
  void GameOver();

 private:
  virtual void Initialize() {}
  virtual void Free() {}
  virtual void Update(Uint32 millis ) {}
  virtual void Render() {}
  virtual void ProcessInput(const Uint8* keyboard, const MouseState& mouse) {}
  virtual void OnWindowResized(int width, int height) {}

  bool is_over_ = false;
};

}  // namespace app
#endif
