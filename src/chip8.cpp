#include "chip8.hpp"
#include <SDL2/SDL.h>
#include <chrono>
#include <iostream>
#include <thread>

namespace chip8 {

int Chip8::init() {
  std::cout << "Starting Chip8 emulator\n";

  display.Init();

  // Set default values
  for (int i = 0; i < 4096; i++) {
    mem[i] = 0;
  }

  for (int i = 0; i < 16; i++) {
    V[i] = 0;
    stack[i] = 0;
    keys[i] = 0;
  }

  for (int i = 0; i < 2048; i++) {
    pixels[i] = 0;
  }

  for (int i = 0; i < 80; i++) {
    mem[i] = fontset[i];
  }

  pc = 0x200;
  sp = 0;
  I = 0;

  sound_timer = 0;
  delay_timer = 0;

  draw_flag = false;
  sound_flag = false;

  use_cosmac_instructions = false;

  return 0;
}

int Chip8::run() {
  // Emulator loop
  while (true) {
    // Fetch, Decode & Execute
    emulate_one_cycle();

    // Process inputs
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
      switch (event.type) {
      case (SDL_QUIT): {
        exit(0);
      }

      case (SDL_KEYDOWN): {
        std::cout << event.key.keysym.sym;

        for (int i = 0; i < 16; i++) {
          if (event.key.keysym.sym == SDLK_ESCAPE) {
            exit(0);
          }

          if (event.key.keysym.sym == keymap[i]) {
            keys[i] = 1;
          }
        }
      }

      case (SDL_KEYUP): {
        for (int i = 0; i < 16; i++) {
          if (event.key.keysym.sym == keymap[i]) {
            keys[i] = 0;
          }
        }
      }
      }
    }

    // Draw
    if (draw_flag) {
      display.Draw(pixels);
      draw_flag = false;
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(1));
  }

  display.Quit();

  return 0;
}

int get_bit(int n, int bit) { return (n >> bit) & 1; }

