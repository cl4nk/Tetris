/* info.c - 24/02/2012 */

#include "info.h"
#include "gui.h"
#include "font.h"
#include "patterns.c"


/*------------------------------ T R I ------------------------------------*/

void SwapValuesTabChar(char* str1, char* str2)
{
	char* temp = (char *)malloc((strlen(str1) + 1) * sizeof(char));
	strcpy(temp, str1);
	strcpy(str1, str2);
	strcpy(str2, temp);
	free(temp);
}

void SwapValuesTabInt(int tab[], int index1, int index2)
{
	int a;
	a = tab[index1];
	tab[index1] = tab[index2];
	tab[index2] = a;
}

void TriABulle(int tab[], char** names, int n)
{
	int i, swapDone = 0;
	do
	{
		swapDone = 0;
		for (i = 0; i < n - 1; i++)
		{
			if (tab[i] < tab[i + 1])
			{
				SwapValuesTabInt(tab, i, i + 1);
				SwapValuesTabChar(names[i], names[i + 1]);
				swapDone = 1;
			}
		}
	}
	while (swapDone);
}

/*------------------------------ I N I T ------------------------------------*/


void info_init_default(Info* info, BestScores* bestScores)
{
	printf("info_init_default ...\n");

	info->anim = 0;
	info->timeout_draw = 0;
	info->timeout_step = 0;
	info->timeout_down = 0;
	info->timeout_second = 0;
	info->time = 0;
	info->value_timeout_down = TIMEOUT_FIRST_LEVEL;

	info->level_in_statusbar = -1;
	info->score_in_statusbar = -1;
	info->current_score = 0;
	info->width = 10;
	info->height = 22;

	info->key_Left = 0;
	info->key_Right = 0;
	info->key_Up = 0;
	info->key_Down = 0;
	info->key_space = 0;

	info->gameover = 0;
	info->sound_is_playing = 0;

	info->mode = CLASSIC_MODE;
	info->skin = CLASSIC_SKIN;

	info->bestScores = bestScores;

	info_set_level(info, 1);
	info_init_grid(info);
	info_init_pieces(info);
	pix_load_all(info);
	soundBuffers_load_all(info);
	info_init_music(info);
}


/*-------------------------- A N I M A T I O N ------------------------------*/

gboolean on_timeout_draw(gpointer data)
{
	Gui* g = gui_check(data);
	Info* info = g->info;
	area_redraw(g->area);
	area_redraw(g->area_pieces);

	if (info->gameover == TRUE) return TRUE;
	if (info->aLines->ready == TRUE)
	{
		info_anim_destroyed_lines_step(info, TIMEOUT_DRAW);
		return TRUE;
	}

	if (info->bonus->type != NO_BONUS) return TRUE;

	if (info->current_score != info->score_in_statusbar)
	{
		info->score_in_statusbar = info->current_score;
		char text[10];
		sprintf(text, "%d", info->score_in_statusbar);
		label_set_text(GTK_LABEL(g->layout.score_label), text);
	}

	if (info->current_level != info->level_in_statusbar)
	{
		info->level_in_statusbar = info->current_level;
		char text[10];
		sprintf(text, "%d", info->level_in_statusbar);
		label_set_text(GTK_LABEL(g->layout.level_label), text);
	}
	info->aText_down->time_left = info->aText_down->time_left + TIMEOUT_DRAW;
	if (info->aText_down->time_left > info->aText_down->duration)
		info->aText_down->ready = FALSE;

	info->aText_line->time_left = info->aText_line->time_left + TIMEOUT_DRAW;
	if (info->aText_line->time_left > info->aText_line->duration)
		info->aText_line->ready = FALSE;

	return TRUE;
}

gboolean on_timeout_step(gpointer data)
{
	Gui* g = gui_check(data);
	Info* info = g->info;

	if (info->gameover == TRUE) return TRUE;
	if (info->aLines->ready == TRUE) return TRUE;

	info_gameBonus_next_step(info);
	info_piece_next_step(info);

	return TRUE;
}

gboolean on_timeout_down(gpointer data)
{
	Gui* g = gui_check(data);
	Info* info = g->info;

	if (info->gameover == TRUE) return TRUE;
	if (info->aLines->ready == TRUE) return TRUE;
	if (info->bonus->type != NO_BONUS) return TRUE;

	info_update_timeout_down(info, data);

	if (info_piece_can_go_down(info))
	{
		info_piece_go_down(info);
	}
	else
	{
		info_add_piece_to_grid(info);
	}
	info_grid_add_line_below(info);
	return TRUE;
}

gboolean on_timeout_second(gpointer data)
{
	Gui* g = gui_check(data);
	Info* info = g->info;

	if (info->gameover == TRUE) return TRUE;
	if (info->aLines->ready == TRUE) return TRUE;
	if (info->bonus->type != NO_BONUS) return TRUE;

	info->time = info->time + TIMEOUT_SECOND;

	int time = info->time;
	time = time / 1000;

	char text[100];
	sprintf(text, "%02d:%02d", ((int)time / 60), ((int)time % 60));
	label_set_text(GTK_LABEL(g->layout.time_label), text);

	return TRUE;
}

void info_anim_start(Info* info, gpointer data)
{
	info_play_music(info);

	if (info->timeout_draw == 0)
		info->timeout_draw = g_timeout_add(TIMEOUT_DRAW, on_timeout_draw, data);

	if (info->timeout_step == 0)
		info->timeout_step = g_timeout_add(TIMEOUT_STEP, on_timeout_step, data);

	if (info->timeout_down == 0)
		info->timeout_down = g_timeout_add(info->value_timeout_down, on_timeout_down, data);

	if (info->timeout_second == 0)
		info->timeout_second = g_timeout_add_seconds(1, on_timeout_second, data);
}

void info_update_timeout_down(Info* info, gpointer data)
{
	if (info->value_timeout_down != info->new_value_timeout_down)
	{
		if (info->timeout_down != 0)
		{
			g_source_remove(info->timeout_down);
			info->timeout_down = 0;
		}
		info->value_timeout_down = info->new_value_timeout_down;
		info->timeout_down = g_timeout_add(info->value_timeout_down, on_timeout_down, data);
	}
}

void info_anim_stop(Info* info)
{
	info_pause_music(info);

	if (info->timeout_draw != 0)
	{
		g_source_remove(info->timeout_draw);
		info->timeout_draw = 0;
	}

	if (info->timeout_step != 0)
	{
		g_source_remove(info->timeout_step);
		info->timeout_step = 0;
	}

	if (info->timeout_down != 0)
	{
		g_source_remove(info->timeout_down);
		info->timeout_down = 0;
	}

	if (info->timeout_second != 0)
	{
		g_source_remove(info->timeout_second);
		info->timeout_second = 0;
	}
}

