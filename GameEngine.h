//
//  GameEngine.h
//  Termin8or
//
//  Created by Rasmus Anthin on 2023-12-30.
//

#pragma once
#include "Keyboard.h"
#include "Screen.h"
#include <Core/Delay.h>
#include <Core/Rand.h>
#include <Core/Math.h>
#include <Core/FolderHelper.h>
#include <Core/OneShot.h>
#include <chrono>

struct GameEngineParams
{
  bool enable_title_screen = true;
  bool enable_instructions_screen = true;
  bool enable_quit_confirm_screen = true;
  bool enable_hiscores = true;
  
  Text::Color screen_bg_color_default = Text::Color::Default;
  Text::Color screen_bg_color_title = Text::Color::Default;
  Text::Color screen_bg_color_instructions = Text::Color::Default;
  
  std::optional<Text::Color> screen_bg_color_paused = std::nullopt;
  
  std::optional<Text::Color> screen_bg_color_quit_confirm = Text::Color::DarkCyan;
  styles::Style quit_confirm_title_style { Text::Color::Black, Text::Color::DarkCyan };
  styles::ButtonStyle quit_confirm_button_style { Text::Color::Black, Text::Color::DarkCyan, Text::Color::Cyan };
  styles::Style quit_confirm_info_style { Text::Color::White, Text::Color::DarkCyan };
  
  std::optional<Text::Color> screen_bg_color_input_hiscore = Text::Color::DarkGray;
  styles::Style input_hiscore_title_style { Text::Color::Green, Text::Color::Black };
  styles::PromptStyle input_hiscore_prompt_style { Text::Color::Green, Text::Color::Black, Text::Color::DarkGreen };
  styles::Style input_hiscore_info_style { Text::Color::DarkGreen, Text::Color::Black };
  
  std::optional<Text::Color> screen_bg_color_hiscores = Text::Color::DarkGray;
  styles::Style hiscores_title_style { Text::Color::Green, Text::Color::Black };
  styles::HiliteFGStyle hiscores_nr_style { Text::Color::Green, Text::Color::Black, Text::Color::Cyan };
  styles::HiliteFGStyle hiscores_score_style { Text::Color::Green, Text::Color::Black, Text::Color::Cyan };
  styles::HiliteFGStyle hiscores_name_style { Text::Color::Green, Text::Color::Black, Text::Color::Cyan };
  styles::Style hiscores_info_style { Text::Color::DarkGreen, Text::Color::Black };
};

template<int NR = 30, int NC = 80>
class GameEngine
{
  bool paused = false;
  bool show_title = true;
  bool show_instructions = false;
  bool show_quit_confirm = false;
  bool show_game_over = false;
  bool show_you_won = false;
  bool show_input_hiscore = false;
  bool show_hiscores = false;
  
  std::string_view path_to_exe; // Includes the <program>.exe file.
  std::string exe_file; // Only the <program>.exe file.
  std::string exe_path; // Excludes the <program>.exe file.
  GameEngineParams m_params;
  
  int delay = 50'000; // 100'000 (10 FPS) // 60'000 (16.67 FPS);
  int fps = 12; // 5
  
  YesNoButtons quit_confirm_button = YesNoButtons::No;
  
  std::vector<HiScoreItem> hiscore_list;
  int score = 0;
  HiScoreItem curr_score_item;
  int hiscore_caret_idx = 0;
  
