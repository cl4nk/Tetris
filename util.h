/* util.h - 24/02/2012 */

#ifndef UTIL__H
#define UTIL__H

#include <gtk/gtk.h>


/* Macros */

#define U __attribute__((unused))

#define HOMOGENEOUS TRUE
#define EXPAND      TRUE
#define FILL        TRUE

#define ON_SIGNAL(w,n,c,d) \
    (g_signal_connect(G_OBJECT(w), (n), G_CALLBACK(c), (d) ))


void pixbuf_get_size(GdkPixbuf* pix, int* w, int* h);
GdkPixbuf* pixbuf_load(const char* filename);
void pixbuf_draw(GdkWindow* win, GdkGC* gc, GdkPixbuf* pix, int x, int y);
void pixbuf_draw_with_size(GdkWindow* win, GdkGC* gc, GdkPixbuf* pix, int x, int y, int width, int height);

/* Colors */

void color_set_rgb(GdkColor* color, guint rgb);
void gc_set_rgb_fg(GdkGC* gc, guint rgb);

GdkColor* static_color_from_rgb(guint rgb);
guint rgb_from_color(GdkColor * color);

/* Area*/

void area_redraw(GtkWidget * area);

/* Frames */

GtkWidget* frame_new(gchar* label, gboolean shadowed);


/* Status bars */

void statusbar_put(GtkStatusbar* bar, char* text);
void label_set_text(GtkLabel* label, char* text);

void sound_play(const char* filename, int* status);
void music_play(const char* filename);

int file_exists(const char* filename);

int keys_are_equals(int key_T, int key_B);
int int_is_in(int x, int size);


#endif /* UTIL__H */
