#ifndef DEF_PLACER_UTILS
#define DEF_PLACER_UTILS

#include "ei_utils.h"

#define min(a, b) (a < b ? a : b)
#define max(a, b) (b < a ? a : b)


int ei_rect_intersect(ei_rect_t *rect, ei_rect_t *reference_rect);


int ei_rect_contained(ei_rect_t *rect, ei_rect_t *container);


ei_rect_t ei_rect_intersection(ei_rect_t *rect1, ei_rect_t *rect2);


void ei_placer_set_int_param(ei_placer_params_t *placer_params,
                             int *new_param, int **old_param, int *param_value);


void ei_placer_set_float_param(ei_placer_params_t *placer_params,
                               float *new_param, float **old_param, float *param_value);


void ei_calculate_topleft(ei_rect_t *rect, ei_point_t *anchor_point, ei_anchor_t anchor);


ei_point_t ei_calculate_where(ei_rect_t container, ei_size_t *size,
                              ei_anchor_t anchor, int over_head);


#endif