void info_gameover_draw(GtkWidget* win, GdkGC* gc, Info* info, int size_cell)
{
	if (info->gameover == FALSE) return;
	if (info->aLines->ready == TRUE) return;
	PangoLayout* layout = gtk_widget_create_pango_layout(win, NULL);
	font_set_name(layout, "8BIT WONDER 24");
	int color = colors_font [info->skin];
	gc_set_rgb_fg(gc, color);
	font_draw_text(win, gc, layout, FONT_TL, GRID_X + 2, GRID_Y + (info->height * size_cell) / 2, "GAMEOVER");
	g_object_unref(G_OBJECT(layout));
}

void info_next_pieces_draw(GdkWindow* win, GdkGC* gc, Info* info, int size_cell)
{
	int tab_x[] = {0, SIZE_PIECE + 2, 0, SIZE_PIECE + 2};
	int tab_y[] = {0, 0, SIZE_PIECE + 2, SIZE_PIECE + 2};
	for (int i = 0; i < NB_NEXT_PIECES; i++)
	{
		piece_draw(win, gc, &info->next_pieces[i], tab_x[i], tab_y[i], size_cell, info->skin, info->cell_img);
	}
}

void area_next_pieces_num_draw(GtkWidget* win, GdkGC* gc, int size_cell, int skin)
{
	int tab_x[] = {0, SIZE_PIECE + 2, 0, SIZE_PIECE + 2};
	int tab_y[] = {0, 0, SIZE_PIECE + 2, SIZE_PIECE + 2};
	for (int i = 0; i < NB_NEXT_PIECES; i++)
	{
		PangoLayout* layout = gtk_widget_create_pango_layout(win, NULL);
		font_set_name(layout, "8BIT WONDER 16");
		int color = colors_font [skin];
		gc_set_rgb_fg(gc, color);
		font_draw_text(win, gc, layout, FONT_TL, (tab_x[i] + SIZE_PIECE + 0.5) * size_cell,
		               (tab_y[i] + SIZE_PIECE + 0.5) * size_cell, "%d", i + 1);
		g_object_unref(G_OBJECT(layout));
	}
}

/*-----------------------A N I M A T E D --- T E X T --------------------------------*/

void animatedText_init(AnimatedText* aText)
{
	aText->ready = FALSE;
	aText->int_to_print = 0;
	aText->duration = 0;
	aText->time_left = 0;
	aText->x = 0;
	aText->y = 0;
}

void animatedText_setup(AnimatedText* aText, int int_to_print, int duration, int x, int y)
{
	aText->int_to_print = int_to_print;
	aText->duration = duration;
	aText->time_left = 0;
	aText->x = x;
	aText->y = y;
	aText->ready = TRUE;
}

void animatedText_draw(GtkWidget* win, GdkGC* gc, AnimatedText* aText, int size_cell, int skin)
{
	if (aText->ready == FALSE) return;
	PangoLayout* layout = gtk_widget_create_pango_layout(win, NULL);
	font_set_name(layout, "8BIT WONDER 16");
	int color = colors_font [skin];
	gc_set_rgb_fg(gc, color);
	font_draw_text(win, gc, layout, FONT_TL, GRID_X + aText->x * size_cell + size_cell / 2,
	               GRID_Y + aText->y * size_cell + size_cell / 2, "+%d", aText->int_to_print);
	g_object_unref(G_OBJECT(layout));
}


void animatedDestroyedLines_init(AnimatedDestroyedLines* aLines)
{
	aLines->ready = FALSE;
	aLines->nbrLines = 0;
	aLines->duration = 0;
	aLines->time_left = 0;
	aLines->color = 0;
	aLines->gameover = 0;
	animatedDestroyedLines_reset_lines(aLines);
}

void animatedDestroyedLines_setup(AnimatedDestroyedLines* aLines, int duration, int color, int gameover)
{
	aLines->duration = duration;
	aLines->time_left = 0;
	aLines->color = color;
	aLines->gameover = gameover;
}

void animatedDestroyedLines_reset_lines(AnimatedDestroyedLines* aLines)
{
	for (int i = 0; i < 4; i ++)
	{
		aLines->lines[i] = -1;
	}
	aLines->nbrLines = 0;
}

void animatedDestroyedLines_add_line(AnimatedDestroyedLines* aLines, int line)
{
	aLines->lines[aLines->nbrLines] = line;
	aLines->nbrLines = aLines->nbrLines + 1;
}

void animatedDestroyedLines_set_ready(AnimatedDestroyedLines* aLines)
{
	aLines->ready = TRUE;
}

void info_anim_destroyed_lines_step(Info* info, int time_left)
{
	if (info->aLines->ready == FALSE || info->aLines->nbrLines == 0) return;

	AnimatedDestroyedLines* aLines = info->aLines;
	aLines->time_left = aLines->time_left + time_left;
	if (aLines->time_left >= aLines->duration)
	{
		aLines->ready = FALSE;

		if (aLines->gameover == TRUE)
		{
			info_grid_color_line(info, aLines->lines[0], -1);
			info->gameover = TRUE;
			show_entry_dialog(info->bestScores, info->current_score);
			for (int y = 0; y < info->height; y++)
				info_grid_destroy_line(info, y);
		}
		else
		{
			for (int y = 0; y < aLines->nbrLines; y++)
				info_grid_destroy_line(info, aLines->lines[y]);
		}

		animatedDestroyedLines_reset_lines(info->aLines);
		info_add_piece_to_queue(info);
		return;
	}

	if (((int)aLines->time_left / 150) != ((int)aLines->time_left - time_left / 150))
	{
		int mColor = ((int)aLines->time_left / 150) % 2 == 0 ? -1 : aLines->color;
		if (aLines->gameover == TRUE)
		{
			int nextLine = (aLines->lines[0] + 1) % info->height;
			info_grid_color_line(info, aLines->lines[0], -1);
			info_grid_color_line(info, nextLine, mColor);
			aLines->lines[0] = nextLine;
		}
		else
		{
			for (int y = 0; y < aLines->nbrLines; y++)
				info_grid_color_line(info, aLines->lines[y], mColor);
		}
	}
}


/*------------------------------ P I E C E ------------------------------------------*/

