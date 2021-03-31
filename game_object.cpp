#include "game_object.h"
#include <glad/glad.h>
#include "Image.h"


GameObject::GameObject(Point Position = {.x = 0, .y = 0}) : pos_x(Position.x), pos_y(Position.y), size(tileSize * tileSize) {
        data = new Pixel[size];
        for (int i = 0; i < size; ++i) { data[i] = color; }
    }
GameObject::GameObject(Image &img, Point &pos, Point &pos_on_back) {
    pos_x = pos.x * tileSize;
    pos_y = pos.y * tileSize;
    size = tileSize * tileSize;

    data = new Pixel[size];

    int tmp_x = pos_on_back.x * tileSize;
    int tmp_y = pos_on_back.y * tileSize;
    for(int y = tmp_y; y < tmp_y + tileSize; ++y)
    {
        for(int x = tmp_x; x < tmp_x + tileSize; ++x)
        {
            //перевернем по y
            int y_new = tileSize - 1 - (y - tmp_y);
            //оставим также для x
            int x_new = x - tmp_x;
            //Заполним массив пикселей
            data[y_new * tileSize + x_new] = img.GetPixel(x, y);
        }
    }
}
GameObject::GameObject(const GameObject &object) : pos_x(object.pos_x), pos_y(object.pos_y), size(object.size), IsSolid(object.IsSolid),
            CompleteLevel(object.CompleteLevel), IsFloor(object.IsFloor) {
	data = new Pixel[size];
	for (int i = 0; i < size; ++i) { data[i] = object.data[i]; }
}
void GameObject::SetPos(Point Position) {
        pos_x = Position.x * tileSize; 
        pos_y = Position.y * tileSize;
}

void GameObject::Draw(Image &screen, float brightness)
{
    for(int y = pos_y; y < pos_y + tileSize; ++y)
    {
        for(int x = pos_x; x < pos_x + tileSize; ++x)
        {
            Pixel tmp = data[(y - pos_y) * tileSize + (x - pos_x)];
            tmp.r *= brightness;
            tmp.g *= brightness;
            tmp.b *= brightness;

            screen.PutPixel(x, y, tmp);
        }
    }
}
