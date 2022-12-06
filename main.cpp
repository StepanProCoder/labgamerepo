#include "app/baseapp.cpp"
#include "snake/snake.h"
#include <iostream>


class GameApp : public app::GameApp {
 public:
  GameApp(int w, int h)
      : app::GameApp(w, h),
        game_field(24, 24),
        snake({game_field.GetWidth() / 2, game_field.GetHeight() / 2}){}

 private:
  void AddApple() {
    int x = rand() % game_field.GetWidth();
    int y = rand() % game_field.GetHeight();
    while (x == snake.GetHead().x && y == snake.GetHead().y) {
      x = rand() % game_field.GetWidth();
      y = rand() % game_field.GetHeight();
    }
    auto* apple = new AppleCell;
    apple->OnVisit([this, x, y]() mutable {
      //snake.Grow(1);
      AddApple();
      game_field.SetCell(x, y, new EmptyCell);
    });
    game_field.SetCell(x, y, apple);
  }

  void SetAllEmpty(){
    for(int x = 0; x < game_field.GetWidth(); x++){
      for(int y = 0; y < game_field.GetHeight(); y++){
        auto* empty = new EmptyCell;
        empty->OnVisit([this]() mutable {
          floor = {NONE, NONE};          
        });
        game_field.SetCell(x, y, empty);
      }
    }
  }

  void SetAlmostWall(int x, int y, AlmostWallCell* almost_wall, Direction cur_floor){
    //auto* almost_wall = new AlmostWallCell;
    almost_wall->OnVisit([this, cur_floor]() mutable {      
      floor = cur_floor;
      //std::cout << floor.side_vert << "/" << floor.side_hor << std::endl;
    });
    game_field.SetCell(x, y, almost_wall);
  }

  void SetWall(int x, int y, WallCell* wall){
    //auto* wall = new WallCell;
    wall->OnVisit([this]() mutable {
      GameOver();
    });
    game_field.SetCell(x, y, wall);
  }

  void SetWalls() {
    Side corner; 

    for(int y = 0; y < game_field.GetHeight(); y++){
      SetWall(0, y, new WallCell);
      SetWall(game_field.GetWidth() - 1, y, new WallCell);
      if(y > 0 && y < game_field.GetHeight() - 1){
        SetAlmostWall(1, y, new AlmostWallCell, {NONE, LEFT});
        SetAlmostWall(game_field.GetWidth() - 2, y, new AlmostWallCell, {NONE, RIGHT});
      }
    }

    for(int x = 0; x < game_field.GetWidth(); x++){
      if(x > game_field.GetWidth()/2 - 4 && x < game_field.GetWidth()/2 + 4)
        continue;

      SetWall(x, 0, new WallCell);
      SetWall(x, game_field.GetHeight() - 1, new WallCell);
      if(x > 0 && x < game_field.GetWidth() - 1){
        if(x == 1)
          corner = LEFT;
        else if(x == game_field.GetWidth() - 2)
          corner = RIGHT;
        else
          corner = NONE;        
        SetAlmostWall(x, 1, new AlmostWallCell, {UP, corner});
        SetAlmostWall(x, game_field.GetHeight() - 2, new AlmostWallCell, {DOWN, corner});
      }
      
    }
    
  }

  void SetStartFinish(Coords start, Coords finish){
    SetAlmostWall(start.x, start.y, new StartCell, {DOWN, LEFT});
    SetWall(finish.x, finish.y, new FinishCell);
  }

  void Initialize() override {
    Coords start = {1, game_field.GetHeight() - 2};
    Coords finish = {game_field.GetWidth() - 2, 1};
    render::LoadResource("resources/images/snake_stay.jpg");
    render::LoadResource("resources/images/snake_move.jpg");
    SetAllEmpty();
    SetWalls();
    //AddApple();
    SetStartFinish(start, finish);
    snake.SetDirection({DOWN,NONE});
    snake.SetHead(start);  
  }