Configs getConfig(int type)
{
	switch (type)
	{
	case 0: return type1_configs;
	case 1: return type2_configs;
	case 2: return type3_configs;
	case 3: return type4_configs;
	case 4: return type5_configs;
	case 5: return type6_configs;
	case 6: return type7_configs;
	case 7: return type8_configs;
	case 8: return type9_configs;
	case 9: return type10_configs;
	case 10: return type11_configs;
	default: return type1_configs;
	}
}

void info_init_pieces(Info* info)
{
	info->current_piece = new_piece(info->mode);
	info->next_pieces = malloc(sizeof (Piece) * 4);
	for (int i = 0; i < 4; i++)
	{
		info->next_pieces[i] = new_piece(info->mode);
	}
	info->pos_x = info->width / 2 - 2;
	info->pos_y = 0;
}

Piece new_piece(int mode)
{
	Piece piece;
	int range = mode == STRANGE_MODE ? NBR_PIECES : NBR_NORMAL_PIECES;
	int type = rand() % range;
	piece.current_rotation = 1;
	piece.config = getConfig(type);
	piece.color = type;

	return piece;
}

Piece new_piece_with_type(int type)
{
	Piece piece;
	piece.current_rotation = 1;
	piece.config = getConfig(type);
	piece.color = type;

	return piece;
}

void piece_in_grid_draw(GdkWindow* win, GdkGC* gc, Piece* piece, int x, int y, int size_cell, int skin, GdkPixbuf* img)
{
	for (int i = 0; i < NB_PATTERN_COLS; i ++)
	{
		for (int j = 0; j < NB_PATTERN_ROWS; j++)
		{
			if (y + j < 0) continue;
			if (piece->config.patterns[piece->current_rotation][i][j] == 1)
			{
				cell_draw(win, gc, TRUE, GRID_X + (i + x) * size_cell, GRID_Y + (j + y) * size_cell, size_cell, size_cell,
				          piece->color, skin, img);
			}
		}
	}
}

void piece_draw(GdkWindow* win, GdkGC* gc, Piece* piece, int x, int y, int size_cell, int skin, GdkPixbuf* img)
{
	for (int i = 0; i < NB_PATTERN_COLS; i ++)
	{
		for (int j = 0; j < NB_PATTERN_ROWS; j++)
		{
			if (piece->config.patterns[piece->current_rotation][i][j] == 1)
			{
				cell_draw(win, gc, TRUE, GRID_X + (i + x) * size_cell, GRID_Y + (j + y) * size_cell, size_cell, size_cell,
				          piece->color, skin, img);
			}
		}
	}
}

int info_piece_can_be_added(Info* info)
{
	Piece* piece = &info->current_piece;
	for (int i = 0; i < NB_PATTERN_COLS; i++)
		for (int j = 0; j < NB_PATTERN_ROWS; j++)
		{
			if (info->pos_y + j < 0) continue;
			if (piece->config.patterns[piece->current_rotation][i][j] != 0 && info->grid[i + info->pos_x][j + info->pos_y] >= 0)
				return FALSE;
		}

	return TRUE;
}

int info_piece_get_bottom_block(Info* info)
{
	Piece* piece = &info->current_piece;
	for (int i = 3; i >= 0; i--)
		for (int j = 0; j < NB_PATTERN_COLS; j++)
			if (piece->config.patterns[piece->current_rotation][j][i] != 0) return i;

	return -1;
}

int info_piece_can_go_left(Info* info)
{
	Piece* piece = &info->current_piece;

	for (int y = 0; y < SIZE_PIECE; y++)
	{
		if (info->pos_y + y < 0) continue;
		for (int x = 0; x < SIZE_PIECE; x ++)
		{
			if (piece->config.patterns[piece->current_rotation][x][y] != 0)
			{
				if (info->pos_x + x + - 1 < 0)
					return FALSE;
				if (info->grid[info->pos_x + x - 1][info->pos_y + y ] >= 0)
					return FALSE;
				break;
			}
		}
	}

	return TRUE;
}

void info_piece_go_left(Info* info)
{
	if (info_piece_can_go_left(info))
	{
		info->pos_x = info->pos_x - 1;
		info_sound_play(info, BUFFER_LEFTRIGHT);
	}
	else
	{
		info_sound_play(info, BUFFER_TOUCH_LF);
	}
}

void info_piece_go_down(Info* info)
{
	if (info_piece_can_go_down(info))
	{
		info->pos_y = info->pos_y + 1;
		info_sound_play(info, BUFFER_DOWN);
	}
}

int info_piece_can_go_down(Info* info)
{
	Piece* piece = &info->current_piece;

	for (int x = 0; x < SIZE_PIECE; x++)
	{
		for (int y = 3; y >= 0; y --)
		{
			if (info->pos_y + y < 0) continue;
			if (piece->config.patterns[piece->current_rotation][x][y] != 0)
			{
				if (info->pos_y + y + 1 >= info->height)
					return FALSE;
				if (info->grid[info->pos_x + x][info->pos_y + y + 1] >= 0)
					return FALSE;
				break;
			}
		}
	}

	return TRUE;
}

int info_piece_can_go_down_xy(Info* info, int pos_x, int pos_y)
{
	Piece* piece = &info->current_piece;

	for (int x = 0; x < SIZE_PIECE; x++)
	{
		for (int y = 3; y >= 0; y --)
		{
			if (pos_y + y < 0) continue;
			if (piece->config.patterns[piece->current_rotation][x][y] != 0)
			{
				if (pos_y + y + 1 >= info->height)
					return FALSE;
				if (info->grid[pos_x + x][pos_y + y + 1] >= 0)
					return FALSE;
				break;
			}
		}
	}

	return TRUE;
}

void info_piece_go_right(Info* info)
{
	if (info_piece_can_go_right(info))
	{
		info->pos_x = info->pos_x + 1;
		info_sound_play(info, BUFFER_LEFTRIGHT);
	}
	else
	{
		info_sound_play(info, BUFFER_TOUCH_LF);
	}
}

int info_piece_can_go_right(Info* info)
{
	Piece* piece = &info->current_piece;

	for (int y = 0; y < SIZE_PIECE; y++)
	{
		if (info->pos_y + y < 0) continue;
		for (int x = 3; x >= 0; x --)
		{
			if (piece->config.patterns[piece->current_rotation][x][y] != 0)
			{
				if (info->pos_x + x + 1 >= info->width)
					return FALSE;
				if (info->grid[info->pos_x + x + 1][info->pos_y + y ] >= 0)
					return FALSE;
				break;
			}
		}
	}

	return TRUE;
}

