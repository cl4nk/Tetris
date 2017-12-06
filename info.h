/* info.h - 24/02/2012 */

#ifndef INFO__H
#define INFO__H

#include <stdio.h>
#include <stdlib.h>
#include <gtk/gtk.h>
#include <time.h>
#include <string.h>
#include <SFML/Audio.h>


#define TIMEOUT_DRAW 30
#define TIMEOUT_STEP 70
#define TIMEOUT_FIRST_LEVEL 500
#define INTERVAL_BY_LEVEL 30
#define TIMEOUT_SECOND 1000

#define SIZE_PIECE 4
#define SIZE_CELL 20
#define GRID_X 20
#define GRID_Y 20

#define NB_ROTATIONS 4
#define NB_PATTERN_ROWS 4
#define NB_PATTERN_COLS 4

#define NB_NEXT_PIECES 4
#define NB_LEVEL 15

#define BS_FILENAME ".best_scores"
#define SAVE_FILENAME ".save"

#define COLOR_WHITE 0xFFFFFF
#define COLOR_WHITE_BACKGROUND 0xFFFFEE
#define COLOR_BLACK 0x000000

#define COLOR_INDEX_WHITE_BLOCK 12
#define COLOR_INDEX_BLACK_BLOCK 11
#define COLOR_INDEX_HOLO_BLOCK 13

#define NBR_NORMAL_PIECES 7
#define NBR_PIECES 11

#define COST_CELL_BONUS 250
#define COST_BLOCK_BONUS 200
#define COST_LINE_BONUS 100

#define NBR_SOUNDS 7
#define SOUND_FOLDER "sound/"


typedef enum SoundBuffers
{
	BUFFER_LEFTRIGHT,
	BUFFER_DOWN,
	BUFFER_ADD_PIECE,
	BUFFER_ROTATE,
	BUFFER_ROTATE_FAIL,
	BUFFER_TOUCH_LF,
	BUFFER_CLEAR_LINE,
	BUFFER_BONUS,
	BUFFER_SWITCH,
	BUFFER_GAMEOVER,
	NBR_BUFFERS
} SoundBuffers;

typedef struct SoundStack
{
	sfSound* list_sounds[NBR_SOUNDS];
	int nbr_sounds;
	int sound_to_load;
} SoundStack;

typedef enum GameSkins
{
	CLASSIC_SKIN,
	BLACK_SKIN,
	MARIO_SKIN,
	DB_SKIN,
	HOLO_SKIN,
	UNICORN_SKIN,
	NBR_SKINS
} GameSkins;

typedef enum GameModes
{
	CLASSIC_MODE,
	BLOCK_MODE,
	BELOW_MODE,
	STRANGE_MODE,
	NBR_MODES
} GameModes;

typedef enum TypeBonus
{
	NO_BONUS,
	CELL_BONUS,
	BLOCK_BONUS,
	LINE_BONUS,
	NBR_BONUS
} TypeBonus;

typedef struct GameBonus
{
	TypeBonus type;
	int x, y;
} GameBonus;

typedef struct BestScores
{
	char** names;
	int* scores;
} BestScores;

typedef struct AnimatedDestroyedLines
{
	int lines[4];
	int nbrLines;
	int duration, time_left;
	int ready;
	int color;
	int gameover;
} AnimatedDestroyedLines;

typedef struct AnimatedText
{
	int int_to_print;
	int duration;
	int time_left;
	int x, y;
	int ready;
} AnimatedText;

typedef struct Configs
{
	int patterns[NB_ROTATIONS][NB_PATTERN_ROWS][NB_PATTERN_COLS];
} Configs;

typedef struct Piece
{
	int current_rotation;
	Configs config;
	int color;
} Piece;

typedef struct
{
	int anim;
	gint timeout_draw;
	gint timeout_step;
	gint timeout_down;
	gint timeout_second;

	GameSkins skin;
	GameModes mode;
	GameBonus* bonus;
	AnimatedText *aText_down, *aText_line;
	AnimatedDestroyedLines* aLines;
	BestScores* bestScores;
	GdkPixbuf *bg_img, *cell_img;

	sfSoundBuffer* soundBuffers[NBR_BUFFERS];
	SoundStack* soundStack;

	sfMusic* music;

	int sound_is_playing;
	int **grid, width, height;
	int score_in_statusbar, time, level_in_statusbar, value_timeout_down;
	Piece *next_pieces, current_piece;
	int pos_x, pos_y;
	int key_Left, key_Right, key_Up, key_Down, key_space;
	int current_level, current_score;
	int new_value_timeout_down;
	int gameover;
	int lines_left_to_next_level;
} Info;

void info_init_default(Info* info, BestScores* bestScores);

void info_anim_start(Info* info, gpointer data);
void info_anim_stop(Info* info);
void info_update_timeout_down(Info* info, gpointer data);

