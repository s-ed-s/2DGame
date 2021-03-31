#ifndef MAIN_PLAYER_H
#define MAIN_PLAYER_H

#include "Image.h"
#include "common.h"
#include "vector"

static Image background("resources/32x32.png");
static Image ImagePlayer("resources/player.png");
static Image ImagePlayerDiag("resources/player_diag.png");
struct Point
{
    int x;
    int y;
};
enum class MovementDir {
    UP,
    DOWN,
    LEFT, 
    RIGHT,
    ACTION,
    FIRE,
    NONE
};
enum class PlayerState {
  LIVE,
  DEAD
};
class Player
{
public:
    Player(Point pos);
    Player(const Player & other);

    bool Moved() const;
    //void ProcessInput(MovementDir dir);
    void Draw(Image &screen);

    Point coords {.x = 10, .y = 10};
    Point old_coords {.x = 10, .y = 10};
    PlayerState state = PlayerState::LIVE;
    int width = -1;
    int height = -1;
    Pixel *data = nullptr;
    Pixel color {.r = 255, .g = 255, .b = 0, .a = 255};
    int move_speed = 4;


    int hitpoints{100};

    ~Player() {
        delete data;
    }

};

#endif //MAIN_PLAYER_H