int info_piece_is_in_grid(Info* info)
{
	return (int_is_in(info->pos_x, info->width) && int_is_in(info->pos_y, info->height));
}

int info_piece_can_be_rotated(Info* info, int rotation)
{
	int i, j;
	for (i = 0; i < SIZE_PIECE; i++)
	{
		for (j = 0; j < SIZE_PIECE; j++)
		{
			if (info->pos_y + j < 0) continue;
			if (info->current_piece.config.patterns[rotation][i][j] != 0)
			{
				if (!int_is_in(info->pos_x + i, info->width))
					return FALSE;
				if (info->grid[info->pos_x + i][info->pos_y + j] >= 0)
					return FALSE;
			}
		}
	}

	return TRUE;
}

void info_piece_rotation(Info* info)
{
	int rotation = info->current_piece.current_rotation + 1;
	rotation = rotation % NB_ROTATIONS;
	if (!info_piece_can_be_rotated(info, rotation))
	{
		info_sound_play(info, BUFFER_ROTATE_FAIL);
		return;
	}
	info_sound_play(info, BUFFER_ROTATE);
	info->current_piece.current_rotation = rotation;
}

void info_add_piece_to_queue(Info* info)
{
	info->current_piece = info->next_pieces[0];
	info->next_pieces[0] = info->next_pieces[1];
	info->next_pieces[1] = info->next_pieces[2];
	info->next_pieces[2] = info->next_pieces[3];
	info->next_pieces[3] = new_piece(info->mode);
	info->pos_x = info->width / 2 - 2;
	info->pos_y = 0 - info_piece_get_bottom_block(info);

	if (!info_piece_can_be_added(info))
		info_on_gameover(info);

	if (info->mode == BLOCK_MODE)
	{
		if (rand() % 7 != 1) return;
		info->current_piece.color = COLOR_INDEX_BLACK_BLOCK;
		info->pos_x = rand() % (info->width - SIZE_PIECE);
		while (info_piece_can_go_down(info))
		{
			info_piece_go_down(info);
		}
		info_add_piece_to_grid(info);
	}
}

void info_pieces_switch(Info* info)
{
	if (info->pos_y > 1 || info->pos_x != info->width / 2 - 2) return;

	info_sound_play(info, BUFFER_SWITCH);

	Piece first_piece = info->current_piece;
	info->current_piece = info->next_pieces[0];
	info->next_pieces[0] = info->next_pieces[1];
	info->next_pieces[1] = info->next_pieces[2];
	info->next_pieces[2] = info->next_pieces[3];
	info->next_pieces[3] = first_piece;

	if (!info_piece_can_be_added(info))
		info_on_gameover(info);
}

void info_piece_next_step(Info* info)
{
	if (info->bonus->type != NO_BONUS) return;
	if (!keys_are_equals(info->key_Left, info->key_Right))
	{
		if (info->key_Left)
			info_piece_go_left(info);
		if (info->key_Right)
			info_piece_go_right(info);
	}

	if (!keys_are_equals(info->key_Down, info->key_Up))
	{
		if (info->key_Down)
		{
			if (info_piece_can_go_down(info))
			{
				info_piece_go_down(info);
				info_add_to_score(info, 2 * info->current_level);
				animatedText_setup(info->aText_down, 2 * info->current_level, 500, info->pos_x + SIZE_PIECE / 2 - 1,
				                   info->pos_y - 1);
			}
			else
			{
				info_add_piece_to_grid(info);
			}
		}
		if (info->key_Up)
		{
			info_piece_rotation(info);
			info->key_Up = 0;
		}
	}

	if (info->key_space)
	{
		info_pieces_switch(info);
		info->key_space = 0;
	}
}

void info_add_piece_to_grid(Info* info)
{
	info_sound_play(info, BUFFER_ADD_PIECE);
	int i, j;
	for (i = 0; i < SIZE_PIECE; i ++)
		for (j = 0; j < SIZE_PIECE; j ++)
			if (info->current_piece.config.patterns[info->current_piece.current_rotation][i][j] != 0)
			{
				if (info->pos_y + j < 0)
				{
					info_on_gameover(info);
					return;
				}
				info->grid[info->pos_x + i][info->pos_y + j] = info->current_piece.color;
			}

	int count = 0;
	int line = 0;
	for (int y = 0; y < info->height; y++)
	{
		int is_full = info_grid_line_is_full(info, y);
		if (is_full)
		{
			count ++;
			if (count == 1)
			{
				line = y;
				animatedDestroyedLines_reset_lines(info->aLines);
			}
			animatedDestroyedLines_add_line(info->aLines, y);
		}
	}

	if (count >= 1)
	{
		int text = 0;

		if (count == 1)
		{
			info_add_to_score(info, 40);
			text = 40;
		}
		if (count == 2)
		{
			info_add_to_score(info, 100);
			text = 100;
		}
		if (count == 3)
		{
			info_add_to_score(info, 300);
			text = 300;
		}
		if (count == 4)
		{
			info_add_to_score(info, 1200);
			text = 1200;
		}
		info_sound_play(info, BUFFER_CLEAR_LINE);
		animatedDestroyedLines_setup(info->aLines, 750, rand() % NBR_PIECES, FALSE);
		animatedDestroyedLines_set_ready(info->aLines);
		animatedText_setup(info->aText_line, text, 1000, info->width / 2 - 1, line - 2);
	}
	else
	{
		info_add_piece_to_queue(info);
	}
}

void info_on_gameover(Info* info)
{
	info_pause_music(info);
	animatedDestroyedLines_reset_lines(info->aLines);
	animatedDestroyedLines_add_line(info->aLines, 0);
	animatedDestroyedLines_setup(info->aLines, 1500, rand() % NBR_PIECES, TRUE);
	animatedDestroyedLines_set_ready(info->aLines);
	info_sound_play(info, BUFFER_GAMEOVER);
}


void info_set_level(Info* info, int level)
{
	info->current_level = level;
	info->new_value_timeout_down = TIMEOUT_FIRST_LEVEL - level * INTERVAL_BY_LEVEL;
	info->lines_left_to_next_level = 4 + level;
}


/*------------------------------ G R I L L E------------------------------------------*/

void info_init_grid(Info* info)
{
	int i, j;
	info->grid = malloc(sizeof(int *) * info->width);
	for (i = 0; i < info->width; i ++)
	{
		info->grid[i] = malloc(sizeof(int) * info->height);
	}

	for (i = 0; i < info->width; i ++)
	{
		for (j = 0; j < info->height; j++)
		{
			info->grid[i][j] = -1;
		}
	}
}

