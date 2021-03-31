#include "game.h"
#include "game_object.h"
#include "Player.h"
#include "common.h"


#include "tuple"
#include "fstream"

#include <cmath> 


Game::~Game()
{
    delete player;
}
void Game::DrawAnimatedObject() {
    for (int i = 0; i < this->Levels[Level].AnimatedObject.size(); ++i) {
        this->Levels[Level].AnimatedObject[i]->Draw(*screenBuffer);
    }
}
void Game::DrawDigits() {
    std::string digit_name = std::to_string(player_money);
    digit_name = "resources/elems/digits/" + digit_name + ".png";
    Image digit_img(digit_name);
    int x_shift = 2 * tileSize;
    int y_shift = 0;
    int width = digit_img.Width();
    int height = digit_img.Height();
    for (int x = 0; x < width; ++x) {
        for (int y = 0; y < height; ++y) {
            screenBuffer->PutPixel(x_shift + x, this->Height - y_shift - y, digit_img.GetPixel(x, y));
        }
    }
}
Magic::Magic(std::string &path, int x, int y, int x_speed, int y_speed): x_speed(x_speed), y_speed(y_speed), 
        magic_animation(path, 4, 3) {
    coords.x = x;
    coords.y = y;
    alive = true;
    time_to_live = 100;
}
void Magic::Draw(Image &screen) {
    Image *img = magic_animation.Draw();
    for (int x = 0; x < img->width; ++x) {
        for (int y = 0; y < img->height; ++y) {   
            Pixel tmp = img->GetPixel(x, y);
            if (tmp.r != 0 || tmp.g != 0 || tmp.b != 0) {
                if (tmp.r != 255 || tmp.g != 255 || tmp.b != 255) {
                    screen.PutPixel(coords.x + x, coords.y + y, img->GetPixel(x, y));
                }
            }
        }
    }
}
void Game::UpdateMagicAll() {
    for (int i = 0; i < Magical_attack.size(); ++i) {
        Magic *magic = Magical_attack[i];
        UpdateMagic(magic);
        if (!magic->alive) {
            delete magic;
            Magical_attack.erase(Magical_attack.begin() + i);
        } else {            
            magic->Draw(*screenBuffer);
        }
    }
}
void Game::UpdateMagic(Magic * magic) {
    magic->time_to_live--;
    if (magic->time_to_live <= 0) {
        magic->alive = false;
        return;
    }

    magic->coords.x += magic->x_speed;
    magic->coords.y += magic->y_speed;

    Point center;
    center.x = magic->coords.x + magic->magic_animation.img[0]->width / 2;
    center.y = magic->coords.y + magic->magic_animation.img[0]->height / 2;
    if (!IsFieldFree(center)) {
        magic->alive = false;
        return;
    }
    Point heart;
    if (!magic->DropMonster) {
        for (int i = 0; i < this->Levels[this->Level].Monsters.size(); ++i) {
            /*
            if (this->Monsters[i]->state != MonsterState::WALK && 
                    this->Monsters[i]->state != MonsterState::SLEEP) {
                continue;
            }*/
            heart.x = this->Levels[this->Level].Monsters[i]->coords.x + tileSize / 2;
            heart.y = this->Levels[this->Level].Monsters[i]->coords.y + tileSize / 2;
            if (dist(center, heart) < tileSize * 2 / 3) {
                magic->alive = false;
                this->Levels[this->Level].Monsters[i]->hitpoints -= 25;
                if (this->Levels[this->Level].Monsters[i]->hitpoints <= 0) {
                    this->Levels[this->Level].Monsters[i]->state = MonsterState::DYING;
                }
                break;
            }
        }
    }
    Point center_player{.x = player->coords.x + tileSize / 2 , .y = player->coords.y + tileSize / 2};
    if (!magic->DropPlayer && dist(center_player, center) < tileSize / 2) {
        magic->alive = false;
        player->hitpoints -= 25;
    }
}
bool Game::IsFieldFree(Point &coords) {
    int x = coords.x / tileSize;
    int y = coords.y / tileSize;
    if (x < 0 || y < 0) {
        return false;
    }
    if (x >= this->Levels[Level].row_blocks || y >= this->Levels[Level].col_blocks) {
        return false;
    }
    return this->Levels[this->Level].Bricks[y * this->Levels[this->Level].row_blocks + x] == this->Levels[this->Level].Blocks[1];
}
float MaxDeltaTime = 0.35f;

