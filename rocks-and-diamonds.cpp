#include <string>
#include <string.h>
#include <memory>
#include <cstdlib>

#include "rocks-and-diamonds.hpp"

using namespace blit;

#define PLAYER_TOP (player.position.y)
#define PLAYER_BOTTOM (player.position.y + player.size.y)
#define PLAYER_RIGHT (player.position.x + player.size.x)
#define PLAYER_LEFT (player.position.x)

constexpr uint16_t screen_width = 160;
constexpr uint16_t screen_height = 120;

constexpr uint16_t level_width = 64;
constexpr uint16_t level_height = 64;

uint8_t *level_data;

Timer timer_level_update;
TileMap* level;

struct Player {
  Point start;
  Point position;
  Point screen_location;
  uint32_t score;
  Vec2 camera;
  Point size = Point(1, 1);
};

Player player;

Mat3 camera;

enum entityType {
  NOTHING = 0x00,
  DIRT = 0x01,
  WALL = 0x02,
  ROCK = 0x10,
  DIAMOND = 0x11,
  PLAYER = 0x30
};

// Line-interrupt callback for level->draw that applies our camera transformation
// This can be expanded to add effects like camera shake, wavy dream-like stuff, all the fun!
std::function<Mat3(uint8_t)> level_line_interrupt_callback = [](uint8_t y) -> Mat3 {
  return camera;
};

void update_camera(uint32_t time) {
  // Create a camera transform that centers around the player's position
  if(player.camera.x < player.position.x) {
    player.camera.x += 0.1;
  }
  if(player.camera.x > player.position.x) {
    player.camera.x -= 0.1;
  }
  if(player.camera.y < player.position.y) {
    player.camera.y += 0.1;
  }
  if(player.camera.y > player.position.y) {
    player.camera.y -= 0.1;
  }

  camera = Mat3::identity();
  camera *= Mat3::translation(Vec2(player.camera.x * 8.0f, player.camera.y * 8.0f)); // offset to middle of world      
  camera *= Mat3::translation(Vec2(-screen_width / 2, -screen_height / 2)); // transform to centre of framebuffer
}

Point level_first(entityType entity) {
  for(auto x = 0; x < level_width; x++) {
    for(auto y = 0; y < level_height; y++) {
      if (level_data[y * level_width + x] == entity) {
        return Point(x, y);
      } 
    }
  }
  return Point(-1, -1);
}

void level_set(Point location, entityType entity) {
  level_data[location.y * level_width + location.x] = entity;
}

bool player_at(Point location) {
  return (player.position.x == location.x && player.position.y == location.y);
}

entityType level_get(Point location) {
  if(location.y < 0 || location.x < 0 || location.y >= level_height || location.x >= level_width) {
    return WALL;
  }
  entityType entity = (entityType)level_data[location.y * level_width + location.x];
  if(entity == NOTHING && player_at(location)) {
    entity = PLAYER;
  }
  return entity;
}

void update_level(Timer &timer) {
  Point location = Point(0, 0);
  for(location.x = 0; location.x < level_width; location.x++) {
    for(location.y = level_height - 1; location.y > 0; location.y--) {
      Point location_below = location + Point(0, 1);
      entityType current = level_get(location);
      entityType below = level_get(location_below);
      if(current == ROCK) {
        if (below == NOTHING) {
          level_set(location, NOTHING);
          level_set(location_below, ROCK);
        } else if (below == ROCK) {
          entityType left = level_get(location + Point(-1, 0));
          entityType below_left = level_get(location + Point(-1, 1));
          entityType right = level_get(location + Point(1, 0));
          entityType below_right = level_get(location + Point(1, 1));

          if(left == NOTHING && below_left == NOTHING){
            level_set(location, NOTHING);
            level_set(location + Point(-1, 1), ROCK);
          } else if(right == NOTHING && below_right == NOTHING){
            level_set(location, NOTHING);
            level_set(location + Point(1, 1), ROCK);
          }
        }
      }
    }
  }
}

void init() {
  set_screen_mode(ScreenMode::lores);

  // Load the spritesheet from the linked binary blob
  screen.sprites = SpriteSheet::load((const uint8_t *)asset_sprites_png.data);

  // Load the level data from the linked binary blob into memory
  level_data = (uint8_t *)malloc(level_width * level_height);
  memcpy((void *)level_data, (const void *)asset_level_tmx.data, level_width * level_height);

  // Load our level data into the TileMap
  level = new TileMap((uint8_t *)level_data, nullptr, Size(level_width, level_height), screen.sprites);

  player.start = level_first(PLAYER);
  level_set(player.start, NOTHING);
  player.position.x = player.start.x;
  player.position.y = player.start.y;

  player.screen_location = Point(screen_width / 2, screen_height / 2);
  player.screen_location += Point(1, 1);
  
  timer_level_update.init(update_level, 250, -1);
  timer_level_update.start();
}

void render(uint32_t time_ms) {
  screen.pen = Pen(0, 0, 0);
  screen.clear();

  // Draw our level
  level->draw(&screen, Rect(0, 0, screen.bounds.w, screen.bounds.h), level_line_interrupt_callback);

  // Draw our character sprite
  screen.sprite(entityType::PLAYER, player.screen_location);

  screen.pen = Pen(255, 255, 255);
  screen.rectangle(Rect(0, 0, screen_width, 10));
  screen.pen = Pen(0, 0, 0);
  screen.text("Score: " + std::to_string(player.score), minimal_font, Point(1, 1));
  // screen.text(std::to_string(player.position.x), minimal_font, Point(0, 0));
  // screen.text(std::to_string(player.position.y), minimal_font, Point(0, 10));
}

void update(uint32_t time) {
  static uint32_t last_buttons = 0;
  static uint32_t last_repeat = 0;
  static uint32_t changed = 0;

  Point movement = Point(0, 0);

  changed = buttons ^ last_buttons;

  if(buttons & changed & Button::DPAD_UP) {
    movement.y = -1;
  }
  if(buttons & changed & Button::DPAD_DOWN) {
    movement.y = 1;
  }
  if(buttons & changed & Button::DPAD_LEFT) {
    movement.x = -1;
  }
  if(buttons & changed & Button::DPAD_RIGHT) {
    movement.x = 1;
  }

  player.position += movement;

  entityType standing_on = level_get(player.position);

  if(standing_on == WALL){
    player.position -= movement;
  }

  if(standing_on == ROCK){
    if(movement.x > 0 && level_get(player.position + Point(1, 0)) == NOTHING){
      level_set(player.position + Point(1, 0), ROCK);
      level_set(player.position, NOTHING);
    }
    else if(movement.x < 0 && level_get(player.position + Point(-1, 0)) == NOTHING){
      level_set(player.position + Point(-1, 0), ROCK);
      level_set(player.position, NOTHING);
    }
    else {
      player.position -= movement;
    }
  }

  if(standing_on == DIAMOND){
    player.score += 1;
    level_set(player.position, NOTHING);
  }

  if(standing_on == DIRT){
    level_set(player.position, NOTHING);
  }

  last_buttons = buttons;

  update_camera(time);
}