int info_grid_is_empty(Info* info)
{
	for (int i = 0; i < info->height; i++)
	{
		if (info_grid_line_is_empty(info, i) == FALSE) return FALSE;
	}

	return TRUE;
}

int info_grid_line_is_empty(Info* info, int line)
{
	int i;
	for (i = 0; i < info->width; i ++)
	{
		if (info->grid[i][line] >= 0)
			return FALSE;
	}
	return TRUE;
}

int info_grid_line_is_full(Info* info, int line)
{
	int i;
	for (i = 0; i < info->width; i ++)
	{
		if (info->grid[i][line] == -1)
			return FALSE;
	}
	return TRUE;
}

void info_grid_destroy_line(Info* info, int line)
{
	info->lines_left_to_next_level = info->lines_left_to_next_level - 1;
	if (info->lines_left_to_next_level <= 0) info_set_level(info, info->current_level + 1);
	int i, j;
	for (i = line; i > 0; i --)
	{
		for (j = 0; j < info->width; j ++)
		{
			info->grid[j][i] = info->grid[j][i - 1];
		}
	}

	for (i = 0; i < info->width; i ++)
	{
		info->grid[i][0] = -1;
	}
}

void info_grid_add_line_below(Info* info)
{
	if (info->mode != BELOW_MODE) return;
	if (rand() % 20 != 1) return;
	if (!info_piece_can_go_down(info)) return;

	for (int i = 0; i < info->height - 1; i++)
		for (int j = 0; j < info->width; j++)
			info->grid[j][i] = info->grid[j][i + 1];

	int last_line = info->height - 1;
	for (int i = 0; i < info->width; i++)
	{
		if (rand() % 2 == 0)
			info->grid[i][last_line] = COLOR_INDEX_BLACK_BLOCK;
		else
			info->grid[i][last_line] = -1;
	}
}

int info_grid_block_is_empty(Info* info, int x, int y)
{
	for (int i = 0; i < 2; i ++)
		for (int j = 0; j < 2; j++)
			if (info->grid[x + i][y + j] >= 0) return FALSE;

	return TRUE;
}

void info_grid_destroy_block(Info* info, int x, int y)
{
	for (int i = 0; i < 2; i ++)
		for (int j = 0; j < 2; j ++)
			info->grid[x + i][y + j] = -1;
}

void info_grid_color_line(Info* info, int line, int color)
{
	for (int i = 0; i < info->width; i++)
	{
		info->grid [i][line] = color;
	}
}

/*---------------------------------- D R A W ------------------------------------------*/

void cell_draw(GdkWindow* win, GdkGC* gc, int filled, int x, int y, int width, int height, int color, int skin,
               GdkPixbuf* img)
{
	if (img != NULL)
	{
		pixbuf_draw(win, gc, img, x, y);
		return;
	}

	int index_color = colors_block_index[skin] >= 0 ? colors_block_index[skin] : color;

	GdkPoint pt_up_left;
	pt_up_left.x = x;
	pt_up_left.y = y;

	GdkPoint pt_bottom_right;
	pt_bottom_right.x = x + width;
	pt_bottom_right.y = y + height;

	GdkPoint pt_up_right;
	pt_up_right.x = x + width;
	pt_up_right.y = y;

	GdkPoint pt_bottom_left;
	pt_bottom_left.x = x;
	pt_bottom_left.y = y + height;

	GdkPoint list_up_right[3];
	GdkPoint list_bottom_left[3];

	list_up_right[0] = pt_bottom_right;
	list_bottom_left[0] = pt_bottom_right;
	list_up_right[1] = pt_up_left;
	list_bottom_left[1] = pt_up_left;
	list_up_right[2] = pt_up_right;
	list_bottom_left[2] = pt_bottom_left;

	gc_set_rgb_fg(gc, colors_dark[index_color]);
	gdk_draw_polygon(win, gc, filled, list_up_right, 3);
	gc_set_rgb_fg(gc, colors_light[index_color]);
	gdk_draw_polygon(win, gc, filled, list_bottom_left, 3);

	if (width > 5 && height > 5)
	{
		gc_set_rgb_fg(gc, colors[index_color]);
		gdk_draw_rectangle(win, gc, TRUE, x + 3, y + 3, width - 6, height - 6);
	}

	gc_set_rgb_fg(gc, colors_line[skin]);
	gdk_draw_rectangle(win, gc, FALSE, x, y, width, height);
}

void info_shadow_draw(GdkWindow* win, GdkGC* gc, Info* info, int size_cell)
{
	if (info->current_level != 1) return;
	int x = info->pos_x;
	int y = info->pos_y;

	while (info_piece_can_go_down_xy(info, x, y))
	{
		y ++;
	}
	Piece* piece = &info->current_piece;
	gc_set_rgb_fg(gc, 0x909090);
	for (int i = 0; i < NB_PATTERN_COLS; i ++)
	{
		for (int j = 0; j < NB_PATTERN_ROWS; j++)
		{
			if (y + j < 0) continue;
			if (piece->config.patterns[piece->current_rotation][i][j] == 1)
			{
				gdk_draw_rectangle(win, gc, TRUE, GRID_X + (i + x) * size_cell, GRID_Y + (j + y) * size_cell, size_cell, size_cell);
			}
		}
	}
}

void info_current_piece_draw(GdkWindow* win, GdkGC* gc, Info* info, int size_cell)
{
	Piece* piece = &info->current_piece;
	piece_in_grid_draw(win, gc, piece, info->pos_x, info->pos_y, size_cell, info->skin, info->cell_img);
}

void info_grid_draw(GdkWindow* win, GdkGC* gc, Info* info, int size_cell)
{
	gc_set_rgb_fg(gc, colors_line[info->skin]);
	gdk_draw_rectangle(win, gc, FALSE, GRID_X - 1, GRID_Y - 1, size_cell * info->width + 2, size_cell * info->height + 2);
	gdk_draw_rectangle(win, gc, FALSE, GRID_X, GRID_Y, size_cell * info->width, size_cell * info->height);
	int i, j;
	for (i = 0; i < info->width; i ++)
	{
		for (j = 0; j < info->height; j++)
		{
			int is_filled = info->grid[i][j] >= 0;
			if (is_filled)
			{
				cell_draw(win, gc, TRUE, GRID_X + i * size_cell, GRID_Y + j * size_cell, size_cell, size_cell, info->grid[i][j],
				          info->skin, info->cell_img);
			}
		}
	}
}

