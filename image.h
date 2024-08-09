class Image {
  unsigned char *data_ = nullptr;
  int width_ = 0, height_ = 0, channels_ = 0;
public:
  explicit Image(const char* filename);
  ~Image();
  Image(const Image& obj) = delete;
  Image(Image&& obj) = delete;
  Image& operator= (const Image& obj) = delete;
  Image& operator= (Image&& obj) = delete;

  const unsigned char *data() const { return data_; }
  int width() { return width_; }
  int height() { return height_; }
  int channels() { return channels_; }
};
