#define STB_IMAGE_IMPLEMENTATION
#include "deps/stb_image.h"
#include "image.h"

Image::Image(const char* filename) {
  data_ = stbi_load(filename, &width_, &height_, &channels_, 0);
}

Image::~Image() {
  stbi_image_free(data_);
}