Piece new_piece(int mode);
Piece new_piece_with_type(int type);
int info_piece_can_be_added(Info* info);
int info_piece_get_bottom_block(Info* info);
int info_piece_can_go_left(Info* info);
void info_piece_go_left(Info* info);
int info_piece_can_go_down(Info* info);
void info_piece_go_down(Info* info);
int info_piece_can_go_down_xy(Info* info, int pos_x, int pos_y);
void info_piece_go_right(Info* info);
int info_piece_can_go_right(Info* info);
void info_piece_rotation(Info* info);
void info_piece_add_to_grid(Info* info);
void info_add_piece_to_queue(Info* info);
void info_piece_next_step(Info* info);

void info_add_to_score(Info* info, int score);
void info_add_piece_to_grid(Info* info);

void cell_draw(GdkWindow* win, GdkGC* gc, int filled, int x, int y, int width, int height, int color, int skin,
	GdkPixbuf* img);
void info_current_piece_draw(GdkWindow* win, GdkGC* gc, Info* info, int size_cell);

void piece_in_grid_draw(GdkWindow* win, GdkGC* gc, Piece* piece, int x, int y, int size_cell, int skin, GdkPixbuf* img);
void piece_draw(GdkWindow* win, GdkGC* gc, Piece* piece, int x, int y, int size_cell, int skin, GdkPixbuf* img);
void info_next_pieces_draw(GdkWindow* win, GdkGC* gc, Info* info, int size_cell);
void area_next_pieces_num_draw(GtkWidget* win, GdkGC* gc, int size_cell, int skin);

void info_shadow_draw(GdkWindow* win, GdkGC* gc, Info* info, int size_cell);
void info_init_pieces(Info* info);
void info_pieces_switch(Info* info);

void info_set_level(Info* info, int level);

void info_background_draw(GdkWindow* win, GdkGC* gc, Info* info, int width, int height);

GtkWidget* info_get_mode_combo(Info* info);
GtkWidget* info_get_skin_combo(Info* info);
GtkWidget* info_get_level_combo(Info* info);
GtkWidget* bestScores_get_table(BestScores* bestScores);
void bestScores_reset(BestScores* bestScores, char* filename);
void bestScores_init(BestScores* bestScores);
void bestScores_add_item(BestScores* bestScores, char* name, int score);
void bestScores_add_item_at_index(BestScores* bestScores, char* name, int score, int index);
void bestScores_show(BestScores * bestScores, GtkWindow * win);

void bestScores_fill_with_file(BestScores* bestScores, char* filename);
void bestScores_write_file(BestScores* bestScores, char* filename);

int info_grid_block_is_empty(Info* info, int x, int y);
void info_grid_destroy_block(Info* info, int x, int y);
void info_grid_add_line_below(Info* info);
void info_on_gameover(Info* info);

void gameBonus_init(GameBonus* bonus);
void gameBonus_set_type(GameBonus* bonus, TypeBonus type);
void info_gameBonus_next_step(Info* info);
void info_bonus_draw(GdkWindow* win, GdkGC* gc, Info* info, int size_cell);

void info_init_grid(Info* info);
int info_grid_line_is_full(Info* info, int line);
int info_grid_is_empty(Info* info);
int info_grid_line_is_empty(Info* info, int line);
void info_grid_destroy_line(Info* info, int line);
void info_grid_background_draw(GdkWindow* win, GdkGC* gc, Info* info, int size_cell);
void info_grid_draw(GdkWindow* win, GdkGC* gc, Info* info, int size_cell);

void animatedText_init(AnimatedText* aText);
void animatedText_setup(AnimatedText* aText, int int_to_print, int duration, int x, int y);
void animatedText_draw(GtkWidget* win, GdkGC* gc, AnimatedText* aText, int size_cell, int skin);

void animatedDestroyedLines_init(AnimatedDestroyedLines* aLines);
void animatedDestroyedLines_setup(AnimatedDestroyedLines* aLines, int duration, int color, int gameover);
void info_anim_destroyed_lines_step(Info* info, int time_left);
void animatedDestroyedLines_reset_lines(AnimatedDestroyedLines* aLines);
void animatedDestroyedLines_add_line(AnimatedDestroyedLines* aLines, int line);
void animatedDestroyedLines_set_ready(AnimatedDestroyedLines* aLines);

void info_grid_color_line(Info* info, int line, int color);

void info_gameover_draw(GtkWidget* win, GdkGC* gc, Info* info, int size_cell);

void pix_load_all(Info* info);
void soundBuffers_load_all(Info* info);
void soundStack_init(SoundStack* stack, int n);
void info_sound_play(Info* info, int index);

void info_soundBuffers_destroy(Info* info);
void soundStack_destroy(SoundStack* stack);
void info_init_music(Info* info);
void info_play_music(Info* info);
void info_pause_music(Info* info);

void info_save_in_file(Info* info, char* filename);
void info_fill_with_file(Info* info, char* filename);

#endif /* INFO__H */