  double ProcessAngle(Direction cur_dir){
    switch (cur_dir.side_vert)
    {
      case UP:
        return 180;

      case DOWN:
        return 0;  

      default:
        switch (cur_dir.side_hor)
        {
          case LEFT:
            return 90;

          case RIGHT:
            return 270;

          default:
            return 0;      
        }
    }
  }

  SDL_Scancode ProcessFlipBtn(Direction cur_dir){
    switch (cur_dir.side_vert)
    {
      case UP:
        return SDL_SCANCODE_A;

      case DOWN:
        return SDL_SCANCODE_D;  

      default:
        switch (cur_dir.side_hor)
        {
          case LEFT:
            return SDL_SCANCODE_S;

          case RIGHT:
            return SDL_SCANCODE_W;

          default:
            return SDL_SCANCODE_UNKNOWN;      
        }
    }
  }

  void ProcessInput(const Uint8* keyboard, const MouseState& mouse) override {
    Direction none = {NONE, NONE};
    Direction cur_dir = none;
    bool check = true;
    if (keyboard[SDL_SCANCODE_RIGHT]) {
      cur_dir.side_hor = RIGHT;
    } else if (keyboard[SDL_SCANCODE_LEFT]) {
      cur_dir.side_hor = LEFT;
    } else if (keyboard[SDL_SCANCODE_UP]) {
      cur_dir.side_vert = UP;
    } else if (keyboard[SDL_SCANCODE_DOWN]) {      
      cur_dir.side_vert = DOWN;
    } else {
      check = false;
    }
    
    if(check)
      snake.SetAngle(ProcessAngle(cur_dir)); 

    check = true;
    
    Direction cur_dir_wasd = cur_dir;    
    if (keyboard[SDL_SCANCODE_D]) {
      cur_dir_wasd.side_hor = RIGHT;
    } else if (keyboard[SDL_SCANCODE_A]) {
      cur_dir_wasd.side_hor = LEFT;      
    } else if (keyboard[SDL_SCANCODE_W]) {
      cur_dir_wasd.side_vert = UP;      
    } else if (keyboard[SDL_SCANCODE_S]) {      
      cur_dir_wasd.side_vert = DOWN;
    }
    else {
      check = false;
    } 

    if(check){
      snake.SwapState();      
      if(keyboard[ProcessFlipBtn(floor)])
        snake.SetFlip(SDL_FLIP_HORIZONTAL);
      else
        snake.SetFlip(SDL_FLIP_NONE);
    }
    else
      snake.SetState(0);
    
    if(cur_dir_wasd != cur_dir)
      snake.StepWasd(cur_dir_wasd);
    if(cur_dir != none)
      snake.SetDirection(cur_dir);
  }

  void Render() override {
    game_field.Render();
    snake.Render();
  }

  void Update(Uint32 millis) override {
    const int kQuant = 30;
    millis_ += millis;
    if (millis_ < kQuant) {
      return;
    }
    millis_ -= kQuant;   
    

    if (Cell* cell = game_field.GetCell(snake.GetHead().x, snake.GetHead().y)) {
      cell->Visit();
    }

    snake.UpdateState(floor);

    Coords head = snake.GetHead();
    if (head.x < 0)
      head.x = game_field.GetWidth() - 1;
    if (head.x >= game_field.GetWidth())
      head.x = 0;
    if (head.y < 0)
      head.y = game_field.GetHeight() - 1;
    if (head.y >= game_field.GetHeight())
      head.y = 0;
    snake.SetHead(head);

    for (size_t i = 1; i < snake.GetUnits().size(); ++i) {
      if (snake.GetHead() == snake.GetUnits()[i]) {
        GameOver();
        return;
      }
    }
  }
  
  GameField game_field;  
  Snake snake;
  Uint32 millis_ = 0;
  Direction floor = {NONE, NONE};
};

#undef main
int main() {
  try {
    GameApp(800, 800).Run();
  } catch (std::exception& e) {
    std::cout << e.what() << std::endl;
  }
  return 0;
}