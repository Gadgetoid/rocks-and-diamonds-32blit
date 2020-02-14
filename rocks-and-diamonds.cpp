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

std::function<Mat3(uint8_t)> pan = [](uint8_t y) -> Mat3 {
  Mat3 transform = Mat3::identity();
  transform *= Mat3::translation(Vec2(player.x * 8.0f, player.y * 8.0f)); // offset to middle of world      
  transform *= Mat3::translation(Vec2(-screen_width / 2, -screen_height / 2)); // transform to centre of framebuffer
  return transform;
};

void init() {
  set_screen_mode(ScreenMode::lores);
  screen.sprites = SpriteSheet::load((const uint8_t *)_binary_sprites_start);
  level = new TileMap((uint8_t *)_binary_level_start, nullptr, Size(level_width, level_height), screen.sprites);
}

void render(uint32_t time_ms) {
  screen.pen = Pen(20, 30, 40);
  screen.clear();

  screen.pen = Pen(80, 120, 160);
  screen.text(data_sprites, minimal_font, Point(0, 0));


  level->draw(&screen, Rect(0, 0, screen.bounds.w, screen.bounds.h), pan);
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
}