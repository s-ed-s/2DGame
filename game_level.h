#ifndef GAMELEVEL_H
#define GAMELEVEL_H
#include <vector>

#include <glad/glad.h>
#include "game_object.h"
#include "Player.h"
#include "Monster.h"


class ObjectWithAnimation {
public:
    Point coords;
    Animation object_animation;
    bool alive;

    ObjectWithAnimation(std::string &path, int x, int y, int count);

    void Draw(Image &screen);
};
// Класс GameLevel содержит все кирпичи уровней игры, 
// а также функционал для загрузки с жесткого диска информации и их рендеринга
class GameLevel
{
public:
    // Состояние уровня
    std::vector<GameObject> Bricks{};
    //Типы блоков
    std::vector<GameObject> Blocks{};

    //Монстры    
    std::vector <Monster *> Monsters;
    std::vector <ObjectWithAnimation *> AnimatedObject;


    GameObject *pol;
	unsigned int row_blocks{}, col_blocks{};
    Point PlayerPosition{.x = 0, .y = 0};
    // Конструктор
    GameLevel() {}
	
    // Загрузка уровня из файла
    void Load(const char* file);
	
    // Рендеринг уровня
    void Draw(Image &screenBuffer, float brightness);
	
    // Проверка, пройден ли уровень
    //bool IsCompleted();
    void init(std::vector<std::vector<unsigned int>> tileData);
    // Инициализация уровня данными из tileData
    ~GameLevel() {
        Bricks.clear();
        Blocks.clear();
    }
};

#endif
