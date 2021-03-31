#include <cmath>
#include <vector>
#include "Image.h"
#include "Player.h"

enum class MonsterState {
    SLEEP,
    WALK,
    ATTACK,
    DEAD,
    DYING
};

static Pixel blend(Pixel oldC, Pixel newC) {
    newC.r = newC.a / 255.0 * (newC.r - oldC.r) + oldC.r;
    newC.g = newC.a / 255.0 * (newC.g - oldC.g) + oldC.g;
    newC.b = newC.a / 255.0 * (newC.b - oldC.b) + oldC.b;
    newC.a = 255;
    return newC;
} 
class Animation {
public:
	//Вектор анимации действия
    std::vector <Image *> img;

    int img_cur;
    //текущее количество кадров, которое живет картинка
    int img_frame;
    //Количество кадров, которое может жить картинка
    int img_frame_max;

    //Определяет двигаться к концу вектора изображений или к началу
    bool to_end_vec_img;

    //конструктор
    Animation(std::string path_to_img, int img_frame, int img_frame_max);

    //Возвращает необходимый номер изображения
    Image *Draw(int curr_frame = -1);

    //Возвращает последнюю анимацию
    Image *Current();

    //Количество кадров
    int len();
    ~Animation() {
        for (int i = 0; i < img.size(); ++i) {
            delete img[i];
        }
        img.clear();
    }

};
//Дистанция между двумя точками
static float dist(Point first, Point second) {
    float x = first.x - second.x;
    float y = first.y - second.y;
    return std::sqrt(x * x + y * y);
}

class Monster {
public:
	//конструктор
    Monster(int x, int y, int type);

    //Вывод на экран
    void Draw(Image &screen);

    //Движение к игроку
    void MoveTo(Point &player);
    //Движение от игрока
    void MoveOut(Point &player);

    //Может ли атаковать
    bool IsAttack(Point &coords);

    //Состояние монстра
    MonsterState state;
    //Координаты монстра
    Point coords;
    //Координаты респауна
    Point start;
    //Направление взгляда
    MovementDir look;

    //Анимация ходьбы
    Animation walk_animation;

    //Анимация атаки
    Animation attack_animation;

    //Анимация смерти
    Animation dying_animation;

    //начало атаки
    int begin_attack;
    //текущий кадр
    int attack_frame;
    //смерть
    bool dying;

    int dying_ind;
    int dying_times;

    int type;
    int attack_points;
    int hitpoints;

    int MagicFrame{0};
    int MagicMaxFrame{3};
};