void Game::CheckPlayerImg(int x, int count) {   
    bool IsFind = true;
    for (int i = x; i < x + count; ++i) {
        if (player->data == PlayerImg[i]) {
            IsFind = false;
        }
    }
    if (IsFind) {
        this->DeltaTime = 0;
        player->data = PlayerImg[x + 1];
    }
    if (this->DeltaTime > MaxDeltaTime) {
        for (int i = x; i < x + count; ++i) {
            if (player->data == PlayerImg[i]) {
                player->data = PlayerImg[(i + 1) % count + x];
                this->DeltaTime = 0;
                break;
            }
        }
    }

}
void Game::ProcessInput(float dt)
{
        float velocity = PLAYER_VELOCITY * dt;
        this->DeltaTime += dt;
        this->deltaIsQ += dt;

		bool A = this->keys[GLFW_KEY_A];
        bool S = this->keys[GLFW_KEY_S];
        bool D = this->keys[GLFW_KEY_D];
        bool W = this->keys[GLFW_KEY_W];


        if (A && S || A && W || D && S || D && W) {
            velocity *= std::sqrt(2.0f) / 2.0f;
        }

        if (this->IsKeyQ && deltaIsQ > 1.0f) {   
            IsKeyQ = false;    
            deltaIsQ = 0;

            int magic_ball_x = player->coords.x + tileSize / 2;  
            int magic_ball_y = player->coords.y + tileSize / 2;

            int xspeed = lastX - player->coords.x;
            int yspeed = (1024 - lastY) - player->coords.y;
        
            int speed = std::sqrt(xspeed * xspeed + yspeed * yspeed);
            std::string path = "resources/player/sword/";
            Magic *magical_ball = new Magic(path, magic_ball_x, magic_ball_y, (xspeed * 7 / speed), (yspeed * 7/ speed));
            magical_ball->DropPlayer = true;
            this->Magical_attack.push_back(magical_ball);
        }
        // Перемещение игрока
        if (this->keys[GLFW_KEY_A])
        {
            if (player->coords.x >= 0.0f) {
                player->coords.x -= velocity;
            }            
        }
        if (this->keys[GLFW_KEY_D])
        {
            if (player->coords.x <= this->Width - player->width) {
                player->coords.x += velocity;
            }
        }
        if (this->keys[GLFW_KEY_S])
        {
            if (player->coords.y >= 0.0f) {
                player->coords.y -= velocity;
            }
        }
        if (this->keys[GLFW_KEY_W])
        {
            if (player->coords.y <= this->Width - player->height) {
                player->coords.y += velocity;
            }
        }
        //перемещение по диагонали
        if (W && A) {
            CheckPlayerImg(16, 4);
        } else if (W && D) {
            CheckPlayerImg(20, 4);
        } else if (S && D) {            
            CheckPlayerImg(24, 4);
        } else if (S && A) {            
            CheckPlayerImg(28, 4);
        } else if (S) {
            CheckPlayerImg(0, 4);
        } else if (W) {
            CheckPlayerImg(4, 4);
        } else if (D) {
            CheckPlayerImg(8, 4);            
        } else if (A) {
            CheckPlayerImg(12, 4);            
        } else {
            player->data = PlayerImg[1];
        }
}
void Game::GetImgData(Image & ImagePlayer, Pixel *img, int pos_x, int pos_y) { 
    for (int y = pos_y * tileSize; y < (pos_y + 1) * tileSize; ++y) {
        for (int x = pos_x * tileSize; x < (pos_x + 1)* tileSize; ++x) {
            int new_y = tileSize - 1 - (y - pos_y * tileSize);
            int new_x = x - pos_x * tileSize;

            Pixel tmp = ImagePlayer.GetPixel(x, y);

            if (tmp.r != 255 || tmp.g != 255 || tmp.g != 255) {
                img[new_y * tileSize + new_x] = tmp;
            } else {
                img[new_y * tileSize + new_x] = Pixel{.r = 0, .g = 0, .b = 0, .a =255}; 
            }
        }
    }
}
void Game::Init() {
	screenBuffer = new Image(Width, Height, 4);
    //Загружаем урвони
    this->Levels.clear();
	GameLevel level_one; level_one.Load("levels/one.lvl");
    GameLevel level_two; level_two.Load("levels/two.lvl");

  	this->Levels.push_back(level_one);
    this->Levels.push_back(level_two);
  	this->Level = 0;

    //Создаем игрока
    player = new Player(Point{.x = Height / 2, .y = Width / 2});
    player->coords = this->Levels[this->Level].PlayerPosition;
    GetPlayerImg();
}
void Game::DrawMonsters() {
    for (int i = 0; i < this->Levels[this->Level].Monsters.size(); ++i) {
        this->Levels[this->Level].Monsters[i]->Draw(*screenBuffer);
    }
}
void Game::Render()
{
	Levels[this->Level].Draw(*screenBuffer, 1.0f);
    UpdateMagicAll();
    DrawMonsters();
    DrawDigits();
    DrawAnimatedObject();
    player->Draw(*screenBuffer);
    glDrawPixels (Width, Height, GL_RGBA, GL_UNSIGNED_BYTE, screenBuffer->Data()); GL_CHECK_ERRORS;
}
IsCollision VectorDirection(glm::vec2 target)
{
    glm::vec2 compass[] = {
        glm::vec2(-1.0f, 0.0f),  // влево
        glm::vec2(1.0f, 0.0f),  // вправо
        glm::vec2(0.0f, 1.0f),  // вверх
        glm::vec2(0.0f, -1.0f)  // вниз
    };
    float max = 0.0f;
    unsigned int best_match = -1;
    for (unsigned int i = 0; i < 4; i++)
    {
        float dot_product = glm::dot(glm::normalize(target), compass[i]);
        if (dot_product > max)
        {
            max = dot_product;
            best_match = i;
        }
    }
    return (IsCollision)best_match;
}

