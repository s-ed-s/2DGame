#include "game_level.h"

#include <fstream>
#include <sstream>
#include <string>
#include <map>
#include <vector>

ObjectWithAnimation::ObjectWithAnimation(std::string &path, int x, int y, int count) : object_animation(path, count, 7) {
    coords.x = x * tileSize;
    coords.y = y * tileSize;
}

void ObjectWithAnimation::Draw(Image &screen) {
    Image *img = object_animation.Draw();
    for (int x = 0; x < img->width; ++x) {
        for (int y = 0; y < img->height; ++y) {   
            Pixel tmp =   img->GetPixel(x, y);
            if (tmp.r != 0 || tmp.g != 0 || tmp.b != 0) {
                screen.PutPixel(coords.x + x, coords.y + y, blend(screen.GetPixel(coords.x, coords.y), img->GetPixel(x, y)));
            }
        }
    }
}

void GameLevel::Load(const char* file)
{
    // Очистка старых данных
    this->Bricks.clear();
	
    // Загрузка из файла
    unsigned int tileCode;

    std::string line;
    std::ifstream fstream(file);
    std::vector<std::vector<unsigned int>> tileData;
    if (fstream)
    {
        while (std::getline(fstream, line)) // построчное чтение файла уровня
        {
            std::vector<unsigned int> row;
            for (unsigned int i = 0; i < line.size(); ++i) {                
                row.push_back(line[i]);
            }
            tileData.push_back(row);
        }
        if (tileData.size() > 0) {
            this->row_blocks = tileData[0].size();
            this->col_blocks = tileData.size();
            this->init(tileData);
        }
    }
}

