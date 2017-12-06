/* font.c - originate from EZ-Draw-GTK
 *
 * Edouard.Thiel@lif.univ-mrs.fr - 08/02/2011
 *
 * This program is free software under the terms of the
 * GNU Lesser General Public License (LGPL) version 2.1.
*/

/* Rend vsnprintf compatible avec -ansi */
#if defined __STRICT_ANSI__ && !defined _GNU_SOURCE
#define _GNU_SOURCE
#include <stdio.h>
#undef _GNU_SOURCE
#else
#include <stdio.h>
#endif
#include <string.h>

#include "font.h"
#include "util.h"


/* Changer la taille de la foncte actuelle ; taille en pixels.
*/

void font_set_size(PangoLayout* layout, int size)
{
	const PangoFontDescription* d1 = pango_layout_get_font_description(layout);
	PangoFontDescription* d2 = pango_font_description_copy(d1);
	pango_font_description_set_absolute_size(d2, size * PANGO_SCALE);
	pango_layout_set_font_description(layout, d2);
	pango_font_description_free(d2);
}


/* Chargement d'une fonte.
 * Usage : font_set_name (layout, "Times, bold italic 20");
*/

void font_set_name(PangoLayout* layout, char const* desc)
{
	PangoFontDescription* d;
	d = pango_font_description_from_string(desc);
	pango_layout_set_font_description(layout, d);
	pango_font_description_free(d);
}


/*
 * Ecrit du texte, s'utilise comme printf.
 *
 * L'affichage de texte necessite un PangoLayout.
 * Si on fait peu d'affichages dans la meme callback, on peut demander
 * a la fonction de creer un layout temporaire en lui passant NULL :
 *
 *     font_draw_text (widget, gc, NULL, FONT_TL, 10, 10, "Bonjour");
 *
 * Si on fait beaucoup d'affichage de texte, il vaut mieux creer un
 * layout avant et le detruire apres :
 *
 *     PangoLayout *layout = gtk_widget_create_pango_layout (widget, NULL);
 *     font_draw_text (widget, gc, layout, FONT_TL, 10, 10, "Bonjour");
 *     font_draw_text (widget, gc, layout, FONT_TL, 10, 30, "Hello");
 *     g_object_unref (G_OBJECT (layout));
*/

void font_draw_text(GtkWidget* widget, GdkGC* gc, PangoLayout* layout,
	Font_align align, int x, int y, const char* format, ...)
{
	int valign, halign, layout_priv = 0;
	va_list ap;
	char buf[16384];
	PangoRectangle extents;

	if (align <= FONT_AA || align >= FONT_BB)
	{
		fprintf(stderr, "font_draw_text: bad align\n");
		return;
	}

	/* Decode align */
	align -= FONT_AA + 1;
	halign = align % 3;
	valign = align / 3;

	/* Ecrit la chaine formatee dans buf */
	va_start(ap, format);
	vsnprintf(buf, sizeof(buf) - 1, format, ap);
	va_end(ap);
	buf[sizeof(buf) - 1] = 0;
	if (buf[0] == 0) return;

	if (layout == NULL)
	{
		/* On cree un layout prive' pour dessiner du texte */
		layout = gtk_widget_create_pango_layout(widget, NULL);
		layout_priv = 1;
	}

	/* On donne la chaine a afficher */
	pango_layout_set_text(layout, buf, strlen(buf));

	/* On recupere les dimensions */
	pango_layout_get_pixel_extents(layout, &extents, NULL);

	/* Alignement horizontal dans le layout */
	pango_layout_set_alignment(layout,
		halign == 0 ? PANGO_ALIGN_LEFT : halign == 1 ? PANGO_ALIGN_CENTER : PANGO_ALIGN_RIGHT);

	/* Debug : affichage de la bounding box */
	if (FALSE)
	{
		gc_set_rgb_fg(gc, 0x888888);
		gdk_draw_rectangle(widget->window, gc, FALSE,
			x - extents.width * halign / 2,
			y - extents.height * valign / 2,
			extents.width, extents.height);
		gc_set_rgb_fg(gc, 0xFF0000);
		gdk_draw_line(widget->window, gc, x - 20, y, x + 20, y);
		gdk_draw_line(widget->window, gc, x, y - 20, x, y + 20);
		gc_set_rgb_fg(gc, 0x0000FF);
	}

	/* Affichage du layout */
	gdk_draw_layout(widget->window, gc,
		x - extents.x - extents.width * halign / 2,
		y - extents.y - extents.height * valign / 2,
		layout);

	if (layout_priv)
	{
		g_object_unref(G_OBJECT(layout)); /* Libere la memoire */
	}
}
