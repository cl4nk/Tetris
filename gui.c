/* gui.c - 24/02/2012 */

#include "gui.h"
#include <gdk/gdkkeysyms.h>  /* pour keyvals tq GDK_q */
#include <assert.h>

/*------------------------------ I N I T ------------------------------------*/

#define GUI_MAGIC 0x46EA7E05

void gui_init (Gui *g, Info *info, BestScores * bestScores)
{
    printf ("gui_init ...\n");
    g->magic = GUI_MAGIC;
    g->info = info;
    g->bestScores = bestScores;
    gui_init_window (g);
    gui_init_layout (g);
    gui_init_menu_bar (g);
	gui_init_options_menu (g);
	gui_init_help_menu (g);
	gui_init_file_menu (g);
    gui_init_area   (g);
    gui_init_info_box (g);
    gui_init_tool_box (g);
    gui_init_statusbar (g);
    gui_init_area_pieces (g);
}


/* Cette fonction permet de tester si le data que l'on a recupere' dans
 * un handler de signaux contient bien un gui ; sinon, cela veut dire que :
 *  - soit on a oublie' de transmettre gui dans g_signal_connect,
 *  - soit on s'est trompe' dans le nombre de parametres du handler de signaux.
*/

Gui *gui_check (gpointer data)
{
    if (data == NULL) { 
        fprintf (stderr, "gui_check: NULL data\n"); return NULL; 
    }
    if (((Gui *)data)->magic != GUI_MAGIC) { 
        fprintf (stderr, "gui_check: bad magic number\n"); return NULL; 
    }
    return data;
}


/*---------------------------- W I N D O W S --------------------------------*/

#define WIN1_TITLE  "Tetris"
#define WIN1_W   520
#define WIN1_H   520


void on_window_destroy (GtkWidget U *widget, gpointer U data)
{
    Gui *g = gui_check (data); 
    Info * info = g->info;

    info_save_in_file (info, SAVE_FILENAME);
    printf ("on_window_destroy\n");
    gtk_main_quit ();
}

gboolean on_window_delete_event (GtkWidget U *widget, GdkEvent U *event, 
    gpointer U data)
{
    /* Gui *g = gui_check (data); */

    printf ("on_window_delete_event\n");
    return FALSE;
}


void gui_init_window (Gui *g)
{
    g->window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title (GTK_WINDOW (g->window), WIN1_TITLE);
    //gtk_window_set_default_size (GTK_WINDOW (g->window), WIN1_W, WIN1_H);
    gtk_window_set_position(GTK_WINDOW(g->window), GTK_WIN_POS_CENTER);
    gtk_window_set_resizable (GTK_WINDOW(g->window),
                          FALSE);
    

    ON_SIGNAL (g->window, "delete-event", on_window_delete_event, g);
    ON_SIGNAL (g->window, "destroy",      on_window_destroy,      g);
}


/*----------------------------- L A Y O U T ---------------------------------*/


void gui_init_layout (Gui *g)
{
    Layout *l = &g->layout;

    l->main_box = gtk_vbox_new (!HOMOGENEOUS, 0);
    l->top_box  = gtk_hbox_new (!HOMOGENEOUS, 0);
    l->central_box 	= gtk_hbox_new (FALSE, 0);

    gtk_container_add (GTK_CONTAINER (g->window), l->main_box);
    
    gtk_box_pack_start (GTK_BOX (l->main_box), l->top_box, !EXPAND, !FILL, 0);
    gtk_box_pack_start(GTK_BOX (l->main_box), l->central_box, TRUE, TRUE, 0);
}


/*-------------------------------I N F O B O X ------------------------------*/

void gui_init_info_box (Gui *g)
{
    Layout *l = &g->layout;

    l->info_box = gtk_vbox_new (!HOMOGENEOUS, 0);
    //gtk_widget_set_size_request(l->info_box, 240, -1);
    l->score_frame 	= frame_new( "Score", TRUE );
    l->level_frame 	= frame_new( "Level", TRUE );
    l->time_frame 	= frame_new( "Time", TRUE );
    l->tool_box = gtk_hbox_new (!HOMOGENEOUS, 0);
    
    l->score_label = gtk_label_new ("0");
    l->level_label = gtk_label_new ("0");
    l->time_label = gtk_label_new ("00:00");

    
    gtk_container_add(GTK_CONTAINER(l->score_frame), l->score_label);
    gtk_container_add(GTK_CONTAINER(l->level_frame), l->level_label);
	gtk_container_add(GTK_CONTAINER(l->time_frame), l->time_label);
    
    gtk_box_pack_start(GTK_BOX(l->info_box), l->score_frame, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(l->info_box), l->level_frame, FALSE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(l->info_box), l->time_frame, FALSE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(l->info_box), l->tool_box, FALSE, TRUE, 0);
    
    gtk_box_pack_start(GTK_BOX(l->central_box), l->info_box, FALSE, TRUE, 0);
    
    
}

/*-------------------------------- T O P ------------------------------------*/
void gui_anim_start (Gui * g) {
	gtk_button_set_label(GTK_BUTTON(g->start_button), "Stop");
	g->info->anim = 1;
	info_anim_start(g->info, g);
	statusbar_put (GTK_STATUSBAR(g->statusbar), "Press Stop to pause the game!");
}

