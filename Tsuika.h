

#ifndef __TSUIKA_H__
#define __TSUIKA_H__

#include <gtk/gtk.h>

#define WINDOW_WIDTH 600
#define WINDOW_HEIGHT 400
#define AREA_WIDTH 200
#define AREA_HEIGHT 350

#define MAX_FRUITS 256
#define TIME_DIFF  15


void initGraphics(GtkApplication* app, gpointer user_data);
void draw(GtkDrawingArea *drawing_area, cairo_t *cr, int width, int height, gpointer user_data);


#endif