  bool handle_hiscores(const HiScoreItem& curr_hsi)
  {
    const int c_max_num_hiscores = 20;
    const std::string c_file_path = folder::join_file_path({ exe_path, "hiscores.txt" });
  
    // Read saved hiscores.
    std::vector<std::string> lines;
    if (std::filesystem::exists(c_file_path))
    {
      if (!TextIO::read_file(c_file_path, lines))
        return false;
    }
    
    // Import saved hiscores from vector of strings.
    hiscore_list.clear();
    for (const auto& hs_str : lines)
    {
      std::istringstream iss(hs_str);
      HiScoreItem hsi;
      iss >> hsi.name >> hsi.score;
      hiscore_list.emplace_back(hsi);
    }
    
    // Add current hiscore.
    hiscore_list.push_back(curr_hsi);
    
    // Sort hiscores.
    auto num_hiscores = static_cast<int>(hiscore_list.size());
    stlutils::sort(hiscore_list,
      [](const auto& hsi_A, const auto& hsi_B) { return hsi_A.score > hsi_B.score; });
    if (num_hiscores >= 1)
      hiscore_list = stlutils::subset(hiscore_list, 0, std::min(num_hiscores, c_max_num_hiscores) - 1);
    else
    {
      std::cerr << "ERROR: Unknown error while saving hiscores!" << std::endl;
      return false;
    }
    
    // Export hiscores to vector of strings.
    lines.clear();
    for (const auto& hsi : hiscore_list)
    {
      std::ostringstream oss;
      oss << str::trim_ret(hsi.name) << " " << hsi.score;
      lines.emplace_back(oss.str());
    }
    
    // Save hiscores.
    if (!TextIO::write_file(c_file_path, lines))
      return false;
    
    return true;
  }
  
protected:
  float dt = static_cast<float>(delay) / 1e6f;
  float time = 0.f;
  double sim_time_s = 0.;
  std::chrono::time_point<std::chrono::steady_clock> sim_start_time_s;
  OneShot time_inited;
  
  Text t;
  SpriteHandler<NR, NC> sh;
  
  Text::Color bg_color = Text::Color::Default;
  
  int anim_ctr = 0;
  
  keyboard::KeyPressData kpd;
  
  void set_fps(float fps_val) { fps = fps_val; }
  
  // Used for dynamics and stuff.
  void set_delay_us(float delay_us)
  {
    delay = delay_us;
    dt = static_cast<float>(delay) / 1e6f;
  }
  
  int& ref_score() { return score; }
  
  double get_sim_time_s() const { return sim_time_s; }
  
  std::string get_exe_folder() const { return exe_path; }
  
  // Callbacks
  virtual void update() = 0;
  virtual void draw_title() = 0;
  virtual void draw_instructions() = 0;
  virtual void on_quit() {}
  virtual void on_exit_title() {}
  virtual void on_exit_instructions() {}
  virtual void on_enter_game_over() {}
  virtual void on_exit_game_over() {}
  virtual void on_enter_you_won() {}
  virtual void on_exit_you_won() {}
  virtual void on_enter_input_hiscore() {}
  virtual void on_exit_input_hiscore() {}
  virtual void on_enter_hiscores() {}
  
public:
  GameEngine(std::string_view exe_path,
             const GameEngineParams& params)
    : path_to_exe(exe_path)
    , m_params(params)
  {}
  
  virtual ~GameEngine() = default;

  void init()
  {
    keyboard::enableRawMode();
    
    clear_screen(); return_cursor();
    
    //nodelay(stdscr, TRUE);
    
    rnd::srand_time();
    
    std::tie(exe_path, exe_file) = folder::split_file_path(std::string(path_to_exe));
    
    if (time_inited.once())
      sim_start_time_s = std::chrono::steady_clock::now();
  }
  
  virtual void generate_data() = 0;
  
  void run()
  {
    // RT-Loop
    clear_screen();
    auto update_func = std::bind(&GameEngine::engine_update, this);
    Delay::update_loop(fps, update_func);
  }
  
  int get_fps() const { return fps; }
  int get_delay_us() const { return delay; }
  
