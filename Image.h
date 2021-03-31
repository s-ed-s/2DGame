#ifndef MAIN_IMAGE_H
#define MAIN_IMAGE_H

#include <string>

constexpr int tileSize = 32;

struct Pixel
{
    uint8_t r;
    uint8_t g;
    uint8_t b;
    uint8_t a;

    friend bool operator != (const Pixel& left, const Pixel& right) {
        if (left.r != right.r || left.g != right.g || left.b != right.b) {
            return true;
        }
        return false;
    }
};


constexpr Pixel backgroundColor{0, 0, 0, 0};
constexpr Pixel red {.r = 255, .g = 0, .b = 0, .a = 0};
constexpr Pixel green {.r = 0, .g = 255, .b = 0, .a = 0};

struct Image
{
  explicit Image(const std::string &a_path);
  Image();
  Image(int a_width, int a_height, int a_channels);
  Image(const Image &other): width(other.width), height(other.height), size(other.size) {
    data = new Pixel[size];
    for (int i = 0; i < size; ++i) { data[i] = other.data[i]; }
  }

  int Save(const std::string &a_path);

  int Width()    const { return width; }
  int Height()   const { return height; }
  int Channels() const { return channels; }
  size_t Size()  const { return size; }
  Pixel* Data()        { return  data; }

  Pixel GetPixel(int x, int y) { return data[width * y + x];}
  void  PutPixel(int x, int y, const Pixel &pix) { data[width* y + x] = pix; }

  ~Image();

  int width = -1;
  int height = -1;
  int channels = 3;
  size_t size = 0;
  Pixel *data = nullptr;
  bool self_allocated = false;
};



#endif //MAIN_IMAGE_H