void gui_anim_stop (Gui * g) {
	gtk_button_set_label(GTK_BUTTON(g->start_button), "Start");
	g->info->anim = 0;
	info_anim_stop(g->info);
	statusbar_put (GTK_STATUSBAR(g->statusbar), "Press Start to play!");
}

void on_quit_clicked (GtkButton U *widget, gpointer U data)
{
    Gui *g = gui_check (data); 
    Info * info = g->info;

    info_save_in_file (info, SAVE_FILENAME);

    gtk_main_quit ();
}

void on_new_clicked (GtkButton U *widget, gpointer U data)
{
	Gui *g = gui_check (data);

	gui_anim_stop (g);

	int mode = g->info->mode;
	int skin = g->info->skin;

	Layout *l = &g->layout;
	label_set_text (GTK_LABEL (l->score_label), "0");
    label_set_text (GTK_LABEL (l->level_label), "0");
    label_set_text (GTK_LABEL (l->time_label), "00:00");

    animatedText_init(g->info->aText_down);
    animatedText_init(g->info->aText_line);
    animatedDestroyedLines_init(g->info->aLines);
    gameBonus_init (g->info->bonus);

	info_init_default (g->info, g->bestScores);

	g->info->skin = skin;
	g->info->mode = mode;

	statusbar_put (GTK_STATUSBAR (g->statusbar), "Press Start to play !");
	reafficher (g->window);
	gui_config_window (g);
}

void on_score_clicked (GtkButton U *widget, gpointer U data)
{
	Gui *g = gui_check (data);

	gui_anim_stop (g);
	bestScores_show (g->bestScores, GTK_WINDOW (g->window));
}

void statusbar_not_enough (GtkStatusbar * bar, int cost) {
	char tmp[100];
	sprintf (tmp, "Not enough points! Needs %d to use that!", cost);
	statusbar_put(GTK_STATUSBAR(bar), tmp);
}

void on_cell_bonus_clicked (GtkButton U *widget, gpointer U data)
{
	Gui *g = gui_check (data);
	gtk_widget_grab_focus(g->area);
	if (g->info->current_score < COST_CELL_BONUS) {
		statusbar_not_enough (GTK_STATUSBAR (g->statusbar), COST_CELL_BONUS);
		return;
	}
	gameBonus_set_type (g->info->bonus, CELL_BONUS);
}

void on_block_bonus_clicked (GtkButton U *widget, gpointer U data)
{
	Gui *g = gui_check (data);
	gtk_widget_grab_focus(g->area);
	if (g->info->current_score < COST_BLOCK_BONUS) {
		statusbar_not_enough (GTK_STATUSBAR (g->statusbar), COST_BLOCK_BONUS);
		return;
	}
	gameBonus_set_type (g->info->bonus, BLOCK_BONUS);
}

void on_line_bonus_clicked (GtkButton U *widget, gpointer U data)
{
	Gui *g = gui_check (data);
	gtk_widget_grab_focus(g->area);
	if (g->info->current_score < COST_LINE_BONUS) {
		statusbar_not_enough (GTK_STATUSBAR (g->statusbar), COST_LINE_BONUS);
		return;
	}
	gameBonus_set_type (g->info->bonus, LINE_BONUS);
}

void on_none_bonus_clicked (GtkButton U *widget, gpointer U data)
{
	Gui *g = gui_check (data);
	gtk_widget_grab_focus(g->area);
	gameBonus_set_type (g->info->bonus, NO_BONUS);
}

void on_start_clicked (GtkButton U *widget, gpointer U data)
{
    Gui *g = gui_check (data); 
    gtk_widget_grab_focus(g->area);
    
	Info *info = g->info;
	info->anim = !info->anim;
	gameBonus_set_type (g->info->bonus, NO_BONUS);
	if (info->anim) {
		gtk_button_set_label(widget, "Stop");
		info_anim_start(info, data);
		statusbar_put (GTK_STATUSBAR(g->statusbar), "Press Stop to pause the game!");
	}
	else {
		gtk_button_set_label(widget, "Start");
		info_anim_stop(info);
		statusbar_put (GTK_STATUSBAR(g->statusbar), "Press Start to play!");
	}
}

void on_about_clicked (GtkButton U *widget, gpointer U data)
{

	Gui *g = gui_check (data); 
	gui_anim_stop (g);

	GtkWidget *dialog = gtk_about_dialog_new();
	gtk_about_dialog_set_name(GTK_ABOUT_DIALOG(dialog), "Tetris");
	gtk_about_dialog_set_version(GTK_ABOUT_DIALOG(dialog), "42"); 
	gtk_about_dialog_set_copyright(GTK_ABOUT_DIALOG(dialog), 
	  "Nicolas Fontes");
	gtk_about_dialog_set_comments(GTK_ABOUT_DIALOG(dialog), 
	 "Projet Tetris pour l'année L2 Info.");
	gtk_about_dialog_set_website(GTK_ABOUT_DIALOG(dialog), 
	  "http://www.likeaboss.com");
	gtk_dialog_run(GTK_DIALOG (dialog));
	gtk_widget_destroy(dialog);
}

