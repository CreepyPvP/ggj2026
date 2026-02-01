#include "score.h"

#include <stdio.h>
#include <string.h>

#include "raylib.h"
#include "raymath.h"

static Color gold_font_color = {248, 197, 58, 255};
static Color gold_font_shadow_color = {241, 100, 31, 255};
static Color silver_font_color = {179, 179, 195, 255};
static Color silver_font_shadow_color = {61, 63, 83, 255};
static Color name_font_color = {66, 191, 232, 255};
static Color name_font_shadow_color = {40, 110, 180, 255};
static Color title_font_color = {190, 103, 154, 255};
static Color title_font_shadow_color = {109, 50, 73, 255};
static Color font_color = {136, 110, 106, 255};
static Color font_shadow_color = {26, 30, 49, 255};
static Color score_font_color = {243, 132, 40, 255};
static Color score_font_shadow_color = {126, 48, 31, 255};
static Color background_color = {244, 237, 216, 255};
static Color background_outline_color = {189, 164, 153, 255};
static Color background_shadow_color = {72, 59, 61, 255};

ScoreManager score_manager = {};

static Color GetPlacementColor(i32 place) {

    if (place == 1) return gold_font_color;
    if (place == 2) return silver_font_color;
    if (place == 3) return {241, 100, 31, 255};

    return {189, 164, 153, 255};
}
static Color GetPlacementShadowColor(i32 place) {

    if (place == 1) return gold_font_shadow_color;
    if (place == 2) return silver_font_shadow_color;
    if (place == 3) return {97, 39, 33, 255};

    return {109, 91, 89, 255};
}

static i32 GetPlacement(TimeScore *highlighted) {
    i32 place = 0;

    for (int i = 0; i < score_manager.score_count; ++i) {
        TimeScore *entry = score_manager.scores + i;
        if (entry->level != highlighted->level) {
            continue;
        }
        place++;
        if (entry == highlighted) {
            return place;
        }
    }
}

static Vector2 DrawCenteredText(f32 x, f32 y, const char *text, f32 size, Color color) {
    f32 defaultFontSize = 10; // Default Font chars height in pixel
    if (size < defaultFontSize) size = defaultFontSize;
    f32 spacing = size / defaultFontSize;

    Vector2 shape = MeasureTextEx(GetFontDefault(), text, size, spacing);

    DrawTextEx(GetFontDefault(), text, Vector2{x - shape.x / 2, y}, size, spacing, color);

    return Vector2{x + shape.x / 2, y + shape.y};
}

static void DrawRightText(f32 x, f32 y, const char *text, f32 size, Color color) {
    f32 defaultFontSize = 10; // Default Font chars height in pixel
    if (size < defaultFontSize) size = defaultFontSize;
    f32 spacing = size / defaultFontSize;

    Vector2 shape = MeasureTextEx(GetFontDefault(), text, size, spacing);

    DrawTextEx(GetFontDefault(), text, Vector2{x - shape.x, y}, size, spacing, color);
}

static Vector2 DrawLeftText(f32 x, f32 y, const char *text, f32 size, Color color) {
    f32 defaultFontSize = 10; // Default Font chars height in pixel
    if (size < defaultFontSize) size = defaultFontSize;
    f32 spacing = size / defaultFontSize;
    Vector2 shape = MeasureTextEx(GetFontDefault(), text, size, spacing);


    DrawTextEx(GetFontDefault(), text, Vector2{x, y}, size, spacing, color);
    return Vector2{x + shape.x, y + shape.y};

}