typedef std::tuple<bool, IsCollision, glm::vec2> Collision;
template <typename T, typename Object>
Collision CheckCollision(T &one, Object & two) // столкновение вида AABB-Окружность
{
    // Сначала вычисляем точку центра окружности 
    glm::vec2 center(one->coords.x + tileSize / 2, one->coords.y + tileSize / 2);
	
    // Вычисляем информацию по AABB(координаты центра, и половинки длин сторон)
    glm::vec2 aabb_half_extents(tileSize / 2.0f, tileSize/ 2.0f);
    glm::vec2 aabb_center(two.pos_x + aabb_half_extents.x, two.pos_y + aabb_half_extents.y);
	
    // Получаем вектор разности между центром окружности и центром AABB
    glm::vec2 difference = center - aabb_center;
    glm::vec2 clamped = glm::clamp(difference, -aabb_half_extents, aabb_half_extents);
	
    // Добавляя переменную clamped к AABB_center мы получим ближайшую к окружности точку, лежащую на стороне AABB
    glm::vec2 closest = aabb_center + clamped;
	
    // Получаем вектор между центром окружности и ближайшей к ней точкой AABB, проверяем чтобы длина этого вектора была меньше радиуса окружности 
    difference = closest - center;

    if (glm::length(difference) < tileSize / 2.0f) // именно "<", а не "<=", т.к. в противном случае в конце стадии разрешения столкновения (когда объекты касаются друг друга) произойдет повторное столкновение
        return std::make_tuple(true, VectorDirection(difference), difference);
    else
        return std::make_tuple(false, UP, glm::vec2(0.0f, 0.0f));
}
void Game::DoCollisions(float dt) {
    for (GameObject &box : this->Levels[this->Level].Bricks)
    {
    	if (box.IsSolid) {
            Collision collision = CheckCollision(player, box);
            if (std::get<0>(collision)) // если столкновение произошло (collision == true)
            {
                // Обработка столкновения
                IsCollision dir = std::get<1>(collision);

                glm::vec2 diff_vector = std::get<2>(collision);
                if (dir == LEFT || dir == RIGHT) // горизонтальное столкновение
                {
                    // Перемещение
                    float penetration = tileSize / 2.0f - std::abs(diff_vector.x);
                    if (dir == LEFT) {
                        player->coords.x += penetration; // двигаем обратно вправо
                    } else {
                        player->coords.x -= penetration; // двигаем обратно влево
                    }
                } else // вертикальное столкновение
                {                    
                    // Перемещение
                    float penetration = tileSize / 2.0f - std::abs(diff_vector.y);
                    if (dir == UP) {
                        player->coords.y -= penetration; // двигаем обратно вверх
                    } else {
                        player->coords.y += penetration; // двигаем обратно вниз
                    }
                }
            }
        } else if (!box.IsFloor && !box.Destroyed) {
            Collision collision = CheckCollision(player, box);
            if (std::get<0>(collision)) // если столкновение произошло (collision == true)
            {
                box.health -= 10;
                if (box.health < 0) {
                    box.Destroyed = true;
                }
                // Обработка столкновения
                IsCollision dir = std::get<1>(collision);

                glm::vec2 diff_vector = std::get<2>(collision);
                if (dir == LEFT || dir == RIGHT) // горизонтальное столкновение
                {
                    // Перемещение
                    float penetration = tileSize / 2.0f - std::abs(diff_vector.x);
                    if (dir == LEFT) {
                        player->coords.x += penetration; // двигаем обратно вправо
                    } else {
                        player->coords.x -= penetration; // двигаем обратно влево
                    }
                } else // вертикальное столкновение
                {                    
                    // Перемещение
                    float penetration = tileSize / 2.0f - std::abs(diff_vector.y);
                    if (dir == UP) {
                        player->coords.y -= penetration; // двигаем обратно вверх
                    } else {
                        player->coords.y += penetration; // двигаем обратно вниз
                    }
                }
            }
        }
    }
}  
void Game::DoCollisionsWithMoney(float dt) {
    for (int i = 0; i < this->Levels[Level].AnimatedObject.size(); ++i)
    {
        if (dist(player->coords, this->Levels[Level].AnimatedObject[i]->coords) < tileSize * 2 / 3.0f) {
            this->Levels[Level].AnimatedObject.erase(this->Levels[Level].AnimatedObject.begin() + i);
            player_money++;
        }
    }
}  