void info_background_draw(GdkWindow* win, GdkGC* gc, Info* info, int width, int height)
{
	int nbr_step = 20;
	if (info->bg_img == NULL)
	{
		int color1 = colors_background [info->skin];
		int color2 = color1 + 0x606060 > 0xFFFFFF ? 0xFFFFFF : color1 + 0x606060;
		int sub_color = color2 - color1;

		int r = (sub_color & 0xFF0000) >> 8;
		int g = (sub_color & 0x00FF00);
		int b = (sub_color & 0x0000FF) << 8;

		int marge_r = r / nbr_step;
		int marge_g = g / nbr_step;
		int marge_b = b / nbr_step;


		for (int i = 0; i < nbr_step; i++)
		{
			int step = (marge_r * i & 0xFF00) << 8 |
				(marge_g * i & 0xFF00) |
				(marge_b * i & 0xFF00) >> 8;
			int tmp_color = color1 + step;
			gc_set_rgb_fg(gc, tmp_color);
			if (i == 0)
				gdk_draw_rectangle(win, gc, TRUE, 0, 0, width, height);
			else
				gdk_draw_rectangle(win, gc, TRUE, 0, 0, width, height / (i * 2));
		}
	}
	else
	{
		pixbuf_draw_with_size(win, gc, info->bg_img, 0, 0, width, height);
	}
}

void info_bonus_draw(GdkWindow* win, GdkGC* gc, Info* info, int size_cell)
{
	if (info->bonus->type == NO_BONUS) return;

	GameBonus* bonus = info->bonus;
	int x = bonus->x;
	int y = bonus->y;

	gc_set_rgb_fg(gc, 0xE10000);
	switch (info->bonus->type)
	{
	case (NO_BONUS): return;
	case (CELL_BONUS):
		gdk_draw_rectangle(win, gc, FALSE, GRID_X + x * size_cell, GRID_Y + y * size_cell, size_cell, size_cell);
		gdk_draw_rectangle(win, gc, FALSE, GRID_X + x * size_cell - 1, GRID_Y + y * size_cell - 1, size_cell + 2,
		                   size_cell + 2);
		return;
	case (BLOCK_BONUS):
		gdk_draw_rectangle(win, gc, FALSE, GRID_X + x * size_cell, GRID_Y + y * size_cell, size_cell * 2, size_cell * 2);
		gdk_draw_rectangle(win, gc, FALSE, GRID_X + x * size_cell - 1, GRID_Y + y * size_cell - 1, size_cell * 2 + 2,
		                   size_cell * 2 + 2);
		return;
	case (LINE_BONUS):
		gdk_draw_rectangle(win, gc, FALSE, GRID_X, GRID_Y + y * size_cell, size_cell * info->width, size_cell);
		gdk_draw_rectangle(win, gc, FALSE, GRID_X - 1, GRID_Y + y * size_cell - 1, size_cell * info->width + 2,
		                   size_cell + 2);
		return;
	default:
		return;
	}
}

/*--------------------------------- B E S T  S C O R E S --------------------------------*/

void info_add_to_score(Info* info, int score)
{
	info->current_score = info->current_score + score;
}

void bestScores_fill_with_file(BestScores* bestScores, char* filename)
{
	if (bestScores == NULL)
	{
		printf("BestScores NULL\n");
		return;
	}
	FILE* f = fopen(filename, "r");
	if (f == NULL)
	{
		printf("Problème lecture de fichier\n");
		return;
	}

	int pos = 0;

	int tmp_score = 0;
	char tmp_name[10];
	while (fscanf(f, "%s %d\n", &tmp_name, &tmp_score) == 2)
	{
		bestScores_add_item_at_index(bestScores, tmp_name, tmp_score, pos);
		pos ++;
	}

	fclose(f);
}

void bestScores_reset(BestScores* bestScores, char* filename)
{
	for (int i = 0; i < 5; i++)
	{
		bestScores->scores[i] = 0;
		strcpy(bestScores->names[i], "");
	}
	FILE* f = fopen(filename, "w+");
	if (f == NULL)
	{
		printf("Problème lecture de fichier\n");
		return;
	}
	fprintf(f, "");
	fclose(f);
}

void bestScores_write_file(BestScores* bestScores, char* filename)
{
	FILE* fichier = fopen(filename, "w+");
	if (fichier != NULL)
	{
		int pos = 0;
		while (bestScores->scores[pos] != 0 && pos < 5)
		{
			fprintf(fichier, "%s %d\n", bestScores->names[pos], bestScores->scores[pos]);
			pos ++;
		}
		fclose(fichier);
	}
}

void bestScores_init(BestScores* bestScores)
{
	bestScores->scores = malloc(sizeof (int) * 5);
	bestScores->names = malloc(sizeof (char *) * 5);
	for (int i = 0; i < 5; i ++)
	{
		bestScores->scores[i] = 0;
		bestScores->names[i] = malloc(sizeof (char) * 5);
		strcpy(bestScores->names[i], "");
	}
}

void bestScores_add_item_at_index(BestScores* bestScores, char* name, int score, int index)
{
	if (index >= 5) return;
	bestScores->scores[index] = score;
	strcpy(bestScores->names [index], name);
}

void bestScores_add_item(BestScores* bestScores, char* name, int score)
{
	int is_in_tab = FALSE;
	for (int i = 0; i < 5; i++)
	{
		if (score > bestScores->scores[i]) is_in_tab = TRUE;
	}

	if (!is_in_tab) return;

	bestScores->scores[4] = score;
	strcpy(bestScores->names [4], name);

	TriABulle(bestScores->scores, bestScores->names, 5);

	bestScores_write_file(bestScores, BS_FILENAME);
}

void bestScores_show(BestScores* bestScores, GtkWindow* win)
{
	GtkWidget* table;

	GtkWidget* dialog;
	GtkWidget* content_area;
	GtkDialogFlags flags = GTK_DIALOG_DESTROY_WITH_PARENT;

	dialog = gtk_dialog_new_with_buttons("BestScores",
	                                     win,
	                                     flags,
	                                     "Ok",
	                                     GTK_RESPONSE_ACCEPT,
	                                     "Reset",
	                                     GTK_RESPONSE_REJECT,
	                                     NULL);

	content_area = gtk_dialog_get_content_area(GTK_DIALOG(dialog));
	//gtk_container_set_border_width(GTK_CONTAINER(content_area), 10);

	table = bestScores_get_table(bestScores);
	gtk_container_add(GTK_CONTAINER(content_area), table);
	gtk_widget_show_all(dialog);

	gint result = gtk_dialog_run(GTK_DIALOG(dialog));

	switch (result)
	{
	case GTK_RESPONSE_ACCEPT:
		printf("OK was clicked\n");
		break;

	case GTK_RESPONSE_REJECT:
		printf("CANCEL was clicked\n");
		bestScores_reset(bestScores, BS_FILENAME);
		break;
	default:
		printf("Undefined. Perhaps dialog was closed\n");
		break;
	}

	gtk_widget_destroy(dialog);
}

