/* font.h - originate from EZ-Draw-GTK
 *
 * Edouard.Thiel@lif.univ-mrs.fr - 08/02/2011
 *
 * This program is free software under the terms of the
 * GNU Lesser General Public License (LGPL) version 2.1.
*/

#ifndef FONT__H
#define FONT__H

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <gtk/gtk.h>

typedef enum
{
	FONT_AA = 183200,
	FONT_TL,
	FONT_TC,
	FONT_TR,
	/* Top Left, Top Center, Top Right */
	FONT_ML,
	FONT_MC,
	FONT_MR,
	/* Middle Left, Middle Center, Middle Right */
	FONT_BL,
	FONT_BC,
	FONT_BR,
	/* Bottom Left, Bottom Center, Bottom Right */
	FONT_BB
} Font_align;

void font_set_size(PangoLayout* layout, int size);
void font_set_name(PangoLayout* layout, char const* desc);
void font_draw_text(GtkWidget* widget, GdkGC* gc, PangoLayout* layout,
	Font_align align, int x, int y, const char* format, ...);


#endif /* FONT__H */