  void set_state_game_over() { show_game_over = true; }
  void set_state_you_won() { show_you_won = true; }
  
private:
  bool engine_update()
  {
    if (time_inited.was_triggered())
    {
      auto curr_time = std::chrono::steady_clock::now();
      std::chrono::duration<double> elapsed_seconds = curr_time - sim_start_time_s;
      sim_time_s = elapsed_seconds.count();
    }
  
    return_cursor();
    sh.clear();
    
    kpd = keyboard::register_keypresses();
    if (kpd.quit)
    {
      math::toggle(show_quit_confirm);
      quit_confirm_button = YesNoButtons::No;
    }
    else if (kpd.pause)
      math::toggle(paused);
      
    if (!m_params.enable_quit_confirm_screen && kpd.quit)
    {
      restore_cursor();
      on_quit();
      return false;
    }
    else if (show_quit_confirm && !show_hiscores && !show_input_hiscore)
    {
      bg_color = m_params.screen_bg_color_quit_confirm.value_or(bg_color);
      draw_confirm_quit(sh, quit_confirm_button,
                        m_params.quit_confirm_title_style,
                        m_params.quit_confirm_button_style,
                        m_params.quit_confirm_info_style);
      if (kpd.curr_special_key == keyboard::SpecialKey::Left)
        quit_confirm_button = YesNoButtons::Yes;
      else if (kpd.curr_special_key == keyboard::SpecialKey::Right)
        quit_confirm_button = YesNoButtons::No;
      
      if (kpd.curr_special_key == keyboard::SpecialKey::Enter)
      {
        if (quit_confirm_button == YesNoButtons::Yes)
        {
          restore_cursor();
          on_quit();
          return false;
        }
        else
          show_quit_confirm = false;
      }
    }
    else
    {
      bg_color = m_params.screen_bg_color_default;
      if (m_params.enable_title_screen && show_title)
      {
        bg_color = m_params.screen_bg_color_title;
        draw_title();
        if (kpd.curr_key == ' ')
        {
          on_exit_title();
          show_title = false;
          show_instructions = true;
        }
      }
      else if (m_params.enable_instructions_screen & show_instructions)
      {
        bg_color = m_params.screen_bg_color_instructions;
        draw_instructions();
        if (kpd.curr_key == ' ')
        {
          on_exit_instructions();
          show_instructions = false;
        }
      }
      else if (show_game_over)
      {
        if (game_over_timer == 0)
          draw_game_over(sh);
        else
        {
          game_over_timer--;
          if (game_over_timer == 0)
            on_enter_game_over();
        }
        
        update();
          
        if (m_params.enable_hiscores && kpd.curr_key == ' ')
        {
          on_exit_game_over();
          show_game_over = false;
          show_input_hiscore = true;
          curr_score_item.init(score);
          hiscore_caret_idx = 0;
          on_enter_input_hiscore();
        }
      }
      else if (show_you_won)
      {
        if (you_won_timer == 0)
          draw_you_won(sh);
        else
        {
          you_won_timer--;
          if (you_won_timer == 0)
            on_enter_you_won();
        }
        
        update();
        
        if (m_params.enable_hiscores && kpd.curr_key == ' ')
        {
          on_exit_you_won();
          show_you_won = false;
          show_input_hiscore = true;
          curr_score_item.init(score);
          hiscore_caret_idx = 0;
          on_enter_input_hiscore();
        }
      }
      else if (show_input_hiscore)
      {
        bg_color = m_params.screen_bg_color_input_hiscore.value_or(bg_color);
        if (draw_input_hiscore(sh, kpd, curr_score_item, hiscore_caret_idx, anim_ctr,
                               m_params.input_hiscore_title_style,
                               m_params.input_hiscore_prompt_style,
                               m_params.input_hiscore_info_style))
        {
          on_exit_input_hiscore();
          handle_hiscores(curr_score_item);
          show_input_hiscore = false;
          show_hiscores = true;
          on_enter_hiscores();
        }
      }
      else if (show_hiscores)
      {
        bg_color = m_params.screen_bg_color_hiscores.value_or(bg_color);
        draw_hiscores(sh, hiscore_list,
                      m_params.hiscores_title_style,
                      m_params.hiscores_nr_style,
                      m_params.hiscores_score_style,
                      m_params.hiscores_name_style,
                      m_params.hiscores_info_style);
        
        if (kpd.curr_key == ' ' || kpd.quit)
        {
          restore_cursor();
          on_quit();
          return false;
        }
      }
      else if (paused)
      {
        bg_color = m_params.screen_bg_color_paused.value_or(bg_color);
        draw_paused(sh, anim_ctr);
      }
      else
        update();
    }
      
    sh.print_screen_buffer(t, bg_color);
    //sh.print_screen_buffer_chars();
    //sh.print_screen_buffer_fg_colors();
    //sh.print_screen_buffer_bg_colors();
    
  ///
    
    anim_ctr++;
    
    time += dt;
    
    return true;
  }
};