/*----------------------------------- G T K W I D G E T --------------------------*/

GtkWidget* info_get_mode_combo(Info* info)
{
	GtkWidget* combo;

	combo = gtk_combo_box_text_new();

	for (int i = 0; i < NBR_MODES; i++)
	{
		gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(combo), modes_labels[i]);
	}
	gtk_combo_box_set_active(GTK_COMBO_BOX(combo), info->mode);

	return combo;
}

GtkWidget* info_get_skin_combo(Info* info)
{
	GtkWidget* combo;

	combo = gtk_combo_box_text_new();

	for (int i = 0; i < NBR_SKINS; i++)
	{
		gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(combo), skins_labels[i]);
	}
	gtk_combo_box_set_active(GTK_COMBO_BOX(combo), info->skin);

	return combo;
}

GtkWidget* info_get_level_combo(Info* info)
{
	GtkWidget* combo;

	combo = gtk_combo_box_text_new();

	for (int i = 0; i < NB_LEVEL; i++)
	{
		char text [10];
		sprintf(text, "%d", i + 1);
		gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(combo), text);
	}
	gtk_combo_box_set_active(GTK_COMBO_BOX(combo), info->current_level - 1);

	return combo;
}


GtkWidget* bestScores_get_table(BestScores* bestScores)
{
	GtkWidget* table;
	GtkWidget* label_score;
	GtkWidget* label_name;

	table = gtk_table_new(5, 2, TRUE);
	gtk_table_set_row_spacings(GTK_TABLE(table), 2);
	gtk_table_set_col_spacings(GTK_TABLE(table), 2);

	int i = 0;

	for (i = 0; i < 5; i++)
	{
		char tmp [10];
		sprintf(tmp, "%5d", bestScores->scores[i]);
		printf("%s\n", bestScores->names[i]);
		label_name = gtk_label_new(bestScores->names[i]);
		label_score = gtk_label_new(tmp);
		gtk_table_attach_defaults(GTK_TABLE(table), label_name, 0, 1, i, i + 1);
		gtk_table_attach_defaults(GTK_TABLE(table), label_score, 1, 2, i, i + 1);
	}

	return table;
}

/*------------------------------ G A M E B O N U S -------------------------*/

void gameBonus_init(GameBonus* bonus)
{
	bonus->type = NO_BONUS;
	bonus->x = 0;
	bonus->y = 0;
}

void gameBonus_set_type(GameBonus* bonus, TypeBonus type)
{
	if (bonus->type == type) bonus->type = NO_BONUS;
	else bonus->type = type;
}

void gameBonus_go_left(GameBonus* bonus)
{
	if (bonus->x - 1 >= 0) bonus->x = bonus->x - 1;
}

void gameBonus_go_right(GameBonus* bonus, int width)
{
	int size_bonus = bonus->type != BLOCK_BONUS ? 1 : 2;
	if (bonus->x + size_bonus < width) bonus->x = bonus->x + 1;
}

void gameBonus_go_up(GameBonus* bonus)
{
	if (bonus->y - 1 >= 0) bonus->y = bonus->y - 1;
}

void gameBonus_go_down(GameBonus* bonus, int height)
{
	int size_bonus = bonus->type != BLOCK_BONUS ? 1 : 2;
	if (bonus->y + size_bonus < height) bonus->y = bonus->y + 1;
}

void info_validate_gameBonus(Info* info)
{
	info_sound_play(info, BUFFER_BONUS);
	int x = info->bonus->x;
	int y = info->bonus->y;

	switch (info->bonus->type)
	{
	case (NO_BONUS): return;
	case (CELL_BONUS):
		if (info->grid[x][y] == -1) break;
		info->grid[x][y] = -1;
		info->current_score = info->current_score - COST_CELL_BONUS;
		break;
	case (BLOCK_BONUS):
		if (info_grid_block_is_empty(info, x, y)) break;
		info_grid_destroy_block(info, x, y);
		info->current_score = info->current_score - COST_BLOCK_BONUS;
		break;
	case (LINE_BONUS):
		if (info_grid_line_is_empty(info, y)) break;
		info_grid_destroy_line(info, y);
		info->current_score = info->current_score - COST_LINE_BONUS;
		break;
	default:
		printf("Nothing passed for bonus\n");
		break;
	}

	info->bonus->type = NO_BONUS;
}

void info_gameBonus_next_step(Info* info)
{
	if (info->bonus->type == NO_BONUS) return;
	GameBonus* bonus = info->bonus;
	if (!keys_are_equals(info->key_Left, info->key_Right))
	{
		if (info->key_Left)
			gameBonus_go_left(bonus);
		if (info->key_Right)
			gameBonus_go_right(bonus, info->width);
	}

	if (!keys_are_equals(info->key_Down, info->key_Up))
	{
		if (info->key_Down)
		{
			gameBonus_go_down(bonus, info->height);
		}
		if (info->key_Up)
		{
			gameBonus_go_up(bonus);
		}
	}

	if (info->key_space)
	{
		info_validate_gameBonus(info);
		info->key_space = 0;
	}
}

/*------------------------------ P I X ------------------------------------------*/
void pix_load_all(Info* info)
{
	char path [100];
	sprintf(path, "%sbackground.png", skin_ressources_folder [info->skin]);
	info->bg_img = pixbuf_load(path);

	sprintf(path, "%scell.png", skin_ressources_folder [info->skin]);
	info->cell_img = pixbuf_load(path);

	if (info->bg_img != NULL)
	{
		/*int w, h;
		pixbuf_get_size (info->bg_img, &w, &h);
		gtk_widget_set_size_request(*widget, w, h);*/
	}

	if (info->cell_img != NULL)
	{
		int w, h;
		pixbuf_get_size(info->cell_img, &w, &h);
		/*gtk_widget_set_size_request(*widget, w, h);*/
		if (w != h)
			info->cell_img = NULL;
		else if (w != SIZE_CELL)
		{
			GdkPixbuf* img_scaled = gdk_pixbuf_scale_simple(info->cell_img, SIZE_CELL, SIZE_CELL, GDK_INTERP_BILINEAR);
			info->cell_img = img_scaled;
		}
	}
}

/*---------------------------- S O U N D -----------------------*/

