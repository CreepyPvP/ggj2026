#pragma once

#include "types.h"

struct TimeScore {
    u32 cash_extracted;
    u32 level;
    char name[12];
};


struct ScoreManager {

    TimeScore scores[1024];
    u32 score_count;

    u32 cash_extracted;
    u32 level;

    // Scoreboard display
    bool show_screen;
    bool highlight_current;
    u32 current_score;
    const char name_input_buffer[32];
    bool input_mode;
};

ScoreManager score_manager = {};

TimeScore *AddNewScore(u32 level, u32 amount);
void ShowList(u32 level);
void ToggleList(u32 level);

void DrawScoreScreen();