static bool DrawScoreInput(Vector2 offset, Vector2 size) {
    TimeScore *score = score_manager.scores + score_manager.current_score;
    Vector2 center = offset + size / 2;
    Vector2 bot_bounds = offset + size;
    const char *cash_score = TextFormat("%ik $", score->cash_extracted);
    char placement[16] = "";

    i32 place = GetPlacement(score);
    sprintf(placement, "#%i", place);


    DrawRightText(center.x, offset.y + 5, "Placement: ", 50, font_shadow_color);
    DrawRightText(center.x, offset.y, "Placement: ", 50, font_color);
    DrawLeftText(center.x, offset.y + 5, placement, 50, GetPlacementShadowColor(place));
    DrawLeftText(center.x, offset.y, placement, 50, GetPlacementColor(place));

    DrawRightText(center.x, offset.y + 5 + 50, "Time: ", 50, font_shadow_color);
    DrawRightText(center.x, offset.y + 50, "Time: ", 50, font_color);
    DrawLeftText(center.x, offset.y + 5 + 50, cash_score, 50, score_font_shadow_color);
    DrawLeftText(center.x, offset.y + 50, cash_score, 50, score_font_color);

    DrawCenteredText(center.x, bot_bounds.y - 160 + 7, "Enter name:", 70, font_shadow_color);
    DrawCenteredText(center.x, bot_bounds.y - 160, "Enter name:", 70, font_color);

    if (place == 1) {
        f32 mod = sinf(GetTime() * 10) / 2 + 0.5f;
        DrawRightText(offset.x + size.x, offset.y - 80 + 6 + 1.5 * mod, "NEW HIGHSCORE", 70 + 10 * mod, gold_font_shadow_color);
        DrawRightText(offset.x + size.x, offset.y - 80, "NEW HIGHSCORE", 70 + 10 * mod, gold_font_color);
    }

    Vector2 name_text_pos = {center.x, bot_bounds.y - 80};
    DrawCenteredText(name_text_pos.x, name_text_pos.y + 7, score->name, 70, font_shadow_color);
    Vector2 end = DrawCenteredText(name_text_pos.x, name_text_pos.y, score->name, 70, font_color);


    i32 name_len = strlen(score->name);
    bool text_max_length_reached = name_len >= lengthof(score->name) - 2;


    if (!text_max_length_reached && sinf(GetTime() * 14) > 0) {
        DrawLeftText(end.x, name_text_pos.y + 7, "_", 70, font_shadow_color);
        DrawLeftText(end.x, name_text_pos.y, "_", 70, font_color);
    }

    if (name_len != 0) {
        if (IsKeyPressed(KEY_BACKSPACE)) {
            score->name[name_len - 1] = '\0';
        }
    }

    if (!text_max_length_reached) {
        for (int i = KEY_A; i <= KEY_Z; ++i) {
            if (IsKeyPressed(i)) {
                score->name[name_len] = 'A' + (i - KEY_A);
                score->name[name_len + 1] = '\0';
                goto post_char;
            }
        }

        for (int i = KEY_ZERO; i <= KEY_NINE; ++i) {
            if (IsKeyPressed(i)) {
                score->name[name_len] = '0' + (i - KEY_ZERO);
                score->name[name_len + 1] = '\0';
                goto post_char;
            }
        }

    post_char:;
    }


    if (IsKeyPressed(KEY_ENTER)) {
        if (name_len == 0) {
            strcpy(score->name, "ANONYMOUS");
        }

        return true;
    }
    return false;
}

static void DrawScoreEntry(Vector2 offset, Vector2 size, TimeScore *score, bool highlight) {


    char placement[16] = "...";
    const char *cash_score = "--k $";
    const char *name = "-----------";
    offset.x += size.x/2 - 300;

    u32 place = 100;

    if (score) {
        place = GetPlacement(score);
        sprintf(placement, "#%i", GetPlacement(score));
        cash_score = TextFormat("%ik $", score->cash_extracted);

        name = score->name;
    }

    if (highlight) {
        DrawRectangle(offset.x-5, offset.y, 800, size.y - 6, font_shadow_color);
        DrawRectangle(offset.x-5, offset.y, 800, size.y - 12, font_color);
    }

    DrawLeftText(offset.x, offset.y + 6, placement, 60, GetPlacementShadowColor(place));
    DrawLeftText(offset.x, offset.y, placement, 60, GetPlacementColor(place));

    DrawLeftText(offset.x + 120, offset.y + 6, cash_score, 60, font_shadow_color);
    DrawLeftText(offset.x + 120, offset.y, cash_score, 60, font_color);

    DrawLeftText(offset.x + 420, offset.y + 6, name, 60, name_font_shadow_color);
    DrawLeftText(offset.x + 420, offset.y, name, 60, name_font_color);
}

static void DrawScoreList(Vector2 offset, Vector2 size, bool highlight_last) {
    Vector2 score_entry_size = {size.x, 70};

    i32 entry_count = floorf(size.y / score_entry_size.y);

    bool appended_highlight = false;
    TimeScore *highlighted = NULL;

    if (highlight_last) {
        highlighted = score_manager.scores + score_manager.current_score;
        i32 placement = GetPlacement(highlighted);

        if (placement >= entry_count) {
            entry_count -= 2;
            appended_highlight = true;
        }
    }
    u32 i = 0;
    while (i < score_manager.score_count && entry_count > 0) {
        TimeScore *score = score_manager.scores + i;

        if (score->level != score_manager.level) {
            i++;
            continue;
        }
        entry_count--;
        DrawScoreEntry(offset + Vector2(0, score_entry_size.y * i), score_entry_size, score_manager.scores + i, score == highlighted);

        i++;
    }

    if (appended_highlight) {
        DrawScoreEntry(Vector2(offset.x, offset.y + size.y - score_entry_size.y * 2), score_entry_size, NULL, false);
        DrawScoreEntry(Vector2(offset.x, offset.y + size.y - score_entry_size.y), score_entry_size, highlighted, true);
    }
}


