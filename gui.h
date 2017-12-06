/* gui.h - 24/02/2012 */

#ifndef GUI__H
#define GUI__H


#include <gtk/gtk.h>
#include "info.h"
#include "util.h"

#define CONTROL_TEXT "Controls :\n  Up : rotate the piece\n  Down : accelerate the piece\n  Left : goes to left\n  Right : goes to right\n  Space : switch pieces/validate choice"

typedef struct Menu {
	GtkWidget * bar;
	GtkWidget * file_item, * file;
	GtkWidget * new_item, * quit_item;

	GtkWidget * options_item, * options;
	GtkWidget * skin_item, * mode_item, * level_item;

	GtkWidget * help_item, * help;
	GtkWidget * controls_item, *bestScores_item, * about_item;
} Menu;

typedef struct {
    GtkWidget *main_box, *top_box, *central_box, *info_box, *tool_box;
    GtkWidget *score_frame, *time_frame, *level_frame, *next_pieces_frame;
    GtkWidget *score_label, *time_label, *level_label;
} Layout;

typedef struct {
    guint magic;
    Info *info;
    Layout layout;
    Menu menu;
    GtkWidget *window, *area, *statusbar, *area_pieces;
    GtkWidget *start_button;
    BestScores * bestScores;
} Gui;

void gui_init_window    (Gui *g);
void gui_init_layout    (Gui *g);
void gui_init_tool_box   (Gui *g);
void gui_init_area      (Gui *g);
void gui_init_info_box  (Gui *g);
void gui_init_statusbar (Gui *g);
void gui_init_area_pieces (Gui *g);

void gui_init_menu_bar (Gui * g);
void gui_init_options_menu (Gui * g);
void gui_init_help_menu (Gui * g);
void gui_init_file_menu (Gui * g);


void gui_anim_stop (Gui * g);
void gui_init (Gui *g, Info *info, BestScores * bestScores);
Gui *gui_check (gpointer data);

void reafficher (GtkWidget *win);

void show_entry_dialog(BestScores * bestScores, int score);
void gui_config_window (Gui * g);

#endif /* GUI__H */