void on_mode_clicked (GtkButton U *widget, gpointer U data)
{
    Gui *g = gui_check (data);
    int mContinue = g->info->anim;
    gui_anim_stop (g);

    GtkWidget *dialog;
    GtkWidget *combo;
    GtkWidget *content_area;
    GtkDialogFlags flags = GTK_DIALOG_DESTROY_WITH_PARENT;

    dialog = gtk_dialog_new_with_buttons ("Choose a mode",
                                      GTK_WINDOW (g->window),
                                      flags,
                                      "Ok",
                                      GTK_RESPONSE_ACCEPT,
                                      "Cancel",
                                      GTK_RESPONSE_REJECT,
                                      NULL);

    content_area = gtk_dialog_get_content_area(GTK_DIALOG(dialog));
    combo = info_get_mode_combo (g->info);

    gtk_container_add(GTK_CONTAINER(content_area), combo);

    gtk_widget_show_all(dialog);
    gtk_widget_grab_focus(combo);
    gint result = gtk_dialog_run(GTK_DIALOG(dialog));

    switch(result)
    {
    case GTK_RESPONSE_ACCEPT:
        printf("OK was clicked\n");
        int mode_selected = gtk_combo_box_get_active (GTK_COMBO_BOX(combo));
        if (mode_selected != g->info->mode) {
            g->info->mode = mode_selected;
        }
        
        break;

    case GTK_RESPONSE_REJECT:
        printf("CANCEL was clicked\n"); 
        break;
    default:
        printf("Undefined. Perhaps dialog was closed\n"); 
        break;
    }
    gtk_widget_destroy(dialog);
    if (mContinue)
        gui_anim_start (g);
}

void on_skin_clicked (GtkButton U *widget, gpointer U data)
{
    Gui *g = gui_check (data);
    int mContinue = g->info->anim;
    gui_anim_stop (g);

    GtkWidget *dialog;
    GtkWidget *combo;
    GtkWidget *content_area;
    GtkDialogFlags flags = GTK_DIALOG_DESTROY_WITH_PARENT;

    dialog = gtk_dialog_new_with_buttons ("Choose a skin",
                                      GTK_WINDOW(g->window),
                                      flags,
                                      "Ok",
                                      GTK_RESPONSE_ACCEPT,
                                      "Cancel",
                                      GTK_RESPONSE_REJECT,
                                      NULL);

    content_area = gtk_dialog_get_content_area(GTK_DIALOG(dialog));
    combo = info_get_skin_combo (g->info);

    gtk_container_add(GTK_CONTAINER(content_area), combo);

    gtk_widget_show_all(dialog);
    gtk_widget_grab_focus(combo);
    gint result = gtk_dialog_run(GTK_DIALOG(dialog));

    switch(result)
    {
    case GTK_RESPONSE_ACCEPT:
        printf("OK was clicked\n");
        int skin_selected = gtk_combo_box_get_active (GTK_COMBO_BOX(combo));
        if (skin_selected != g->info->skin) {
            g->info->skin = skin_selected;
            pix_load_all(g->info);
            reafficher (g->window);
        }
        break;

    case GTK_RESPONSE_REJECT:
        printf("CANCEL was clicked\n"); 
        break;
    default:
        printf("Undefined. Perhaps dialog was closed\n"); 
        break;
    }

    gtk_widget_destroy(dialog);
    if (mContinue)
    	gui_anim_start (g);
}

void on_select_level_clicked (GtkButton U *widget, gpointer U data)
{
    Gui *g = gui_check (data);
    int mContinue = g->info->anim;
    gui_anim_stop (g);

    GtkWidget *dialog;
    GtkWidget *combo;
    GtkWidget *content_area;
    GtkDialogFlags flags = GTK_DIALOG_DESTROY_WITH_PARENT;

    dialog = gtk_dialog_new_with_buttons ("Choose level",
                                      GTK_WINDOW(g->window),
                                      flags,
                                      "Ok",
                                      GTK_RESPONSE_ACCEPT,
                                      "Cancel",
                                      GTK_RESPONSE_REJECT,
                                      NULL);

    content_area = gtk_dialog_get_content_area(GTK_DIALOG(dialog));
    combo = info_get_level_combo (g->info);

    gtk_container_add(GTK_CONTAINER(content_area), combo);

    gtk_widget_show_all(dialog);
    gtk_widget_grab_focus(combo);
    gint result = gtk_dialog_run(GTK_DIALOG(dialog));

    switch(result)
    {
    case GTK_RESPONSE_ACCEPT:
        printf("OK was clicked\n");
        int level_selected = gtk_combo_box_get_active (GTK_COMBO_BOX(combo));
        if (level_selected + 1 != g->info->current_level)
            info_set_level(g->info, level_selected + 1);
        break;

    case GTK_RESPONSE_REJECT:
        printf("CANCEL was clicked\n"); 
        break;
    default:
        printf("Undefined. Perhaps dialog was closed\n"); 
        break;
    }

    gtk_widget_destroy(dialog);
    if (mContinue)
    	gui_anim_start (g);
}