void Chip8::emulate_one_cycle() {
  int opcode = (mem[pc] << 8) | mem[pc + 1];
  pc += 2;

  for (int i = 0; i < 16; i++) {
    if (keys[i] == 1) {
      std::cout << keys[i] << "\n";
    }
  }

  uint8_t x = (opcode & 0x0F00) >> 8;
  uint8_t y = (opcode & 0x00F0) >> 4;
  uint8_t n = opcode & 0x000F;
  uint8_t nn = opcode & 0x00FF;
  uint16_t nnn = opcode & 0x0FFF;

  switch (opcode & 0xF000) {

  case (0x0000): {
    switch (opcode & 0x00FF) {
    case (0x00E0): {
      // Clear screen
      for (int i = 0; i < Display::height * Display::width; i++) {
        pixels[i] = 0;
      }
      draw_flag = true;
      break;
    }

    case (0x00EE): {
      // Return from subroutine
      pc = stack[sp];
      sp--;

      if (sp < 0) {
        std::cout << "Stack pointer is negative - quitting\n";
        exit(0);
      }
      break;
    }

    default: {
      std::cout << "Unknown instruction in 0x0xxx: " << std::hex << opcode
                << "\n";
    }
    }
    break;
  }

  case (0x1000): {
    // Jump
    pc = nnn;
    break;
  }

  case (0x2000): {
    // Subbroutine
    sp++;
    stack[sp] = pc;
    pc = nnn;
    break;
  }

  case (0x3000): {
    // Conditional skip, VX == nn
    if (V[x] == nn) {
      pc += 2;
    }
    break;
  }

  case (0x4000): {
    // Conditional skip, VX != nn
    if (V[x] != nn) {
      pc += 2;
    }
    break;
  }

  case (0x5000): {
    // Conditional skip, VX == VY
    if (V[x] == V[y]) {
      pc += 2;
    }
    break;
  }

  case (0x6000): {
    // Set register VX to nn
    V[x] = nn;
    break;
  }

  case (0x7000): {
    // Add value nn to VX
    V[x] += nn;
    break;
  }

  case (0x8000): {
    // Logical and arithmetic ops
    switch (opcode & 0x000F) {
    case (0x0): {
      // Set VX to VY
      V[x] = V[y];
      break;
    }

    case (0x1): {
      // Binary OR
      V[x] |= V[y];
      break;
    }

    case (0x2): {
      // Binary AND
      V[x] &= V[y];
      break;
    }

    case (0x3): {
      // Binary XOR
      V[x] ^= V[y];
      break;
    }

    case (0x4): {
      // Add
      if (V[x] + V[y] > 255) {
        V[0xF] = 1;
      } else {
        V[0xF] = 0;
      }
      V[x] += V[y];
      break;
    }

    case (0x5): {
      // Subtract
      if (V[x] > V[y]) {
        V[0xF] = 1;
      } else {
        V[0xF] = 0;
      }
      V[x] -= V[y];
      break;
    }

    case (0x6): {
      // Right shift
      if (use_cosmac_instructions) {
        V[x] = V[y];
      }
      V[0xF] = V[x] & 1; // Set VF to shifted bit (LSB)
      V[x] >>= 1;
      break;
    }

    case (0x7): {
      // Subtract
      if (V[y] > V[x]) {
        V[0xF] = 1;
      } else {
        V[0xF] = 0;
      }
      V[x] = V[y] - V[x];
      break;
    }

    case (0xE): {
      // Left shift
      if (use_cosmac_instructions) {
        V[x] = V[y];
      }
      V[0xF] = V[x] >> 16; // Set VF to shifted bit (MSB)
      V[x] <<= 1;
      break;
    }

    default: {
      std::cout << "Unknown instruction in 0x8xxx: " << std ::hex << opcode
                << "\n";
      break;
    }
    }
    break;
  }

  case (0x9000): {
    // Conditional skip, VX != VY
    if (V[x] != V[y]) {
      pc += 2;
    }
    break;
  }

  case (0xA000): {
    I = nnn;
    break;
  }

  case (0xB000): {
    // Jump with offset
    if (use_cosmac_instructions) {
      // Legacy
      pc = nnn + V[0];
      pc = nnn + V[x];
    }
    break;
  }

  case (0xC000): {
    // Random
    V[x] = (rand() % (0xFF + 1)) & nn;
    break;
  }

  case (0xD000): {
    // Draw
    // Starting positions should wrap
    uint16_t x_start = V[x] % Display::width;
    uint16_t y_start = V[y] % Display::height;

    V[0xF] = 0;

    for (int i = 0; i < n; i++) {
      uint8_t pixel = mem[I + i];

      for (int j = 0; j < 8; j++) {
        if ((pixel & (0x80 >> j)) != 0) {
          int pixel_index = (x_start + j + ((y_start + i) * Display::width));
          if (pixels[pixel_index] == 1) {
            V[0xF] = 1;
          }

          pixels[pixel_index] ^= 1;
        }
      }
    }
    draw_flag = true;
    break;
  }

  case (0xE000): {
    // Skip on button press
    if (V[x] >= 16) {
      // Not a valid key
      std::cout << "Invalid key index " << V[x] << "\n";
      exit(0);
    }

    switch (opcode & 0x00FF) {
    case (0x9E): {
      // Skip if key press
      if (keys[V[x]] == 1) {
        pc += 2;
      }
      break;
    }

    case (0xA1): {
      // Skip if key not pressed
      if (keys[V[x]] == 0) {
        pc += 2;
      }
      break;
    }

    default: {
      std::cout << "Unknown instruction: " << std::hex << opcode << "\n";
    }
    }
    break;
  }

  case (0xF000): {
    // Timers
    switch (opcode & 0x00FF) {
    case (0x07): {
      // Set VX to delay timer
      V[x] = delay_timer;
      break;
    }

    case (0x0A): {
      // Wait for key press
      bool key_pressed = false;
      for (int i = 0; i < 16; i++) {
        if (keys[i] == 1) {
          V[x] = i;
          key_pressed = true;
        }
      }

      if (!key_pressed) {
        pc -= 2;
      }
      break;
    }

    case (0x15): {
      // Set delay timer to VX
      delay_timer = V[x];
      break;
    }

    case (0x18): {
      // Set sound timer to VX
      sound_timer = V[x];
      break;
    }

    case (0x1E): {
      // Add to I
      if (I + V[x] >= 1000) {
        // Overflow
        V[0xF] = 1;
      }

      I += V[x];
      break;
    }

    case (0x29): {
      // Font
      I = V[x] * 0x5;
      break;
    }

    case (0x33): {
      // Decimal conversion
      mem[I] = V[x] / 100;
      mem[I + 1] = (V[x] / 10) % 10;
      mem[I + 2] = V[x] % 10;
      break;
    }

    case (0x55): {
      // Store mem
      for (int i = 0; i <= x; i++) {
        mem[I + i] = V[i];
      }

      if (use_cosmac_instructions) {
        I += x + 1;
      }
      break;
    }

    case (0x65): {
      // Load mem
      for (int i = 0; i <= x; i++) {
        V[i] = mem[I + i];
      }

      if (use_cosmac_instructions) {
        I += x + 1;
      }
      break;
    }

    default: {
      std::cout << "Unknown instruction in 0xFxxx: " << std::hex << opcode
                << "\n";
    }
    }
    break;
  }

  default: {
    std::cout << "Unknown instruction: " << std::hex << opcode << "\n";
  }
  }
}

bool Chip8::load_rom(std::string path) {
  FILE *gameP = fopen(path.c_str(), "rb");
  if (gameP == NULL) {
    return false;
  }

  // Finds the size of the game
  fseek(gameP, 0, SEEK_END);
  long int gameSize = ftell(gameP);
  fseek(gameP, 0, SEEK_SET);

  // Reads the data from the game into the buffer
  char *buffer = (char *)malloc(sizeof(char) * gameSize);
  if (buffer == NULL) {
    return false;
  }
  fread(buffer, 1, gameSize, gameP);

  // Writes the data on the memory
  for (int i = 0; i < gameSize; i++) {
    mem[i + 512] = buffer[i];
  }

  fclose(gameP);
  free(buffer);
  return true;
}

} // namespace chip8
