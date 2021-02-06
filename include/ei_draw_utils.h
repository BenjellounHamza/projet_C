#ifndef DEF_DRAW_UTILS
#define DEF_DRAW_UTILS

#include "hw_interface.h"

#define min(a, b) (a < b ? a : b)
#define max(a, b) (b < a ? a : b)

#define BIG_NUMBER  0x7FFFFFFF


typedef struct ei_side_t
{
    int y_max;
    float x_ymin;
    float fract;
    struct ei_side_t *next;
} ei_side_t;


typedef struct {
    int y;
    ei_side_t *sides;
} ei_scanline_t;


ei_color_t ei_unmap_color(ei_surface_t surface, uint32_t mapped_color);


ei_color_t ei_mixed_color(ei_color_t old_color, const ei_color_t *new_color);


void ei_fill_pixel(ei_surface_t surface, uint32_t* pixel_addr, const ei_color_t* color);


void ei_draw_line(ei_surface_t surface,
                  const ei_color_t *color,
                  const ei_point_t *first_point,
                  const ei_point_t *second_point,
                  const ei_rect_t *clipper);


int ei_initialise_tc_table(ei_scanline_t **tc_table,
                           const ei_linked_point_t *first_point);


void ei_add_tc_to_tca(ei_side_t **tca_table, ei_side_t *sides);


void ei_sort_tca(ei_side_t *tca_table);


void ei_fill_intervals(ei_surface_t surface,
                       ei_side_t *tca_table,
                       uint32_t *addr_pixel,
                       const ei_color_t *color,
                       const ei_rect_t *clipper);


void ei_remove_from_tca(ei_side_t **tca_table, int y);


void ei_update_tca(ei_side_t *tca_table);


void ei_free_tc_table(ei_scanline_t *tc_table, int tc_size);

#endif
