#include "display.hpp"
#include <algorithm>

namespace chip8 {

Display::Display() {}

void Display::Init() {

  SDL_Init(SDL_INIT_VIDEO);

  window = SDL_CreateWindow(
      "CHIP-8", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width * scale,
      height * scale, SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL);
  if (window == NULL) {
    printf("SDL initialization failed - window");
  }

  // Create renderer
  renderer = SDL_CreateRenderer(window, -1, 0);
  if (renderer == NULL) {
    printf("SDL initialization failed - renderer");
  }

  SDL_RenderSetLogicalSize(renderer, width * scale, height * scale);

  // Create texture that stores frame buffer
  sdlTexture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888,
                                 SDL_TEXTUREACCESS_STREAMING, width, height);
  if (renderer == NULL) {
    printf("SDL initialization failed - texture");
  }

  for (size_t i = 0; i < 100; i++) {
    SDL_PumpEvents();
  }
}

void Display::Clear() {
  uint8_t pixels[width * height] = {0};
  std::fill_n(pixels, height * width, 1);

  Display::Draw(pixels);
}

void Display::Draw(uint8_t *pixels) {
  uint32_t converted[height * width];

  for (int i = 0; i < height * width; i++) {
    converted[i] = (0x00FFFFFF * pixels[i]) | 0xFF000000;
  }

  // Update SDL texture
  SDL_UpdateTexture(sdlTexture, NULL, converted, width * sizeof(uint32_t));

  // Clear screen and render
  SDL_RenderClear(renderer);
  SDL_RenderCopy(renderer, sdlTexture, NULL, NULL);
  SDL_RenderPresent(renderer);
}

void Display::Quit() {

  SDL_DestroyWindow(window);
  SDL_DestroyTexture(sdlTexture);
  SDL_DestroyRenderer(renderer);

  SDL_Quit();
}

Display::~Display() { Display::Quit(); }

} // namespace chip8