void Game::DoCollisionsWithMonsters(float dt) {
    for (GameObject &box : this->Levels[this->Level].Bricks)
    {
        for (Monster *monster : this->Levels[this->Level].Monsters) {

            if (box.IsSolid) {
                Collision collision = CheckCollision(monster, box);
                if (std::get<0>(collision)) // если столкновение произошло (collision == true)
                {
                    // Обработка столкновения
                    IsCollision dir = std::get<1>(collision);

                    glm::vec2 diff_vector = std::get<2>(collision);
                    if (dir == LEFT || dir == RIGHT) // горизонтальное столкновение
                    {
                        // Перемещение
                        float penetration = tileSize / 2.0f - std::abs(diff_vector.x);
                        if (dir == LEFT) {
                            monster->coords.x += penetration; // двигаем обратно вправо
                        } else {
                            monster->coords.x -= penetration; // двигаем обратно влево
                        }
                    } else // вертикальное столкновение
                    {                    
                        // Перемещение
                        float penetration = tileSize / 2.0f - std::abs(diff_vector.y);
                        if (dir == UP) {
                            monster->coords.y -= penetration; // двигаем обратно вверх
                        } else {
                            monster->coords.y += penetration; // двигаем обратно вниз
                        }
                    }
                }
            } else if (!box.IsFloor && !box.Destroyed) {
                Collision collision = CheckCollision(monster, box);
                if (std::get<0>(collision)) // если столкновение произошло (collision == true)
                {
                    box.health -= 5;
                    if (box.health < 0) {
                        box.Destroyed = true;
                    }
                    // Обработка столкновения
                    IsCollision dir = std::get<1>(collision);

                    glm::vec2 diff_vector = std::get<2>(collision);
                    if (dir == LEFT || dir == RIGHT) // горизонтальное столкновение
                    {
                        // Перемещение
                        float penetration = tileSize / 2.0f - std::abs(diff_vector.x);
                        if (dir == LEFT) {
                            monster->coords.x += penetration; // двигаем обратно вправо
                        } else {
                            monster->coords.x -= penetration; // двигаем обратно влево
                        }
                    } else // вертикальное столкновение
                    {                    
                        // Перемещение
                        float penetration = tileSize / 2.0f - std::abs(diff_vector.y);
                        if (dir == UP) {
                            monster->coords.y -= penetration; // двигаем обратно вверх
                        } else {
                            monster->coords.y += penetration; // двигаем обратно вниз
                        }
                    }
                }
            }
        }
    }
}  
bool CheckComplete(Player &one, GameObject &two) 
{
    // Перекрытие по оси x?
    bool collisionX = one.coords.x + tileSize >= two.pos_x &&
        two.pos_x + tileSize >= one.coords.x;
 
    // Перекрытие по оси y?
    bool collisionY = one.coords.y + tileSize >= two.pos_y &&
        two.pos_y + tileSize >= one.coords.y;
 
    // Если перекрытия происходят относительно обеих осей, то мы имеем столкновение
    return collisionX && collisionY;
}

