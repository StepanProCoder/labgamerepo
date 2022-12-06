#ifndef SNAKE_H
#define SNAKE_H

#include "../graphics/graphics.cpp"

#include <functional>
#include <memory>
#include <vector>
#include <iostream>


class Cell {
 public:
  virtual ~Cell() = default;
  virtual void Render(int x, int y) = 0;

  void OnVisit(std::function<void()> on_visit) { this->on_visit = on_visit; }

  void Visit() {
    if (on_visit) {
      on_visit();
    }
  }

 private:
  std::function<void()> on_visit;
};

class EmptyCell : public Cell {
 public:
  void Render(int x, int y) override {}
};

class AlmostWallCell : public EmptyCell {  
};

class WallCell : public Cell{
  public:
  void Render(int x, int y) override {
    SDL_SetRenderDrawColor(render::GetRenderer(), 255, 255, 255, 255);
    SDL_RenderFillRect(render::GetRenderer(), render::MakeRect(x * 32, y * 32, 32, 32));
    SDL_SetRenderDrawColor(render::GetRenderer(), 0, 0, 0, 0);
    SDL_RenderDrawRect(render::GetRenderer(), render::MakeRect(x * 32, y * 32, 32, 32));
  }
};

class StartCell : public AlmostWallCell{
  public:
  void Render(int x, int y) override {
    SDL_SetRenderDrawColor(render::GetRenderer(), 0, 0, 0, 0);
    SDL_RenderFillRect(render::GetRenderer(), render::MakeRect(x * 32, y * 32, 32, 32));
    SDL_SetRenderDrawColor(render::GetRenderer(), 0, 150, 255, 255);
    SDL_RenderDrawRect(render::GetRenderer(), render::MakeRect(x * 32, y * 32, 32, 32));
  }
};

class FinishCell : public WallCell{
  public:
  void Render(int x, int y) override {
    SDL_SetRenderDrawColor(render::GetRenderer(), 0, 0, 0, 0);
    SDL_RenderFillRect(render::GetRenderer(), render::MakeRect(x * 32, y * 32, 32, 32));
    SDL_SetRenderDrawColor(render::GetRenderer(), 255, 87, 51, 255);
    SDL_RenderDrawRect(render::GetRenderer(), render::MakeRect(x * 32, y * 32, 32, 32));
  }
};

class AppleCell : public Cell {
 public:
  void Render(int x, int y) {
    SDL_SetRenderDrawColor(render::GetRenderer(), 255, 255, 255, 255);
    SDL_RenderDrawRect(render::GetRenderer(), render::MakeRect(x * 32, y * 32, 32, 32));
    render::DrawImage("apple.png", x * 32, y * 32,  32, 32);
  }
};

class GameField {
 public:
  GameField(int w = 24, int h = 24) {
    fields.resize(w);
    for (auto& column : fields) {
      column.resize(h);      
    }
  }

  int GetWidth() const { return fields.size(); }
  int GetHeight() const { return fields[0].size(); }

  void Render() {
    // Render background
    for (int x = 0; x < GetWidth(); ++x) {
      for (int y = 0; y < GetHeight(); ++y) {
        fields[x][y]->Render(x, y);
      }
    }
  }

  Cell* GetCell(int x, int y) const {
    if (x < 0 || x >= fields.size())
      return nullptr;
    if (y < 0 || y >= fields[x].size())
      return nullptr;
    return fields[x][y].get();
  }

  Cell* SetCell(int x, int y, Cell* cell) {
    fields[x][y].reset(cell);
    return fields[x][y].get();
  }

 private:
  std::vector<std::vector<std::unique_ptr<Cell>>> fields;
};

struct Coords {
  int x, y;

  bool operator ==(const Coords& o) const {
    return x == o.x && y == o.y;
  }
};

enum Side{
  NONE,
  UP,
  DOWN,
  LEFT,
  RIGHT,
};

struct Direction{
  Side side_vert; 
  Side side_hor; 
};

bool operator==(Direction dir1, Direction dir2){
  return dir1.side_vert == dir2.side_vert && dir1.side_hor == dir2.side_hor;
}

bool operator!=(Direction dir1, Direction dir2){
  return !operator==(dir1, dir2);
}

bool no_sides_included(Direction dir1, Direction dir2){
  return !((dir1.side_vert == dir2.side_vert && dir1.side_vert != NONE && dir2.side_vert != NONE) || (dir1.side_hor == dir2.side_hor && dir1.side_hor != NONE && dir2.side_hor != NONE));
}

class Snake {
 public:
  explicit Snake(const Coords& head) {
    direction = {NONE, NONE};
    units.push_back(head);
  }

  const std::vector<Coords>& GetUnits() const { return units; }

  void SetHead(const Coords& head) { units[0] = head; }
  const Coords& GetHead() const { return units[0]; }
  double GetAngle() const { return angle; }
  void SetAngle(const double cur_angle){ angle = cur_angle; }  
  void SetFlip(SDL_RendererFlip cur_flip){ flip = cur_flip; }
  void SetState(bool state){ pic_path = state?"snake_move.jpg":"snake_stay.jpg"; }
  void SwapState(){ pic_path == "snake_stay.jpg"?SetState(1):SetState(0); }

  void ProcessSide(Coords& new_head, Side step_side){
    switch (step_side) {
      case NONE:
        return;
      case UP:
        new_head.y--;
        break;
      case DOWN:
        new_head.y++;
        break;
      case RIGHT:
        new_head.x++;
        break;
      case LEFT:
        new_head.x--;
        break;
    }
  }

  void StepWasd(Direction wasd_dir){
    wasd_direction = wasd_dir;
    is_wasd = true;
  }

  void MakeStep(Direction step_dir){
    Coords new_head = units[0];
    ProcessSide(new_head, step_dir.side_vert);
    ProcessSide(new_head, step_dir.side_hor);
    units.insert(units.begin(), new_head);
    if (grow > 0) {
      --grow;
      return;
    }

    units.pop_back();
  }

  void SetDirection(Direction dir) {
    // if (direction != NONE && dir == NONE)
    //   return;
    // if (direction == UP && dir == DOWN)
    //   return;
    // if (direction == DOWN && dir == UP)
    //   return;
    // if (direction == LEFT && dir == RIGHT)
    //   return;
    // if (direction == RIGHT && dir == LEFT)
    //   return;
    direction = dir;
  }

  //void Grow(int count) { grow += count; }

  void UpdateState(Direction floor) {  
    //std::cout << direction.side_vert << " " << direction.side_hor << "/" << floor.side_vert << " " << floor.side_hor << std::endl;  
    if(is_wasd && no_sides_included(wasd_direction, floor)){
      MakeStep(wasd_direction);
    }
    if(no_sides_included(direction, floor) && (no_sides_included(wasd_direction, direction) || !is_wasd)){       
      MakeStep(direction);
    }    
    is_wasd = false;
  }

  void Render() {
    for (const Coords& u : units) {
      // SDL_SetRenderDrawColor(render::GetRenderer(), 255, 191, 0, 0);
      // SDL_RenderFillRect(render::GetRenderer(),
      //                    render::MakeRect(u.x * 32, u.y * 32, 32, 32));
      render::DrawImage(pic_path, u.x * 32, u.y * 32, 32, 32, angle, flip);    
    }
  }

 private:
  Direction direction;
  Direction wasd_direction;
  std::vector<Coords> units;
  double angle = 0;  
  SDL_RendererFlip flip = SDL_FLIP_NONE;
  std::string pic_path = "snake_stay.jpg";
  int grow = 0;
  bool is_wasd = false;
};
#endif