void show_entry_dialog(BestScores * bestScores, int score)
{
	assert(bestScores != NULL);
    GtkWidget *dialog;
    GtkWidget *entry;
    GtkWidget *content_area;
    GtkDialogFlags flags = GTK_DIALOG_DESTROY_WITH_PARENT;

    dialog = gtk_dialog_new_with_buttons ("Enter your name",
                                      NULL,
                                      flags,
                                      "Ok",
                                      GTK_RESPONSE_ACCEPT,
                                      "Cancel",
                                      GTK_RESPONSE_REJECT,
                                      NULL);
    gtk_dialog_set_default_response (GTK_DIALOG(dialog),
                                 GTK_RESPONSE_ACCEPT);

    content_area = gtk_dialog_get_content_area(GTK_DIALOG(dialog));
    entry = gtk_entry_new();
    gtk_entry_set_activates_default (GTK_ENTRY (entry),
                                 TRUE);
    gtk_container_add(GTK_CONTAINER(content_area), entry);


    gtk_widget_show_all(dialog);
    gtk_widget_grab_focus(entry);
    gint result = gtk_dialog_run(GTK_DIALOG(dialog));

    switch(result)
    {
    case GTK_RESPONSE_ACCEPT:
        printf("OK was clicked\n");
        static char entry_line[20] = "";
        strcpy(entry_line, gtk_entry_get_text(GTK_ENTRY(entry)));
		printf ("%s\n", entry_line);
        if(gtk_entry_get_text_length (GTK_ENTRY(entry)) == 0)
        {
            printf("Line is to short. Nothing to passing\n");
            printf("%d\n", gtk_entry_get_text_length (GTK_ENTRY(entry)));
            break;
        }

        bestScores_add_item(bestScores, entry_line, score);
        gtk_widget_destroy(dialog);
        bestScores_show(bestScores, NULL);
        break;

    case GTK_RESPONSE_REJECT:
        printf("CANCEL was clicked\n"); 
        gtk_widget_destroy(dialog);
        break;
    default:
        printf("Undefined. Perhaps dialog was closed\n"); 
        gtk_widget_destroy(dialog);
        break;
    }

    
}

void on_help_clicked (GtkButton U *widget, gpointer U data)
{
    Gui *g = gui_check (data); 
    gui_anim_stop (g);

    GtkWidget *dialog;
    dialog = gtk_message_dialog_new (GTK_WINDOW(g->window),
            GTK_DIALOG_DESTROY_WITH_PARENT,
            GTK_MESSAGE_INFO,
            GTK_BUTTONS_OK,
            CONTROL_TEXT);
    gtk_window_set_title(GTK_WINDOW(dialog), "Help");
    gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);
}

/*------------------------------- M E N U -----------------------------------*/

void gui_init_file_menu (Gui * g) {
	Menu * m = &g->menu;
	
	m->file = gtk_menu_new();
	m->new_item = gtk_menu_item_new_with_label("New");
	m->quit_item = gtk_menu_item_new_with_label("Quit");
	
	
	g_signal_connect (G_OBJECT (m->new_item), "activate",
                               G_CALLBACK (on_new_clicked),
                               g);
		
	g_signal_connect (G_OBJECT (m->quit_item), "activate",
                               G_CALLBACK (on_quit_clicked),
                               g);
	
	gtk_menu_shell_append(GTK_MENU_SHELL(m->file), m->new_item);
	gtk_menu_shell_append(GTK_MENU_SHELL(m->file), m->quit_item);
	
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(m->file_item), m->file);
}

void gui_init_help_menu (Gui * g) {
	Menu * m = &g->menu;
	
	m->help = gtk_menu_new();
	m->controls_item = gtk_menu_item_new_with_label("Controls");
	m->bestScores_item = gtk_menu_item_new_with_label("BestScores");
	m->about_item = gtk_menu_item_new_with_label("About");
	
	
	g_signal_connect (G_OBJECT (m->controls_item), "activate",
                               G_CALLBACK (on_help_clicked),
                               g);
		
	g_signal_connect (G_OBJECT (m->bestScores_item), "activate",
                               G_CALLBACK (on_score_clicked),
                               g);

   g_signal_connect (G_OBJECT (m->about_item), "activate",
							   G_CALLBACK (on_about_clicked),
							   g);
	
	gtk_menu_shell_append(GTK_MENU_SHELL(m->help), m->controls_item);
	gtk_menu_shell_append(GTK_MENU_SHELL(m->help), m->bestScores_item);
	gtk_menu_shell_append(GTK_MENU_SHELL(m->help), m->about_item);
	
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(m->help_item), m->help);
}

void gui_init_options_menu (Gui * g) {
	Menu * m = &g->menu;
	
	m->options = gtk_menu_new();
	m->skin_item = gtk_menu_item_new_with_label("Skin");
	m->mode_item = gtk_menu_item_new_with_label("Mode");
	m->level_item = gtk_menu_item_new_with_label("Level");
	
	
	g_signal_connect (G_OBJECT (m->skin_item), "activate",
                               G_CALLBACK (on_skin_clicked),
                               g);
		
	g_signal_connect (G_OBJECT (m->mode_item), "activate",
                               G_CALLBACK (on_mode_clicked),
                               g);

    g_signal_connect (G_OBJECT (m->level_item), "activate",
							   G_CALLBACK (on_select_level_clicked),
							   g);
	
	gtk_menu_shell_append(GTK_MENU_SHELL(m->options), m->skin_item);
	gtk_menu_shell_append(GTK_MENU_SHELL(m->options), m->mode_item);
	gtk_menu_shell_append(GTK_MENU_SHELL(m->options), m->level_item);
	
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(m->options_item), m->options);
}