void soundBuffers_load_all(Info* info)
{
	char tmp[100];
	sprintf(tmp, "%sSFX_PieceMoveLR.ogg", SOUND_FOLDER);
	info->soundBuffers [BUFFER_LEFTRIGHT] = sfSoundBuffer_createFromFile(tmp);

	sprintf(tmp, "%sSFX_PieceFall.ogg", SOUND_FOLDER);
	info->soundBuffers [BUFFER_DOWN] = sfSoundBuffer_createFromFile(tmp);

	sprintf(tmp, "%sSFX_PieceLockdown.ogg", SOUND_FOLDER);
	info->soundBuffers [BUFFER_ADD_PIECE] = sfSoundBuffer_createFromFile(tmp);

	sprintf(tmp, "%sSFX_PieceRotateLR.ogg", SOUND_FOLDER);
	info->soundBuffers [BUFFER_ROTATE] = sfSoundBuffer_createFromFile(tmp);

	sprintf(tmp, "%sSFX_PieceRotateFail.ogg", SOUND_FOLDER);
	info->soundBuffers [BUFFER_ROTATE_FAIL] = sfSoundBuffer_createFromFile(tmp);

	sprintf(tmp, "%sSFX_SpecialLineClearTriple.ogg", SOUND_FOLDER);
	info->soundBuffers [BUFFER_CLEAR_LINE] = sfSoundBuffer_createFromFile(tmp);

	sprintf(tmp, "%sSFX_PieceTouchLR.ogg", SOUND_FOLDER);
	info->soundBuffers [BUFFER_TOUCH_LF] = sfSoundBuffer_createFromFile(tmp);

	sprintf(tmp, "%sSFX_PieceHold.ogg", SOUND_FOLDER);
	info->soundBuffers [BUFFER_SWITCH] = sfSoundBuffer_createFromFile(tmp);

	sprintf(tmp, "%sSFX_SpecialLineClearSingle.ogg", SOUND_FOLDER);
	info->soundBuffers [BUFFER_BONUS] = sfSoundBuffer_createFromFile(tmp);

	sprintf(tmp, "%sSFX_GameOver.wav", SOUND_FOLDER);
	info->soundBuffers [BUFFER_GAMEOVER] = sfSoundBuffer_createFromFile(tmp);
}

void info_soundBuffers_destroy(Info* info)
{
	for (int i = 0; i < NBR_BUFFERS; i++)
	{
		sfSoundBuffer_Destroy(info->soundBuffers[i]);
	}
}

void soundStack_destroy(SoundStack* stack)
{
	for (int i = 0; i < stack->nbr_sounds; i ++)
	{
		sfSound_destroy(stack->list_sounds[i]);
	}
	free(stack);
}

void soundStack_init(SoundStack* stack, int n)
{
	for (int i = 0; i < n; i ++)
	{
		stack->list_sounds[i] = sfSound_create();
	}
	stack->nbr_sounds = n;
	stack->sound_to_load = 0;
}

void info_sound_play(Info* info, int index)
{
	SoundStack* stack = info->soundStack;
	int i = stack->sound_to_load;
	sfSound_setBuffer(stack->list_sounds[i], info->soundBuffers[index]);
	sfSound_play(stack->list_sounds[i]);
	stack->sound_to_load = (i + 1) % stack->nbr_sounds;
}

void info_init_music(Info* info)
{
	info->music = sfMusic_createFromFile("music.ogg");
	if (info->music != NULL)
	{
		sfMusic_setVolume(info->music, 30);
		sfMusic_setLoop(info->music, TRUE);
	}
}

void info_play_music(Info* info)
{
	if (info->music == NULL) return;
	sfMusic_play(info->music);
}

void info_pause_music(Info* info)
{
	if (info->music == NULL) return;
	sfMusic_pause(info->music);
}

/*---------------------------------- S A V E ------------------------------------*/

void info_save_in_file(Info* info, char* filename)
{
	remove(filename);
	if (info_grid_is_empty(info)) return;

	FILE* fichier = fopen(filename, "w+");
	if (fichier == NULL) return;

	fprintf(fichier, "%d\n", info->width);
	fprintf(fichier, "%d\n", info->height);

	for (int i = 0; i < info->height; i++)
	{
		for (int j = 0; j < info->width; j++)
		{
			fprintf(fichier, "%d ", info->grid[j][i]);
		}
		fprintf(fichier, "\n");
	}
	fprintf(fichier, "%d\n", info->current_score);
	fprintf(fichier, "%d\n", info->time);
	fprintf(fichier, "%d\n", info->current_level);
	fprintf(fichier, "%d\n", info->lines_left_to_next_level);
	fprintf(fichier, "%d\n", info->mode);
	fprintf(fichier, "%d\n", info->skin);

	fprintf(fichier, "%d %d\n", info->pos_x, info->pos_y);
	fprintf(fichier, "%d %d\n", info->current_piece.color, info->current_piece.current_rotation);

	for (int i = 0; i < 4; i ++)
		fprintf(fichier, "%d\n", info->next_pieces[i].color);

	fclose(fichier);
}

void info_fill_with_file(Info* info, char* filename)
{
	FILE* fichier = fopen(filename, "r");
	if (fichier == NULL)
	{
		printf("Probleme de lecture de la sauvegarde\n");
		return;
	}

	fscanf(fichier, "%d\n", &info->width);
	fscanf(fichier, "%d\n", &info->height);

	for (int i = 0; i < info->height; i++)
	{
		for (int j = 0; j < info->width; j++)
		{
			fscanf(fichier, "%d ", &info->grid[j][i]);
		}
		fscanf(fichier, "\n");
	}
	fscanf(fichier, "%d\n", &info->current_score);
	fscanf(fichier, "%d\n", &info->time);
	int level;
	fscanf(fichier, "%d\n", &level);
	info_set_level(info, level);
	fscanf(fichier, "%d\n", &info->lines_left_to_next_level);
	fscanf(fichier, "%d\n", &info->mode);
	fscanf(fichier, "%d\n", &info->skin);

	fscanf(fichier, "%d %d\n", &info->pos_x, &info->pos_y);

	int type = 0, rotation = 0;

	fscanf(fichier, "%d %d\n", &type, &rotation);
	info->current_piece = new_piece_with_type(type);
	info->current_piece.current_rotation = rotation;

	for (int i = 0; i < 4; i ++)
	{
		fscanf(fichier, "%d\n", &type);
		info->next_pieces[i] = new_piece_with_type(type);
	}

	fclose(fichier);
	pix_load_all(info);
}
