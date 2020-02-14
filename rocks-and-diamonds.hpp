#pragma once

#include <stdint.h>

#include "32blit.hpp"

void init();
void update(uint32_t time);
void render(uint32_t time);

extern const char _binary_level_start[];
extern const char _binary_sprites_start[];

const char *data_level = _binary_level_start;
const char *data_sprites = _binary_sprites_start;