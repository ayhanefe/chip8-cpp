#include "chip8.hpp"
#include <iostream>

int main(int argc, char *argv[]) {
  if (argc < 2) {
    std::cout << "Rom name is required";
    exit(0);
  }

  std::cout << "Initializing emulator\n";
  chip8::Chip8 chip8;
  chip8.init();

  std::cout << "Loading rom " << argv[1] << "\n";
  chip8.load_rom(argv[1]);

  std::cout << "Starting emulator\n";
  chip8.run();
}
