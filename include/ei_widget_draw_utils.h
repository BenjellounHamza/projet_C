#ifndef DEF_WIDGET_DRAW_UTILS
#define DEF_WIDGET_DRAW_UTILS

#include "ei_utils.h"

#define min(a, b) (a < b ? a : b)
#define max(a, b) (b < a ? a : b)

#define PI          acos(-1)
#define cosd(x) cosf(x*PI/180.0)
#define sind(x) sinf(x*PI/180.0)

#define WINDOW_BAR    30
#define BUTTON_RADIUS 10
#define RESIZE_SIZE   15
#define ANGLE_STEP     6


void ei_draw_rounded_frame(ei_surface_t surface,
                           ei_rect_t *rect,
                           int corner_radius,
                           int frame_type,
                           ei_color_t color,
                           ei_rect_t *clipper);


void ei_draw_cadre(ei_surface_t surface,
                   ei_rect_t *rect,
                   ei_relief_t relief_type,
                   int border_width,
                   int corner_radius,
                   ei_color_t color,
                   ei_rect_t *clipper);

#endif