void gui_init_menu_bar (Gui * g) {

	Layout * l 	= &g->layout;
	Menu * m 	= &g->menu;
	
	m->bar = gtk_menu_bar_new();
	m->file_item = gtk_menu_item_new_with_label ("File");
	gtk_menu_shell_append(GTK_MENU_SHELL(m->bar), m->file_item);

	m->options_item = gtk_menu_item_new_with_label ("Options");
	gtk_menu_shell_append(GTK_MENU_SHELL(m->bar), m->options_item);

	m->help_item = gtk_menu_item_new_with_label ("Help");
	gtk_menu_shell_append(GTK_MENU_SHELL(m->bar), m->help_item);
	
	gtk_container_add(GTK_CONTAINER(l->top_box), m->bar);
}

void gui_init_tool_box (Gui *g)
{
    Layout *l = &g->layout;
    GtkWidget *tmp_start;
    GtkWidget *tmp_cell;
    GtkWidget *tmp_label;
    GtkWidget *tmp_block;
    GtkWidget *tmp_line;
    GtkWidget *tmp_none;
    GtkWidget *tmp_separator;

    
	tmp_label = gtk_label_new ("Bonus : ");
    tmp_start = gtk_button_new_with_label ("Start");
    tmp_cell = gtk_button_new_with_label ("Cell");
    tmp_block = gtk_button_new_with_label ("Block");
    tmp_line = gtk_button_new_with_label ("Line");
    tmp_none = gtk_button_new_with_label ("None");

    tmp_separator = gtk_vseparator_new ();

    gtk_box_pack_start (GTK_BOX (l->tool_box), tmp_start, !EXPAND, !FILL, 0);
    gtk_box_pack_start (GTK_BOX (l->tool_box), tmp_separator, !EXPAND, !FILL, 3);
    gtk_box_pack_start (GTK_BOX (l->tool_box), tmp_label, !EXPAND, !FILL, 0);
    gtk_box_pack_start (GTK_BOX (l->tool_box), tmp_cell, !EXPAND, !FILL, 0);
    gtk_box_pack_start (GTK_BOX (l->tool_box), tmp_block, !EXPAND, !FILL, 0);
    gtk_box_pack_start (GTK_BOX (l->tool_box), tmp_line, !EXPAND, !FILL, 0);
    gtk_box_pack_start (GTK_BOX (l->tool_box), tmp_none, !EXPAND, !FILL, 0);
    
    ON_SIGNAL (tmp_start, "clicked", on_start_clicked, g);
    ON_SIGNAL (tmp_cell, "clicked", on_cell_bonus_clicked, g);
    ON_SIGNAL (tmp_block, "clicked", on_block_bonus_clicked, g);
    ON_SIGNAL (tmp_line, "clicked", on_line_bonus_clicked, g);
    ON_SIGNAL (tmp_none, "clicked", on_none_bonus_clicked, g);

    g->start_button = tmp_start;
}

/*------------------------ D R A W I N G   A R E A --------------------------*/


gboolean on_area_expose (GtkWidget *widget, GdkEventExpose U *e, gpointer U data)
{
    GdkGC *gc= gdk_gc_new (widget->window);
    int width, height;
    Gui *g = data;
    Info * info = g->info;

    printf ("on_area_expose\n");
    gdk_drawable_get_size (widget->window, &width, &height);
    
	/*draw tout les objets ici */
	info_background_draw (widget->window, gc, info, width, height);
    if (info->aLines->ready != TRUE && info->gameover == 0) {
    	info_shadow_draw (widget->window, gc, info, SIZE_CELL);
	   info_current_piece_draw (widget->window, gc, info, SIZE_CELL);
    }
	info_grid_draw (widget->window, gc, info, SIZE_CELL);
	info_bonus_draw (widget->window, gc, info, SIZE_CELL);
	if (info->gameover == 0) {
		animatedText_draw (g->area, gc, info->aText_down, SIZE_CELL, info->skin);
    	animatedText_draw (g->area, gc, info->aText_line, SIZE_CELL, info->skin);
	}
	info_gameover_draw (g->area, gc, info, SIZE_CELL);
    g_object_unref (gc);
    return TRUE;  /* L'evenement a ete traite' */
}

gboolean on_area_pieces_expose (GtkWidget *widget, GdkEventExpose U *e, gpointer U data)
{
    GdkGC *gc= gdk_gc_new (widget->window);
    int width, height;
    Gui *g = data;
    
    gdk_drawable_get_size (widget->window, &width, &height);
    info_background_draw (widget->window, gc, g->info, width, height);

	/*draw tout les objets ici */
	info_next_pieces_draw (widget->window, gc, g->info, SIZE_CELL);
	area_next_pieces_num_draw (g->area_pieces, gc, SIZE_CELL, g->info->skin);
    g_object_unref (gc);
    return TRUE;  /* L'evenement a ete traite' */
}