void Game::FadeOut(GLFWwindow* window, float brightness) {

    this->Levels[Level].Draw(*screenBuffer, brightness);

    glDrawPixels (Width, Height, GL_RGBA, GL_UNSIGNED_BYTE, screenBuffer->Data()); GL_CHECK_ERRORS; 

    glfwSwapBuffers(window);

}
void Game::FadeIn(GLFWwindow* window, float brightness) {
    this->Levels[Level].Draw(*screenBuffer, brightness);
    
    glDrawPixels (Width, Height, GL_RGBA, GL_UNSIGNED_BYTE, screenBuffer->Data()); GL_CHECK_ERRORS; 

    glfwSwapBuffers(window);

}
void Game::ChangeLevel(GLFWwindow* window) {
    float ChangeTime = 2.0f;

    float currentFrame = glfwGetTime();
    float lastFrame = currentFrame;
    float deltaTime = currentFrame - lastFrame;
    //FadeOut
    while (deltaTime < ChangeTime) {
        float brightness = deltaTime / ChangeTime;
        FadeOut(window, 1.0f - brightness);   
        currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
    }

    this->Level += 1;
    player->coords = this->Levels[this->Level].PlayerPosition;
    //FadeIn
    lastFrame = currentFrame;
    deltaTime = currentFrame - lastFrame;
    while (deltaTime < ChangeTime) {
        float brightness = deltaTime / ChangeTime;
        FadeIn(window, brightness);   
        currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
    }
}
void game_win(Image &screen) {
    Image you_died("resources/elems/you_win.png");

    int width = you_died.Width();
    int x_shift = screen.Width() / 2 - you_died.width / 2;
    int height = you_died.Height();
    int y_shift = screen.Height() / 2;

    for (int i = 0; i < width; ++i) {
        for (int j = 0; j < height; ++j) {
            screen.PutPixel(i + x_shift, j + y_shift, you_died.GetPixel(i, height - j - 1));
        }   
    }
}
void Game::CompleteLev(GLFWwindow* window) {
    for (GameObject &box : this->Levels[this->Level].Bricks) {
        if (box.CompleteLevel) {
            if (CheckComplete(*player, box)) {

                ChangeLevel(window);

                return;
            }
        }
    }
}
void game_over(Image &screen) {
    Image you_died("resources/elems/you_died.png");

    int width = you_died.Width();
    int x_shift = screen.Width() / 2 - you_died.width / 2;
    int height = you_died.Height();
    int y_shift = screen.Height() / 2;

    for (int i = 0; i < width; ++i) {
        for (int j = 0; j < height; ++j) {
            screen.PutPixel(i + x_shift, j + y_shift, you_died.GetPixel(i, height - j - 1));
        }   
    }
}
void Game::IsPlayerLive() {
    if (player->hitpoints <= 0) {
        player->state = PlayerState::DEAD;
    }
}
void Game::DeleteALL() {
    for (Monster *monster : this->Levels[this->Level].Monsters) {
        delete monster;
    }
    for (Magic *magic : Magical_attack) {
        delete magic;
    }

}
void Game::Update(float dt, GLFWwindow* window) {
    this->CompleteLev(window);

    if (Levels.size() <= this->Level) {
        float ChangeTime = 2.0f;

        float currentFrame = glfwGetTime();
        float lastFrame = currentFrame;
        float deltaTime = currentFrame - lastFrame;
        //FadeOut
        while (deltaTime < ChangeTime) {
            float brightness = deltaTime / ChangeTime;
            FadeOut(window, 1.0f - brightness);   
            currentFrame = glfwGetTime();
            deltaTime = currentFrame - lastFrame;
        }

        currentFrame = glfwGetTime();
        lastFrame = currentFrame;
        while (dt < 3) {
            currentFrame = glfwGetTime();
            dt = currentFrame - lastFrame;
            game_win(*screenBuffer);

            glDrawPixels (Width, Height, GL_RGBA, GL_UNSIGNED_BYTE, screenBuffer->Data()); GL_CHECK_ERRORS; 
            glfwSwapBuffers(window);
        }
        glfwTerminate();
    }
	this->DoCollisions(dt);
    this->ProcessPlayerAndMonster();
    this->MonstersAttack();
    this->DoCollisionsWithMonsters(dt);
    this->DoCollisionsWithMoney(dt);
    this->IsPlayerLive();
    if (player->state == PlayerState::DEAD) {
        float ChangeTime = 2.0f;

        float currentFrame = glfwGetTime();
        float lastFrame = currentFrame;
        float deltaTime = currentFrame - lastFrame;
        //FadeOut
        while (deltaTime < ChangeTime) {
            float brightness = deltaTime / ChangeTime;
            FadeOut(window, 1.0f - brightness);   
            currentFrame = glfwGetTime();
            deltaTime = currentFrame - lastFrame;
        }

        currentFrame = glfwGetTime();
        lastFrame = currentFrame;
        while (dt < 3) {
            currentFrame = glfwGetTime();
            dt = currentFrame - lastFrame;
            game_over(*screenBuffer);

            glDrawPixels (Width, Height, GL_RGBA, GL_UNSIGNED_BYTE, screenBuffer->Data()); GL_CHECK_ERRORS; 
            glfwSwapBuffers(window);
        }
        this->DeleteALL();
        this->Init();
    }
}

