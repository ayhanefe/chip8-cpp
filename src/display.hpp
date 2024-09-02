#include <SDL2/SDL.h>

namespace chip8 {

class Display {

public:
  static const unsigned int width = 64;
  static const unsigned int height = 32;

  static const unsigned int scale = 20;
  Display();
  void Init();
  void Clear();
  void Draw(uint8_t *pixels);
  void Quit();
  ~Display(); // Destructor

private:
  SDL_Window *window;
  SDL_Renderer *renderer;
  SDL_Texture *sdlTexture;
};

} // namespace chip8
