#include <string>
#include <string.h>
#include <memory>
#include <cstdlib>

#include "rocks-and-diamonds.hpp"

using namespace blit;

constexpr uint16_t screen_width = 160;
constexpr uint16_t screen_height = 120;

constexpr uint16_t level_width = 64;
constexpr uint16_t level_height = 64;


TileMap* level;

Vec2 player = Vec2(12.0f, 1.0f);

Mat3 camera;

// Line-interrupt callback for level->draw that applies our camera transformation
// This can be expanded to add effects like camera shake, wavy dream-like stuff, all the fun!
std::function<Mat3(uint8_t)> level_line_interrupt_callback = [](uint8_t y) -> Mat3 {
  return camera;
};

void update_camera() {
  // Create a camera transform that centers around the player's position
  camera = Mat3::identity();
  camera *= Mat3::translation(Vec2(player.x * 8.0f, player.y * 8.0f)); // offset to middle of world      
  camera *= Mat3::translation(Vec2(-screen_width / 2, -screen_height / 2)); // transform to centre of framebuffer
}

void init() {
  set_screen_mode(ScreenMode::lores);

  // Load the spritesheet from the linked binary blob
  screen.sprites = SpriteSheet::load((const uint8_t *)_binary_sprites_start);

  // Load the level data from the linked binary blob
  // Note: This will be const! Should probably copy the level data to a local array first
  level = new TileMap((uint8_t *)_binary_level_start, nullptr, Size(level_width, level_height), screen.sprites);
}

void render(uint32_t time_ms) {
  screen.pen = Pen(0, 0, 0);
  screen.clear();

  // Draw our level
  level->draw(&screen, Rect(0, 0, screen.bounds.w, screen.bounds.h), level_line_interrupt_callback);

  // Draw our character sprite
  screen.blit(screen.sprites, Rect(0, 24, 8, 8), Point(screen_width / 2, screen_height / 2), false);
}

void update(uint32_t time) {
  static uint32_t last_buttons = 0;
  static uint32_t changed = 0;

  changed = buttons ^ last_buttons;

  if(buttons & Button::DPAD_LEFT) {
    player.x -= 0.05f;
  }
  if(buttons & Button::DPAD_RIGHT) {
    player.x += 0.05f;
  }
  if(buttons & Button::DPAD_DOWN) {
    player.y += 0.05f;
  }
  if(buttons & Button::DPAD_UP) {
    player.y -= 0.05f;
  }

  last_buttons = buttons;

  update_camera();
}