void Game::MonstersAttack() {
    int MonstersStrike = attack();
    player->hitpoints -= MonstersStrike;
}

int Game::attack() {
    int strike = 0;
    for (int i = 0; i < this->Levels[this->Level].Monsters.size(); ++i) {
        if (this->Levels[this->Level].Monsters[i]->IsAttack(player->coords)) {
            if (this->Levels[this->Level].Monsters[i]->type == 1) {                
                strike += this->Levels[this->Level].Monsters[i]->attack_points;
            } else if (this->Levels[this->Level].Monsters[i]->type == 2 && 
                        this->Levels[this->Level].Monsters[i]->MagicFrame++ > this->Levels[this->Level].Monsters[i]->MagicMaxFrame) {
                
                this->Levels[this->Level].Monsters[i]->MagicFrame = 0;

                int magic_ball_x = this->Levels[this->Level].Monsters[i]->coords.x + tileSize / 2;  
                int magic_ball_y = this->Levels[this->Level].Monsters[i]->coords.y + tileSize / 2;

                int xspeed = player->coords.x - magic_ball_x;
                int yspeed = player->coords.y - magic_ball_y;
            
                int speed = std::sqrt(xspeed * xspeed + yspeed * yspeed); 

                std::string path = "resources/player/magical_ball/";
                Magic *magical_ball = new Magic(path, magic_ball_x, magic_ball_y, (xspeed * 7 / speed), (yspeed * 7/ speed));
                magical_ball->DropPlayer = false;                
                magical_ball->DropMonster = true;
                this->Magical_attack.push_back(magical_ball);
            }
        }
    }
    return strike;
}
void Game::ProcessPlayerAndMonster() {
    double monster_attack_distance = tileSize / 2;
    double monster_awake_distance = 7 * tileSize;
    for (int i = 0; i < this->Levels[this->Level].Monsters.size(); i++) {
        Monster *monster = this->Levels[this->Level].Monsters[i];
        double player_dist = dist(player->coords, monster->coords);
        switch (monster->state) {
            case MonsterState::SLEEP:
                if (player_dist < monster_awake_distance) {
                    monster->state = MonsterState::WALK;
                }
                break;

            case MonsterState::WALK:
                if (player_dist > monster_awake_distance) {
                    monster->state = MonsterState::SLEEP;
                    break;
                }
                if (monster->type == 1 && player_dist <= monster_attack_distance) {
                    monster->state = MonsterState::ATTACK;
                    monster->begin_attack = 1;
                    break;
                } else if (monster->type == 2 && player_dist <= 8 * tileSize) {
                    monster->state = MonsterState::ATTACK;
                    monster->begin_attack = 1;         
                    break;
                }
                if (monster->type == 1) {
                    monster->MoveTo(player->coords);
                }
            case MonsterState::ATTACK:
                if (!monster->begin_attack) {
                    monster->state = MonsterState::WALK;
                    break;
                }                    
                if (monster->type == 2) {
                    monster->MoveOut(player->coords);
                }
                break;
            case MonsterState::DEAD:
                delete monster;
                this->Levels[this->Level].Monsters.erase(this->Levels[this->Level].Monsters.begin() + i);
                break;
            default:
                break;
        }
    }
}

