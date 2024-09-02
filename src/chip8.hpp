#include "display.hpp"
#include <string>
namespace chip8 {

class Chip8 {

public:
  int init();
  int run();
  bool load_rom(std::string path);
  void emulate_one_cycle();

private:
  // Variables
  uint8_t mem[4096]; // 4kB
  uint8_t V[16];     // Registers, V0-VF

  uint16_t pc; // Program counter
  uint16_t I;  // Index register

  uint8_t delay_timer;
  uint8_t sound_timer;

  uint8_t fontset[5 * 16] = {
      0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
      0x20, 0x60, 0x20, 0x20, 0x70, // 1
      0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
      0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
      0x90, 0x90, 0xF0, 0x10, 0x10, // 4
      0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
      0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
      0xF0, 0x10, 0x20, 0x40, 0x40, // 7
      0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
      0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
      0xF0, 0x90, 0xF0, 0x90, 0x90, // A
      0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
      0xF0, 0x80, 0x80, 0x80, 0xF0, // C
      0xE0, 0x90, 0x90, 0x90, 0xE0, // D
      0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
      0xF0, 0x80, 0xF0, 0x80, 0x80  // F
  };

  uint8_t keymap[16] = {SDLK_1, SDLK_2, SDLK_3, SDLK_4, SDLK_q, SDLK_w,
                        SDLK_e, SDLK_r, SDLK_a, SDLK_s, SDLK_d, SDLK_f,
                        SDLK_z, SDLK_x, SDLK_x, SDLK_v};

  uint8_t keys[16];

  uint16_t stack[16];
  uint16_t sp;

  uint8_t pixels[Display::height * Display::width];

  // Flags
  bool draw_flag = false;
  bool sound_flag = false;

  // Set to resolve ambiguous instructions to original CORMAC VIP instructions
  // Otherwise, will use the newer SUPER-CHIP instruction set
  bool use_cosmac_instructions = false;

  Display display;
};

} // namespace chip8
