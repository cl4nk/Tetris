/* util.c - 24/02/2012 */

#include "util.h"
#include <gdk-pixbuf/gdk-pixbuf.h>


void pixbuf_get_size (GdkPixbuf *pix, int *w, int *h)
{
	*w = gdk_pixbuf_get_width (pix); *h = gdk_pixbuf_get_height (pix);
}

GdkPixbuf *pixbuf_load (const char *filename)
{
	GError *error = NULL; GdkPixbuf *pix; int w, h;
	printf ("Loading %s ... ", filename); fflush (stdout);
	pix = gdk_pixbuf_new_from_file (filename, &error);
	if (pix == NULL) {
		printf ("error\n");
		fprintf (stderr, "Unable to read file: %s\n", error->message);
		g_error_free (error); return NULL;
	}
	pixbuf_get_size (pix, &w, &h); printf ("%dx%d\n", w, h);
	return pix;
}

void pixbuf_draw (GdkWindow *win, GdkGC *gc, GdkPixbuf *pix, int x, int y)
{
	gdk_draw_pixbuf (win, gc, pix, 0, 0, x, y, -1, -1, GDK_RGB_DITHER_NONE, 0, 0);
}

void pixbuf_draw_with_size (GdkWindow *win, GdkGC *gc, GdkPixbuf *pix, int x, int y, int width, int height)
{
	gdk_draw_pixbuf (win, gc, pix, 0, 0, x, y, width, height, GDK_RGB_DITHER_NONE, 0, 0);
}


/*------------------------------- C O L O R S -------------------------------*/


void color_set_rgb (GdkColor *color, guint rgb)
{
    color->red   = (rgb & 0xFF0000) >> 8;
    color->green = (rgb & 0x00FF00)     ;
    color->blue  = (rgb & 0x0000FF) << 8;
}

void gc_set_rgb_fg (GdkGC *gc, guint rgb)
{
    gdk_gc_set_rgb_fg_color (gc, static_color_from_rgb (rgb));
}

GdkColor *static_color_from_rgb (guint rgb)
{
    static GdkColor color;
    color_set_rgb (&color, rgb);
    return &color;
}

guint rgb_from_color (GdkColor *color)
{
    return (color->red   & 0xFF00) << 8 |
           (color->green & 0xFF00)      |
           (color->blue  & 0xFF00) >> 8 ;
}

/*--------------------------------- A R E A ---------------------------------*/

void area_redraw (GtkWidget * area) {
	gdk_window_invalidate_rect (area->window, NULL, TRUE);
}

/*------------------------------- F R A M E S -------------------------------*/


GtkWidget *frame_new (gchar *label, gboolean shadowed)
{
    GtkWidget *frame = gtk_frame_new (label);
    GtkShadowType type = shadowed ? GTK_SHADOW_ETCHED_IN : GTK_SHADOW_NONE;
    gtk_frame_set_shadow_type (GTK_FRAME (frame), type);
    return frame;
}


/*------------------------- S T A T U S    B A R ----------------------------*/


void statusbar_put (GtkStatusbar *bar, char *text)
{
    gtk_statusbar_pop (bar, 0);
    gtk_statusbar_push (bar, 0, text);
}

void label_set_text (GtkLabel * label, char *text) {
	gtk_label_set_label (label, text);
}
/*
void music_play (const char * filename) {
	sfMusic * music = sfMusic_CreateFromFile(filename);
	if (!music)
		exit(1);

	sfMusic_Play(music);

	sfMusic_Destroy (music);
}*/

int file_exists(const char * filename)
{
	FILE * file = fopen(filename, "r");
    if (file != NULL)
    {
        fclose(file);
        return TRUE;
    }
    return FALSE;
}

int keys_are_equals (int key_T, int key_B) {
    return (key_T == 0 && key_B == 0) || (key_T == 1 && key_B == 1);
}

int int_is_in (int x, int size) {
    return (x >=0 && x < size);
}

/*---------------------------------------------------------------------------*/