void Game::GetPlayerImg() {
    //Вниз
    Pixel *img_S = new Pixel[tileSize * tileSize];
    GetImgData(ImagePlayer, img_S, 6, 0);
    PlayerImg.push_back(img_S);

    img_S = new Pixel[tileSize * tileSize];
    GetImgData(ImagePlayer, img_S, 7, 0);
    PlayerImg.push_back(img_S);

    img_S = new Pixel[tileSize * tileSize];
    GetImgData(ImagePlayer, img_S, 8, 0);
    PlayerImg.push_back(img_S);

    img_S = new Pixel[tileSize * tileSize];
    GetImgData(ImagePlayer, img_S, 7, 0);
    PlayerImg.push_back(img_S);

    //Вверх
    Pixel *img_W = new Pixel[tileSize * tileSize];
    GetImgData(ImagePlayer, img_W, 6, 3);
    PlayerImg.push_back(img_W);

    img_W = new Pixel[tileSize * tileSize];
    GetImgData(ImagePlayer, img_W, 7, 3);
    PlayerImg.push_back(img_W);

    img_W = new Pixel[tileSize * tileSize];
    GetImgData(ImagePlayer, img_W, 8, 3);
    PlayerImg.push_back(img_W);

    img_W = new Pixel[tileSize * tileSize];
    GetImgData(ImagePlayer, img_W, 7, 3);
    PlayerImg.push_back(img_W);
    
    //Вправо
    Pixel *img_R = new Pixel[tileSize * tileSize];
    GetImgData(ImagePlayer, img_R, 6, 2);
    PlayerImg.push_back(img_R);

    img_R = new Pixel[tileSize * tileSize];
    GetImgData(ImagePlayer, img_R, 7, 2);
    PlayerImg.push_back(img_R);

    img_R = new Pixel[tileSize * tileSize];
    GetImgData(ImagePlayer, img_R, 8, 2);
    PlayerImg.push_back(img_R);

    img_R = new Pixel[tileSize * tileSize];
    GetImgData(ImagePlayer, img_R, 7, 2);
    PlayerImg.push_back(img_R);
    
    //Влево
    Pixel *img_L = new Pixel[tileSize * tileSize];
    GetImgData(ImagePlayer, img_L, 6, 1);
    PlayerImg.push_back(img_L);

    img_L = new Pixel[tileSize * tileSize];
    GetImgData(ImagePlayer, img_L, 7, 1);
    PlayerImg.push_back(img_L);

    img_L = new Pixel[tileSize * tileSize];
    GetImgData(ImagePlayer, img_L, 8, 1);
    PlayerImg.push_back(img_L);

    img_L = new Pixel[tileSize * tileSize];
    GetImgData(ImagePlayer, img_L, 7, 1);
    PlayerImg.push_back(img_L);
    
    //диагональ вверх-влево    
    Pixel *img_WA = new Pixel[tileSize * tileSize];
    GetImgData(ImagePlayerDiag, img_WA, 0, 3);
    PlayerImg.push_back(img_WA);

    img_WA = new Pixel[tileSize * tileSize];
    GetImgData(ImagePlayerDiag, img_WA, 1, 3);
    PlayerImg.push_back(img_WA);

    img_WA = new Pixel[tileSize * tileSize];
    GetImgData(ImagePlayerDiag, img_WA, 2, 3);
    PlayerImg.push_back(img_WA);

    img_WA = new Pixel[tileSize * tileSize];
    GetImgData(ImagePlayerDiag, img_WA, 1, 3);
    PlayerImg.push_back(img_WA);
    
    //диагональ вверх-вправо    
    Pixel *img_WD = new Pixel[tileSize * tileSize];
    GetImgData(ImagePlayerDiag, img_WD, 0, 2);
    PlayerImg.push_back(img_WD);

    img_WD = new Pixel[tileSize * tileSize];
    GetImgData(ImagePlayerDiag, img_WD, 1, 2);
    PlayerImg.push_back(img_WD);

    img_WD = new Pixel[tileSize * tileSize];
    GetImgData(ImagePlayerDiag, img_WD, 2, 2);
    PlayerImg.push_back(img_WD);

    img_WD = new Pixel[tileSize * tileSize];
    GetImgData(ImagePlayerDiag, img_WD, 1, 2);
    PlayerImg.push_back(img_WD);
    
    //диагональ вниз-вправо    
    Pixel *img_SD = new Pixel[tileSize * tileSize];
    GetImgData(ImagePlayerDiag, img_SD, 0, 1);
    PlayerImg.push_back(img_SD);

    img_SD = new Pixel[tileSize * tileSize];
    GetImgData(ImagePlayerDiag, img_SD, 1, 1);
    PlayerImg.push_back(img_SD);

    img_SD = new Pixel[tileSize * tileSize];
    GetImgData(ImagePlayerDiag, img_SD, 2, 1);
    PlayerImg.push_back(img_SD);

    img_SD = new Pixel[tileSize * tileSize];
    GetImgData(ImagePlayerDiag, img_SD, 1, 1);
    PlayerImg.push_back(img_SD);
    
    //диагональ вниз-влево    
    Pixel *img_SA = new Pixel[tileSize * tileSize];
    GetImgData(ImagePlayerDiag, img_SA, 0, 0);
    PlayerImg.push_back(img_SA);

    img_SA = new Pixel[tileSize * tileSize];
    GetImgData(ImagePlayerDiag, img_SA, 1, 0);
    PlayerImg.push_back(img_SA);

    img_SA = new Pixel[tileSize * tileSize];
    GetImgData(ImagePlayerDiag, img_SA, 2, 0);
    PlayerImg.push_back(img_SA);

    img_SA = new Pixel[tileSize * tileSize];
    GetImgData(ImagePlayerDiag, img_SA, 1, 0);
    PlayerImg.push_back(img_SA);
}