gboolean on_area_configure (GtkWidget U *area, GdkEventConfigure *e, gpointer U data)   
{
    /*Gui *g = gui_check (data);
	Info *info = g->info;*/
	
    printf ("on_area_configure width = %d  height = %d\n", 
           (int) e->width, (int) e->height);
	
    return TRUE;  /* L'evenement a ete traite' */
}

gboolean on_area_but_press (GtkWidget U *w, GdkEventButton *e, gpointer U data)   
{
    Gui *g = gui_check (data); 
    gtk_widget_grab_focus(g->area);

    printf ("on_area_but_press  x = %d  y = %d  b = %d\n", 
           (int) e->x, (int) e->y, e->button);

    return TRUE;  /* L'evenement a ete traite' */
}


gboolean on_area_but_release (GtkWidget U *w, GdkEventButton *e, gpointer U d)   
{
    /* Gui *g = gui_check (data); */

    printf ("on_area_but_release  x = %d  y = %d  b = %d\n", 
           (int) e->x, (int) e->y, e->button);

    return TRUE;  /* L'evenement a ete traite' */
}


gboolean on_area_motion (GtkWidget U *w, GdkEventMotion *e, gpointer U d)
{
    /* Gui *g = gui_check (data); */

    printf ("on_area_motion  x = %d  y = %d\n", 
           (int) e->x, (int) e->y);

    return TRUE;  /* L'evenement a ete traite' */
}


gboolean on_area_key_press (GtkWidget U *w, GdkEventKey *e, gpointer U d)
{
    Gui *g = gui_check (d); 
    Info * info = g->info;

    /* ev->keyval contient le symbole de la touche pressee */
    switch (e->keyval) {
        case GDK_Escape : 		gtk_main_quit (); break;
        case GDK_space : 		info->key_space = 1; break;
        case GDK_Left : 		info->key_Left = 1; break;
        case GDK_Right : 		info->key_Right = 1; break;
        case GDK_Up : 			info->key_Up = 1; break;
        case GDK_Down : 		info->key_Down = 1; break;

        default :  printf ("on_area_key_press    \"%s\"  length = %d  keyval = GDK_%s\n",
            e->string, e->length, gdk_keyval_name(e->keyval)); break;
    }

    return TRUE;  /* L'evenement a ete traite' */
}


gboolean on_area_key_release (GtkWidget U *w, GdkEventKey U *e, gpointer U d)
{
    Gui *g = gui_check (d);
    Info * info = g->info;

    /* ev->keyval contient le symbole de la touche relachee */
    switch (e->keyval) {
        case GDK_space :        info->key_space = 0; break;
    	case GDK_Left : 		info->key_Left = 0; break;
        case GDK_Right : 		info->key_Right = 0; break;
        case GDK_Up : 			info->key_Up = 0; break;
        case GDK_Down : 		info->key_Down = 0; break;

        default :  printf ("on_area_key_release  \"%s\"  length = %d  keyval = GDK_%s\n",
            e->string, e->length, gdk_keyval_name(e->keyval)); break;
    }

    return TRUE;  /* L'evenement a ete traite' */
}


void gui_init_area (Gui *g)
{
    Layout *l = &g->layout;

    g->area = gtk_drawing_area_new ();
    gtk_widget_set_size_request (g->area,
                             GRID_X * 2 + g->info->width * SIZE_CELL,
                             GRID_Y * 2 + g->info->height * SIZE_CELL);
    gtk_box_pack_start (GTK_BOX (l->central_box), g->area, EXPAND, FILL, 3);

    GTK_WIDGET_SET_FLAGS  (g->area, GTK_CAN_FOCUS);
    gtk_widget_add_events (g->area, 
        GDK_BUTTON_MOTION_MASK  |
        GDK_BUTTON_PRESS_MASK   |
        GDK_BUTTON_RELEASE_MASK |
        GDK_KEY_PRESS_MASK      |
        GDK_KEY_RELEASE_MASK    |
        GDK_EXPOSURE_MASK);

    ON_SIGNAL (g->area, "expose-event",         on_area_expose,      g);
    ON_SIGNAL (g->area, "button-press-event",   on_area_but_press,   g);
    ON_SIGNAL (g->area, "button-release-event", on_area_but_release, g);
    ON_SIGNAL (g->area, "motion-notify-event",  on_area_motion,      g);
    ON_SIGNAL (g->area, "key-press-event",      on_area_key_press,   g);
    ON_SIGNAL (g->area, "key-release-event",    on_area_key_release, g);
    ON_SIGNAL (g->area, "configure-event",    	on_area_configure,	 g);
}

void gui_init_area_pieces (Gui *g)
{
    Layout *l = &g->layout;

    GtkWidget *pieces_frame;

    pieces_frame = frame_new("Next Pieces", TRUE);
    g->area_pieces = gtk_drawing_area_new ();

	gtk_container_set_border_width(GTK_CONTAINER(pieces_frame), 1);
    gtk_container_add (GTK_CONTAINER(pieces_frame), g->area_pieces);
    gtk_box_pack_start (GTK_BOX (l->info_box), pieces_frame, EXPAND, FILL, 3);

    gtk_widget_add_events (g->area, 
        GDK_BUTTON_MOTION_MASK  |
        GDK_BUTTON_PRESS_MASK   |
        GDK_BUTTON_RELEASE_MASK |
        GDK_KEY_PRESS_MASK      |
        GDK_KEY_RELEASE_MASK    |
        GDK_EXPOSURE_MASK);

    ON_SIGNAL (g->area_pieces, "expose-event",         on_area_pieces_expose,      g);
}


