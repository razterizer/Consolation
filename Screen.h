#pragma once
#include "SpriteHandler.h"
#include <cmath>

// Game Over
int game_over_timer = 10;
int you_won_timer = 10;
// Wavey Text
float wave_x0 = 0.f;
float wave_f = 0.4f;
float wave_a = 5.f;
float wave_step = 0.1f;
// Misc
const float pix_ar = 1.9f;//1.5f; // height/width of "pixel".
const float pix_ar_sq = pix_ar*pix_ar;
const float pix_ar2 = 1.5f;
const float pix_ar2_sq = pix_ar2*pix_ar;


void clear_screen()
{
  printf("\x1b[2J");
}

void return_cursor()
{
  printf("\x1b[H");
}

void gotorc(int r, int c)
{
  printf("%c[%d;%df", 0x1B, r, c);
}

void draw_frame(SpriteHandler& sh, Text::Color fg_color)
{
  sh.write_buffer("+" + str::rep_char('-', 78) + "+", 0, 0, fg_color);
  for (int r=1; r<29; ++r)
  {
    sh.write_buffer("|", r, 0, fg_color);
    sh.write_buffer("|", r, 79, fg_color);
  }
  sh.write_buffer("+" + str::rep_char('-', 78) + "+", 29, 0, fg_color);
}

// http://www.network-science.de/ascii/
// http://patorjk.com/software/taag/#p=display&f=Graffiti&t=Game%20Over Graffiti
//  ________    _____      _____  ___________
// /  _____/   /  _  \    /     \ \_   _____/
///   \  ___  /  /_\  \  /  \ /  \ |    __)_
//\    \_\  \/    |    \/    Y    \|        \
// \______  /\____|__  /\____|__  /_______  /
//        \/         \/         \/        \/
//____________   _________________________
//\_____  \   \ /   /\_   _____/\______   \
// /   |   \   Y   /  |    __)_  |       _/
///    |    \     /   |        \ |    |   \
//\_______  /\___/   /_______  / |____|_  /
//        \/                 \/         \/
void draw_game_over(SpriteHandler& sh)
{
  auto wave_func = [](float c, int i)
  {
    float x = static_cast<float>(i)/12.f;
    auto x1 = c + wave_a*std::sin(wave_f*2*M_PI*(x + wave_x0));

    return std::round(x1);
  };

  int c = 18;
  sh.write_buffer("  ________    _____      _____  ___________", 7,  wave_func(c, 0), Text::Color::DarkRed, Text::Color::White);
  sh.write_buffer(" /  _____/   /  _  \\    /     \\ \\_   _____/", 8,  wave_func(c, 1), Text::Color::DarkRed, Text::Color::Yellow);
  sh.write_buffer("/   \\  ___  /  /_\\  \\  /  \\ /  \\ |    __)_ ",9,  wave_func(c, 2), Text::Color::DarkRed, Text::Color::Yellow);
  sh.write_buffer("\\    \\_\\  \\/    |    \\/    Y    \\|        \\", 10, wave_func(c, 3), Text::Color::DarkRed, Text::Color::DarkYellow);
  sh.write_buffer(" \\______  /\\____|__  /\\____|__  /_______  /", 11, wave_func(c, 4), Text::Color::DarkRed, Text::Color::DarkYellow);
  sh.write_buffer("        \\/         \\/         \\/        \\/ ", 12, wave_func(c, 5), Text::Color::DarkRed, Text::Color::DarkYellow);
  sh.write_buffer("____________   _________________________   ", 13, wave_func(c, 6), Text::Color::DarkRed, Text::Color::DarkYellow);
  sh.write_buffer("\\_____  \\   \\ /   /\\_   _____/\\______   \\  ", 14, wave_func(c, 7), Text::Color::DarkRed, Text::Color::DarkYellow);
  sh.write_buffer(" /   |   \\   Y   /  |    __)_  |       _/  ", 15, wave_func(c, 8), Text::Color::DarkRed, Text::Color::DarkYellow);
  sh.write_buffer("/    |    \\     /   |        \\ |    |   \\  ", 16, wave_func(c, 9), Text::Color::DarkRed, Text::Color::Yellow);
  sh.write_buffer("\\_______  /\\___/   /_______  / |____|_  /  ", 17, wave_func(c, 10), Text::Color::DarkRed, Text::Color::Yellow);
  sh.write_buffer("        \\/                 \\/         \\/   ", 18, wave_func(c, 11), Text::Color::DarkRed, Text::Color::White);

  wave_x0 += wave_step;
  if (std::abs(wave_x0 - 100.f) < 1e-4f)
    wave_x0 = 0.f;
}

//_____.___.               __      __            ._.
//\__  |   | ____  __ __  /  \    /  \____   ____| |
// /   |   |/  _ \|  |  \ \   \/\/   /  _ \ /    \ |
// \____   (  <_> )  |  /  \        (  <_> )   |  \|
// / ______|\____/|____/    \__/\  / \____/|___|  /_
// \/                            \/             \/\/
void draw_you_won(SpriteHandler& sh)
{
  wave_f = 1.5f;//0.4f;
  wave_a = 1.f;//5.f;
  wave_step = 0.07f; //0.1f;

  auto wave_func = [](float c, int i)
  {
    float x = static_cast<float>(i)/12.f;
    auto x1 = c + wave_a*std::sin(wave_f*2*M_PI*(x + wave_x0));

    return std::round(x1);
  };

  int c = 15;
  sh.write_buffer("_____.___.               __      __            ._.", 10, wave_func(c, 0), Text::Color::DarkBlue, Text::Color::Cyan);
  sh.write_buffer("\\__  |   | ____  __ __  /  \\    /  \\____   ____| |", 11, wave_func(c, 1), Text::Color::DarkBlue, Text::Color::Cyan);
  sh.write_buffer(" /   |   |/  _ \\|  |  \\ \\   \\/\\/   /  _ \\ /    \\ |", 12, wave_func(c, 2), Text::Color::DarkBlue, Text::Color::Cyan);
  sh.write_buffer(" \\____   (  <_> )  |  /  \\        (  <_> )   |  \\|", 13, wave_func(c, 3), Text::Color::DarkBlue, Text::Color::DarkCyan);
  sh.write_buffer(" / ______|\\____/|____/    \\__/\\  / \\____/|___|  /_", 14, wave_func(c, 4), Text::Color::DarkBlue, Text::Color::DarkCyan);
  sh.write_buffer(" \\/                            \\/             \\/\\/", 15, wave_func(c, 5), Text::Color::DarkBlue, Text::Color::DarkCyan);

  wave_x0 += wave_step;
  if (std::abs(wave_x0 - 100.f) < 1e-4f)
    wave_x0 = 0.f;
}

void draw_paused(SpriteHandler& sh, int anim_ctr)
{
  int anim = anim_ctr % 10;
  std::string msg;
  switch (anim)
  {
    case 6: msg = "PAUSED"; break;
    case 5: msg = "P USED"; break;
    case 4: msg = "P USE "; break;
    case 3: msg = "P U E "; break;
    case 2: msg = "  U E "; break;
    case 1: msg = "  U   "; break;
    case 0: msg = "      "; break;
    default: msg = "PAUSED"; break;
  }
  sh.write_buffer(msg, 15, 36, Text::Color::White, Text::Color::DarkCyan);
}
