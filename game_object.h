#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

#include <glad/glad.h>
#include "Image.h"
#include "Player.h"


// Контейнерный объект, хранящий все состояния, относящиеся к отдельно взятой игровой сущности
class GameObject
{
public:
    // Состояние объекта
    
    int pos_x = -1;
    int pos_y = -1;
    int channels = 3;
    size_t size = 0;
    Pixel *data = nullptr;
    Pixel color {.r = 50, .g = 255, .b = 0, .a = 255};

    bool IsSolid = false;
    bool CompleteLevel = false;
    bool Destroyed = false;
    bool IsFloor = false;

    int health{100};
		
    // Конструкторы
    GameObject();
    GameObject(Point Position);
    
    GameObject(Image &img, Point &pos, Point &pos_on_back);

    GameObject(const GameObject &object);   //конструктор копирования объекта класса (для размножениея блоков)
    
    //Изменение значений x, y
    void SetPos(Point Position);
	// Отрисовка спрайта
    void Draw(Image &screen, float brightness);

    //Перезагрузка оператора ==
    friend bool operator == (const GameObject& left, const GameObject& right) {
        for (int i = 0; i < tileSize * tileSize; ++i) {
            if (left.data[i] != right.data[i]) {
                return false;
            }
        }
        return true;
    }
    
};


#endif //GAMEOBJECT_H