void reafficher (GtkWidget *win) {
	gtk_widget_queue_draw(win);
}


/*--------------------------- S T A T U S B A R -----------------------------*/


void gui_init_statusbar (Gui *g)
{
    Layout *l = &g->layout;
 
    g->statusbar = gtk_statusbar_new ();
    statusbar_put (GTK_STATUSBAR (g->statusbar), "Press Start to play !");
    gtk_box_pack_start (GTK_BOX (l->main_box), g->statusbar, !EXPAND, !FILL, 0);
}


/*----------------------------- C O N F I G ---------------------------*/

void on_config_destroy (GtkWidget U *widget, gpointer U data)
{
	remove (SAVE_FILENAME);

    Gui *g = gui_check (data); 
    gtk_widget_show_all (g->window);
    reafficher (g->window);
}

void on_config_delete_event (GtkWidget U *widget, gpointer U data) {

	GtkWidget *toplevel = gtk_widget_get_toplevel (widget);
   	if (gtk_widget_is_toplevel (toplevel)) {
   		gtk_widget_destroy (toplevel);
   	}
}

void on_config_resume_clicked (GtkWidget U *widget, gpointer U data)
{
    Gui *g = gui_check (data); 
    Info * info = g->info;

    info_fill_with_file (info, SAVE_FILENAME);

    char text[10];

    Layout *l = &g->layout;

    info->score_in_statusbar = info->current_score;
    sprintf (text, "%d", info->score_in_statusbar);
	label_set_text (GTK_LABEL (l->score_label), text);

	info->level_in_statusbar = info->current_level;
	sprintf (text, "%d", info->level_in_statusbar);
    label_set_text (GTK_LABEL (l->level_label), text);

    int time = info->time;
	time = time/1000;
	sprintf (text, "%02d:%02d", ((int) time/60), ((int) time % 60));
    label_set_text (GTK_LABEL (l->time_label), text);

    GtkWidget *toplevel = gtk_widget_get_toplevel (widget);
   	if (gtk_widget_is_toplevel (toplevel)) {
   		gtk_widget_destroy (toplevel);
   	}
   	gtk_widget_show_all (g->window);
   	reafficher (g->window);
}

void on_config_confirm_clicked (GtkWidget U *widget, gpointer U data)
{
    Gui *g = gui_check (data); 


    GtkWidget *toplevel = gtk_widget_get_toplevel (widget);
   	if (gtk_widget_is_toplevel (toplevel)) {
   		gtk_widget_destroy (toplevel);
   	}
   	gtk_widget_show_all (g->window);
   	reafficher (g->window);
}

void on_config_default_clicked (GtkWidget U *widget, gpointer U data)
{
    Gui *g = gui_check (data); 
    Info * info = g->info;

    info->skin = 0;
    info->mode = 0;
    info_set_level(info, 1);
    pix_load_all(g->info);

    GtkWidget *toplevel = gtk_widget_get_toplevel (widget);
   	if (gtk_widget_is_toplevel (toplevel)) {
   		gtk_widget_destroy (toplevel);
   	}
   	gtk_widget_show_all (g->window);
   	reafficher (g->window);
}

void on_config_reset_clicked (GtkWidget U *widget, gpointer U data)
{
    Gui *g = gui_check (data); 

    bestScores_reset (g->bestScores, BS_FILENAME);

    Info * info = g->info;

    info->skin = 0;
    info->mode = 0;
    info_set_level(info, 1);
    pix_load_all(g->info);

    GtkWidget *toplevel = gtk_widget_get_toplevel (widget);
   	if (gtk_widget_is_toplevel (toplevel)) {
   		gtk_widget_destroy (toplevel);
   	}
   	gtk_widget_show_all (g->window);
   	reafficher (g->window);

}

void on_config_skin_changed (GtkComboBox U *widget, gpointer U data)
{
    Gui *g = gui_check (data); 

    int skin_selected = gtk_combo_box_get_active (widget);
    if (skin_selected != g->info->skin) {
        g->info->skin = skin_selected;
        pix_load_all(g->info);
        printf("SKIIIIIIN %d\n", g->info->skin);
    }
}

void on_config_mode_changed (GtkComboBox U *widget, gpointer U data)
{
    Gui *g = gui_check (data); 

    int mode_selected = gtk_combo_box_get_active (widget);
    if (mode_selected != g->info->mode) {
        g->info->mode = mode_selected;
    }
}

void on_config_level_changed (GtkComboBox U *widget, gpointer U data)
{
    Gui *g = gui_check (data); 

    int level_selected = gtk_combo_box_get_active (widget);
    if (level_selected + 1 != g->info->current_level)
        info_set_level(g->info, level_selected + 1);
}