void GameLevel::Draw(Image &screenBuffer, float brightness)
{
    for (unsigned int i = 0; i < this->Bricks.size(); ++i) {
        if (this->Bricks[i].Destroyed) {
            for (int j = 0; j < pol->size; ++j) {
                this->Bricks[i].data[j] = pol->data[j];
                this->Bricks[i].Destroyed = false;
                this->Bricks[i].IsFloor = true;
            }
        }
        this->Bricks[i].Draw(screenBuffer, brightness);
    }
}
/*
bool GameLevel::IsCompleted()
{
    for (GameObject& tile : this->Bricks)
        if (!tile.IsSolid && !tile.Destroyed)
            return false;
    return true;
}
*/
void GameLevel::init(std::vector<std::vector<unsigned int>> tileData)
{
    // Вычисляем размеры
    unsigned int height = tileData.size();
    unsigned int width = tileData[0].size();

    // Инициализация уровня данными из tileData	

    Point pos_nul{.x = 0,  .y = 0};
    Point pos_solid_blocks{.x = 49, .y = 23};
    Point pos_floor{.x = 1, .y = 23};
    Point pos_flame{.x = 111, .y = 23};
    Point pos_next_lev{.x = 22, .y = 25};
    Point pos_destr_block{.x = 46, .y = 23};

    //стена
    GameObject solid(background, pos_nul, pos_solid_blocks);
    solid.IsSolid = true;
    Blocks.push_back(solid);
    
    //пол
    GameObject pol(background, pos_nul, pos_floor);
    Blocks.push_back(pol);

    this->pol = new GameObject(pol);

    //переход на следующий уровень
    GameObject next_lev(background, pos_nul, pos_next_lev);
    Blocks.push_back(next_lev);

    //разрушающийся блок
    GameObject destr_block(background, pos_nul, pos_destr_block);
    Blocks.push_back(destr_block);

    //Блок с факелом
    GameObject  flame(background, pos_nul, pos_flame);
    Blocks.push_back(flame);

    for (unsigned int pos_y = 0; pos_y < height; ++pos_y)
    {
        for (unsigned int pos_x = 0; pos_x < width; ++pos_x)
        {
            // Проверяем тип блока по информации об уровне
            if (tileData[pos_y][pos_x] == '#') {// твердый

                GameObject obj = Blocks[0];
                obj.IsSolid = true;
                obj.CompleteLevel = false;
                obj.IsFloor = false;
                obj.SetPos(Point{.x = (int)pos_x, .y = (int)pos_y});
                this->Bricks.push_back(obj);

            } else if (tileData[pos_y][pos_x] == '.') {// пол

                GameObject obj = Blocks[1];
                obj.IsSolid = false;
                obj.CompleteLevel = false;
                obj.IsFloor = true;
                obj.SetPos(Point{.x = (int)pos_x, .y = (int)pos_y});
                this->Bricks.push_back(obj);

            } else if (tileData[pos_y][pos_x] == 'x') {// следующий уровень

                GameObject obj = Blocks[1];
                obj.IsSolid = false;
                obj.CompleteLevel = true;
                obj.IsFloor = false;
                obj.SetPos(Point{.x = (int)pos_x, .y = (int)pos_y});
                this->Bricks.push_back(obj);

                std::string path = "resources/elems/portal/";
                ObjectWithAnimation *portal = new ObjectWithAnimation(path, pos_x, pos_y, 6);
                AnimatedObject.push_back(portal);
            } else if (tileData[pos_y][pos_x] == '%') {// разрушающийся блок

                GameObject obj = Blocks[3];
                obj.IsSolid = false;
                obj.CompleteLevel = false;
                obj.IsFloor = false;
                obj.SetPos(Point{.x = (int)pos_x, .y = (int)pos_y});
                this->Bricks.push_back(obj);

            } else if (tileData[pos_y][pos_x] == '@') {// позиция игрока

                PlayerPosition = Point{.x = int(pos_x) * tileSize, .y = int(pos_y) * tileSize};
                GameObject obj = Blocks[1];
                obj.IsSolid = false;
                obj.CompleteLevel = false;
                obj.IsFloor = true;
                obj.SetPos(Point{.x = (int)pos_x, .y = (int)pos_y});
                this->Bricks.push_back(obj);

            } else if (tileData[pos_y][pos_x] == 'f')  {// блок с факелом

                GameObject obj = Blocks[4];
                obj.IsSolid = true;
                obj.CompleteLevel = false;
                obj.IsFloor = false;
                obj.SetPos(Point{.x = (int)pos_x, .y = (int)pos_y});
                this->Bricks.push_back(obj);

            } else if (tileData[pos_y][pos_x] == '1') { //монстр

                GameObject obj = Blocks[1];
                obj.IsSolid = false;
                obj.CompleteLevel = false;
                obj.IsFloor = true;
                obj.SetPos(Point{.x = (int)pos_x, .y = (int)pos_y});
                this->Bricks.push_back(obj);

                Monster *monster = new Monster(pos_x, pos_y, 1);
                Monsters.push_back(monster);
            } else if (tileData[pos_y][pos_x] == 'm') { //монетка

                GameObject obj = Blocks[1];
                obj.IsSolid = false;
                obj.CompleteLevel = false;
                obj.IsFloor = true;
                obj.SetPos(Point{.x = (int)pos_x, .y = (int)pos_y});
                this->Bricks.push_back(obj);

                std::string path = "resources/elems/coins/";
                ObjectWithAnimation *money = new ObjectWithAnimation(path, pos_x, pos_y, 8);
                AnimatedObject.push_back(money);
            } else if (tileData[pos_y][pos_x] == 'M') { //Неберущаяся монетка

                std::string path = "resources/elems/coins/";
                ObjectWithAnimation *money = new ObjectWithAnimation(path, pos_x, pos_y, 8);
                AnimatedObject.push_back(money);
            } else if (tileData[pos_y][pos_x] == '2') { //монстр

                GameObject obj = Blocks[1];
                obj.IsSolid = false;
                obj.CompleteLevel = false;
                obj.IsFloor = true;
                obj.SetPos(Point{.x = (int)pos_x, .y = (int)pos_y});
                this->Bricks.push_back(obj);

                Monster *monster = new Monster(pos_x, pos_y, 2);
                Monsters.push_back(monster);
            }
        }
    }
}
