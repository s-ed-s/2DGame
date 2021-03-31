
#include "Monster.h"
#include <fstream>

Animation::Animation(std::string path, int img_number, int times_max) {
    std::string name;

    img_cur = 0;
    img_frame = 0;
    img_frame_max = times_max;
    to_end_vec_img = true;

    for (int i = 0; i < img_number; i++) {
        name = path + std::to_string(i + 1) + ".png";
        Image *new_img = new Image(name);
        img.push_back(new_img);
    }
}

Image *Animation::Draw(int curr_frame) {
    int draw = curr_frame;
    if (draw == -1) {
        draw = img_cur;
        img_frame++;
        if (img_frame > img_frame_max) {
            if (to_end_vec_img) {
                img_cur++;
                if (img_cur == img.size() - 1) {
                    to_end_vec_img = false;
                }
            } else {
                img_cur--;
                if (img_cur == 0) {
                    to_end_vec_img = true;
                }
            }
            img_frame = 0;
        }
    }
    return img[draw];
}

Image * Animation::Current() {
    return img[img_cur];
}

int Animation::len() {
    return img.size();
}

Monster::Monster(int x, int y, int type): 
    walk_animation("resources/monsters/" + std::to_string(type) + "/walk/", 3, 5),
    attack_animation("resources/monsters/" + std::to_string(type) + "/attack/", 6, 9),
    dying_animation("resources/monsters/" + std::to_string(type) + "/dying/", 3, 12) {
   

    attack_points = 6; 
    this->type = type;
    hitpoints = 50;

    coords.x = x * tileSize;
    coords.y = y * tileSize;

    start.x = x * tileSize;
    start.y = y * tileSize;

    state = MonsterState::SLEEP;
    look = MovementDir::RIGHT;

    begin_attack = 0;
    attack_frame = 0;

    dying = false;
    dying_ind = 0;
    dying_times = 0;
}


void Monster::Draw(Image &screen) {
    Image *img;
    switch (state) {
        case MonsterState::DYING:
            img = dying_animation.Draw(dying_ind);
            dying_times++;
            if (dying_times > dying_animation.img_frame_max) {
                dying_ind++;
                dying_times = 0;
            }
            if (dying_ind == dying_animation.len()) {
                state = MonsterState::DEAD;
            }
            break;
        case MonsterState::DEAD:
            return;
         //   img = dying_animation.Draw(dying_animation.len() - 1);
        case MonsterState::SLEEP:
            img = walk_animation.Draw(1);
            break;
        case MonsterState::WALK:
            img = walk_animation.Draw();
            break;
        case MonsterState::ATTACK:
            img = attack_animation.Draw(attack_frame);
            attack_frame += begin_attack;
            if (attack_frame == attack_animation.len() - 1) {
                begin_attack = -1;
            }
            if (attack_frame == 0) {
                begin_attack = 0;
            }
            break;
    }

    for (int x = 0; x < tileSize; ++x) {
        for (int y = 0; y < tileSize; ++y) {
            screen.PutPixel(coords.x + x, coords.y + y,
                    blend(screen.GetPixel(coords.x + x, coords.y + y),
                        img->GetPixel((look == MovementDir::LEFT) ? x : tileSize - x - 1, tileSize - y - 1)));
        }
    }

    for (int i = 0; i * 6 < hitpoints; ++i) {
        for (int j = 0; j < 5; j++) {
            screen.PutPixel(coords.x + tileSize / 2 + i, coords.y + tileSize + j, red);
            screen.PutPixel(coords.x + tileSize / 2 - i, coords.y + tileSize + j, red);
        }
    }

}
void Monster::MoveTo(Point &player) {
    int movedist = 1;
    Point heart;
    
    heart.x = player.x + tileSize / 2;
    heart.y = player.y + tileSize / 2;

    Point tale;

    tale.x = (look == MovementDir::LEFT) ? coords.x + tileSize : coords.x;
    tale.y = coords.y + tileSize / 2;

    if ((heart.x - tale.x) > movedist) {
        coords.x += movedist;
        look = MovementDir::RIGHT;
    } else if (movedist < tale.x - heart.x) {
        coords.x -= movedist;
        look = MovementDir::LEFT;
    }

    if (heart.y - tale.y > movedist) {
        coords.y += movedist;
    } else if (movedist < tale.y - heart.y) {
        coords.y -= movedist;
    }
    
}
void Monster::MoveOut(Point &player) {
    int movedist = 1;
    Point heart;
    
    heart.x = player.x + tileSize / 2;
    heart.y = player.y + tileSize / 2;

    Point tale;

    tale.x = (look == MovementDir::LEFT) ? coords.x + tileSize : coords.x;
    tale.y = coords.y + tileSize / 2;

    if ((heart.x - tale.x) > movedist) {
        coords.x -= movedist;
        look = MovementDir::RIGHT;
    } else if (movedist < tale.x - heart.x) {
        coords.x += movedist;
        look = MovementDir::LEFT;
    }

    if (heart.y - tale.y > movedist) {
        coords.y -= movedist;
    } else if (movedist < tale.y - heart.y) {
        coords.y += movedist;
    }
    
}

bool Monster::IsAttack(Point &coords) {
    Point player_center;
    player_center.x = coords.x + tileSize / 2;
    player_center.y = coords.y + tileSize / 2;

    double hit_distance = tileSize;
    if (attack_frame == attack_animation.img.size() - 1) {
        Point tale;
        tale.x = (look == MovementDir::LEFT) ? this->coords.x : this->coords.x + tileSize;
        tale.y = this->coords.y + tileSize / 2;
        if ((type == 1 && dist(player_center, tale) < hit_distance) || ((type == 2) && dist(player_center, tale) < 7 * tileSize)) {
            return true;
        }
        return false;
    } else {
        return false;
    }
}