void gui_config_window (Gui * g) {

	GtkWidget *window;
	GtkWidget *main_box;
	GtkWidget *content_box;
	GtkWidget *button_box;
	GtkWidget *text_side;
	GtkWidget *config_side;
	GtkWidget *separator0;
	GtkWidget *separator1;
	GtkWidget *separator2;

	GtkWidget * label_controls;
	GtkWidget * label_bestScores;
	GtkWidget * button_reset;
	GtkWidget * table_scores;

	GtkWidget *combo_mode;
	GtkWidget *combo_skin;
	GtkWidget *combo_level;

	GtkWidget *label_mode;
	GtkWidget *label_skin;
	GtkWidget *label_level;

	GtkWidget *confirm_button;
	GtkWidget *default_button;

	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
	gtk_window_set_title(GTK_WINDOW(window), "Config");
	gtk_container_set_border_width(GTK_CONTAINER(window), 10);

	main_box = gtk_vbox_new (!HOMOGENEOUS, 0);
	content_box = gtk_hbox_new (!HOMOGENEOUS, 0);
	button_box = gtk_hbox_new (!HOMOGENEOUS, 0);
	text_side = gtk_vbox_new (!HOMOGENEOUS, 2);
	config_side = gtk_vbox_new (!HOMOGENEOUS, 3);

	separator0 = gtk_hseparator_new ();
	separator1 = gtk_vseparator_new ();
	separator2 = gtk_hseparator_new ();

	label_controls = gtk_label_new (CONTROL_TEXT);
	label_bestScores = gtk_label_new ("Best Scores");
	table_scores = bestScores_get_table (g->bestScores);
	button_reset = gtk_button_new_with_label ("Reset");

	combo_mode = info_get_mode_combo (g->info);
	combo_skin = info_get_skin_combo (g->info);
	combo_level = info_get_level_combo (g->info);

	label_mode = gtk_label_new ("Mode :");
	label_skin = gtk_label_new ("Skin :");
	label_level = gtk_label_new ("Level :");

	confirm_button = gtk_button_new_with_label ("Ok");
	default_button = gtk_button_new_with_label ("Default");

    char * text = g_locale_to_utf8("<span size=\"xx-large\"><b>Tetris</b></span>\n <span size=\"large\">Configuration</span>\n", -1, NULL, NULL, NULL);
    GtkWidget * title_label = gtk_label_new(text);
    gtk_label_set_use_markup(GTK_LABEL(title_label), TRUE);
    gtk_label_set_justify(GTK_LABEL(title_label), GTK_JUSTIFY_CENTER);

	gtk_container_add(GTK_CONTAINER(window), main_box);
    gtk_box_pack_start(GTK_BOX(main_box), title_label, FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(main_box), content_box, FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(main_box), separator0, FALSE, FALSE, 3);
	gtk_box_pack_start(GTK_BOX(main_box), button_box, FALSE, FALSE, 0);

	gtk_box_pack_start(GTK_BOX(content_box), text_side, FALSE, FALSE, 2);
	gtk_box_pack_start(GTK_BOX(content_box), separator1, FALSE, FALSE, 2);
	gtk_box_pack_start(GTK_BOX(content_box), config_side, FALSE, FALSE, 2);

	gtk_box_pack_start(GTK_BOX(text_side), label_controls, FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(text_side), separator2, FALSE, FALSE, 2);
	gtk_box_pack_start(GTK_BOX(text_side), label_bestScores, FALSE, FALSE, 1);
	gtk_box_pack_start(GTK_BOX(text_side), table_scores, FALSE, FALSE, 0);

	gtk_box_pack_start(GTK_BOX(config_side), label_mode, FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(config_side), combo_mode, FALSE, FALSE, 0);

	gtk_box_pack_start(GTK_BOX(config_side), label_skin, FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(config_side), combo_skin, FALSE, FALSE, 0);

	gtk_box_pack_start(GTK_BOX(config_side), label_level, FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(config_side), combo_level, FALSE, FALSE, 0);

	gtk_box_pack_end(GTK_BOX(button_box), default_button, FALSE, FALSE, 0);
	gtk_box_pack_end(GTK_BOX(button_box), confirm_button, FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(button_box), button_reset, FALSE, FALSE, 0);

	ON_SIGNAL (window, "destroy", on_config_destroy, g);
	ON_SIGNAL (window, "delete-event", on_config_delete_event, g);
	ON_SIGNAL (default_button, "clicked", on_config_default_clicked, g);
	ON_SIGNAL (confirm_button, "clicked", on_config_confirm_clicked, g);

	ON_SIGNAL (combo_mode, "changed", on_config_mode_changed, g);
	ON_SIGNAL (combo_skin, "changed", on_config_skin_changed, g);
	ON_SIGNAL (combo_level, "changed", on_config_level_changed, g);

	if (file_exists (SAVE_FILENAME)) {
		GtkWidget * resume_button = gtk_button_new_with_label ("Resume");
		gtk_box_pack_start(GTK_BOX(button_box), resume_button, FALSE, FALSE, 0);
		ON_SIGNAL (resume_button, "clicked", on_config_resume_clicked, g);
	}

    gtk_window_set_keep_above (GTK_WINDOW(window), TRUE);
	gtk_widget_show_all(window);

}