void ToggleList(u32 level) {
    if (score_manager.show_screen) score_manager.show_screen = false;
    else ShowList(level);

}
void ShowList(u32 level) {
    score_manager.highlight_current = false;
    score_manager.input_mode = false;
    score_manager.show_screen = true;
    score_manager.level = level;
}

TimeScore *AddNewScore(u32 level, u32 amount) {
    u32 insert_index = score_manager.score_count;

    for (u32 i = 0; i < score_manager.score_count; ++i) {
        if (score_manager.scores[i].cash_extracted < amount) {
            insert_index = i;
            break;
        }
    }

    score_manager.current_score = insert_index;
    score_manager.highlight_current = true;
    score_manager.input_mode = true;
    score_manager.show_screen = true;
    score_manager.level = level;

    if (score_manager.score_count != insert_index) {
        memcpy(score_manager.scores + insert_index + 1, score_manager.scores + insert_index, sizeof(TimeScore) * (score_manager.score_count - insert_index));
    }

    score_manager.scores[insert_index] = {
        .cash_extracted = amount,
        .level = level,
        .name = "",
    };
    score_manager.score_count++;

    return score_manager.scores + insert_index;
}

void DrawScoreScreen() {
    if (!score_manager.show_screen) return;
    int screen_width = GetScreenWidth();
    int screen_height = GetScreenHeight();
    Vector2 size = {screen_width * 0.9f, screen_height * 0.9f};
    Vector2 center = {screen_width / 2.0f, screen_height / 2.0f};
    Vector2 top_bounds = {center.x - size.x / 2, center.y - size.y / 2};
    DrawRectangleRec({top_bounds.x - 7, top_bounds.y + 14, size.x + 14, size.y + 7}, background_shadow_color);
    DrawRectangleRec({top_bounds.x - 7, top_bounds.y - 7, size.x + 14, size.y + 14}, background_outline_color);
    DrawRectangleRec({top_bounds.x, top_bounds.y, size.x, size.y}, background_color);
    DrawRectangleRec({top_bounds.x, top_bounds.y, size.x, 14}, background_shadow_color);

    f32 title_wave_offset = sinf(GetTime() * 5) * 4;
    DrawCenteredText(center.x, top_bounds.y + 19 + title_wave_offset, "Scoreboard", 90, title_font_shadow_color);
    DrawCenteredText(center.x, top_bounds.y + 10 + title_wave_offset, "Scoreboard", 90, title_font_color);

    Vector2 content_offset = {top_bounds.x + 20, top_bounds.y + 120};

    Vector2 content_size = {size.x - 40, size.y - 120 - 20};

    if (score_manager.input_mode) {
        if (DrawScoreInput(content_offset, content_size)) {
            score_manager.input_mode = false;
            PersistScores();
        }
    } else {
        DrawScoreList(content_offset, content_size, score_manager.highlight_current);
    }
}

//
// Persistence
//

static u32 buffer_offset;
static u8 *buffer;

void WriteU32(u32 value)
{
    u32 *target = (u32 *) (buffer + buffer_offset);
    *target = value;
    buffer_offset += 4;
}

u32 ReadU32()
{
    u32 *target = (u32 *) (buffer + buffer_offset);
    u32 value = *target;
    buffer_offset += 4;
    return value;
}

void WriteBytes(void *bytes, u32 size)
{
    u8 *target = buffer + buffer_offset;
    memcpy(target, bytes, size);
    buffer_offset += size;
}

void ReadBytes(void *target, u32 size)
{
    u8 *src = buffer + buffer_offset;
    memcpy(target, src, size);
    buffer_offset += size;
}

void PersistScores()
{
    buffer_offset = 0;
    if (!buffer) 
        buffer = (u8 *) malloc(1024 * 1024);

    WriteU32(score_manager.score_count);
    WriteBytes(score_manager.scores, sizeof(TimeScore) * score_manager.score_count);

    SaveFileData("scores.txt", buffer, buffer_offset);
}

void LoadScores()
{
    i32 file_size = 0;
    u8 *memory = LoadFileData("scores.txt", &file_size);
    if (!memory)
        return;

    buffer_offset = file_size;
    if (!buffer) 
        buffer = (u8 *) malloc(1024 * 1024);

    memcpy(buffer, memory, file_size);

    score_manager.score_count = ReadU32();
    ReadBytes(score_manager.scores, sizeof(TimeScore) * score_manager.score_count);
}
