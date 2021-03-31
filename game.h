#ifndef GAME_H
#define GAME_H

#include "game_level.h"
#include "common.h"

#include <GLFW/glfw3.h>
#include "glm/glm.hpp"
#include <glad/glad.h>
#include <fstream>

enum IsCollision {
    LEFT = 0,
    RIGHT = 1,
    UP = 2,
    DOWN = 3
};

const float PLAYER_VELOCITY(120.0f);


class Magic {
public:
    Point coords;
    Animation magic_animation;
    int x_speed;
    int y_speed;
    int time_to_live;
    bool alive;
    bool DropPlayer = false;
    bool DropMonster = false;

    Magic(std::string &path, int x, int y, int x_speed, int y_speed);

    void Draw(Image &screen);
};

// Класс Game содержит все относящиеся к игре состояния и необходимый функционал.
// Объединяет все данные, связанные с игрой, в один класс для простого доступа к каждому из компонентов
class Game
{
public:
    bool IsKeyQ = false;
    float deltaIsQ {};

    bool keys[1024]{}; //массив состояний кнопок - нажата/не нажата 
    GLfloat lastX = 400, lastY = 300; //исходное положение мыши
    bool firstMouse = true;
    bool captureMouse         = true;  // Мышка захвачена нашим приложением или нет?
    bool capturedMouseJustNow = false;
    int Width, Height;

    std::vector<GameLevel> Levels;
    unsigned int Level;
    float DeltaTime;

    Player *player;
    unsigned int player_money{};
    Image *screenBuffer;
    std::vector <Magic *> Magical_attack;

    std::vector <Pixel *> PlayerImg;
	
    // Конструктор
    Game(unsigned int width, unsigned int height) : Width(width), Height(height) {};
	
	// Деструктор
    ~Game();
	
    // Инициализация состояния игры (загрузка всех шейдеров/текстур/уровней)
    void Init();
	
    // Игровой цикл
    void ProcessInput(float dt);

    //Загружает анимацию игрока
    void GetPlayerImg();
    void GetImgData(Image & ImagePlayer, Pixel *img, int pos_x, int pos_y);
    void CheckPlayerImg(int x, int count);

    //Взаиможействие с монстрами
    void ProcessPlayerAndMonster();
    //Возвращает количество хитпоинтов, которые сняли монстры у игрока
    int attack();
    //Вычитает хитпоинты из здоровья игрока
    void MonstersAttack();
    
    //Обновляет данные класса
    void Update(float dt, GLFWwindow* window);
    //Рисует все объекты
    void Render();

    void DrawDigits();
    //Проверяет на смену уровня
    void CompleteLev(GLFWwindow* window);
    void ChangeLevel(GLFWwindow* window);
    //Появление уровня
    void FadeIn(GLFWwindow* window, float brightness);
    //Затухание уровня
    void FadeOut(GLFWwindow* window, float brightness);
    //определение столкновений
    void DoCollisions(float dt);
    void DoCollisionsWithMonsters(float dt);
    void DoCollisionsWithMoney(float dt);

    //Проверяет свободно ли поле
    bool IsFieldFree(Point &coords);
    void UpdateMagic(Magic *magic);
    void UpdateMagicAll();

    //Рисует монстров
    void DrawMonsters();

    //Жив ли игрок
    void IsPlayerLive();

    void DrawAnimatedObject();

    void DeleteALL();
};


#endif

