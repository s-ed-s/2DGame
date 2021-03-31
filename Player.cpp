#include "Player.h"
#include <fstream>


Player::Player(Point pos = {.x = 10, .y = 10}) :coords(pos), old_coords(coords) {
    data = new Pixel[tileSize * tileSize];

    for (int y = 0 *  tileSize; y < 1 * tileSize; ++y) {
        for (int x = 7 * tileSize; x < 8 * tileSize; ++x) {
          int new_y =  tileSize - 1 - y;
          int new_x = x - 7 * tileSize;
          Pixel tmp = ImagePlayer.GetPixel(x, y);
          if (tmp.r != 255 || tmp.g != 255 || tmp.g != 255) {
              data[new_y * tileSize + new_x] = tmp;
          } else {
            data[new_y * tileSize + new_x] = Pixel{.r = 0, .g = 0, .b = 0, .a =255}; 
          }
      }
    }
}
Player::Player(const Player& other) :coords(other.coords), old_coords(other.old_coords), width(other.width), height(other.height) {
  data = new Pixel[width * height];
  for (int i = 0; i < width * height; ++i) {
    data[i] = other.data[i];
  }
}

bool Player::Moved() const
{
  if(coords.x == old_coords.x && coords.y == old_coords.y)
    return false;
  else
    return true;
}
void Player::Draw(Image &screen)
{
    if(Moved())
    {
        if (coords.y >= screen.Height() - tileSize) {
            coords.y = screen.Height() - tileSize - 1;
        }
        if (coords.x >= screen.Width() - tileSize) {
            coords.x = screen.Width() - tileSize - 1;
        }
        if (coords.y < 0) {
            coords.y = 0;
        }
        if (coords.x < 0) {
            coords.x = 0;
        }
        //draw
        /*
        for(int y = old_coords.y; y <= old_coords.y + tileSize; ++y)
        {
          for(int x = old_coords.x; x <= old_coords.x + tileSize; ++x)
          {
            screen.PutPixel(x, y, screen.GetPixel(x, y));
          }
        }*/
        old_coords = coords;
    }

    for(int y = coords.y; y < coords.y + tileSize; ++y)
    {
        for(int x = coords.x; x < coords.x + tileSize; ++x)
        {
            Pixel tmp = data[(y - coords.y) * tileSize + (x - coords.x)];
            if (tmp.r != 0 || tmp.g != 0 || tmp.r != 0) {        
                screen.PutPixel(x, y, tmp);
            } else {
               //screen.PutPixel(x, y, screen.GetPixel(x, y));
            }
        }
    }

    for (int x = 0; x * 8 < hitpoints; ++x) {
        for (int y = 0; y < 5; ++y) {
            screen.PutPixel(coords.x + tileSize / 2 + x, coords.y + tileSize + y, green);
            screen.PutPixel(coords.x + tileSize / 2 - x, coords.y + tileSize + y, green);
        }
    }
}