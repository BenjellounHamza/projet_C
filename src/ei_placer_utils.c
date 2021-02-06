#include <stdlib.h>
#include "ei_placer.h"
#include "ei_placer_utils.h"


void ei_placer_set_int_param(ei_placer_params_t *placer_params, int *new_param,
                             int **old_param, int *param_value)
{
    if (new_param != NULL)
        *param_value = *new_param;
    else if (*old_param == NULL)
        *param_value = 0;
    *old_param = param_value;
}


void ei_placer_set_float_param(ei_placer_params_t *placer_params, float *new_param,
                               float **old_param, float *param_value)
{
    if (new_param != NULL)
        *param_value = *new_param;
    else if (*old_param == NULL)
        *param_value = 0.0;
    *old_param = param_value;
}


int ei_rect_intersect(ei_rect_t *rect, ei_rect_t *reference_rect)
{
    // intersection on the x axis
    if (rect->top_left.x < reference_rect->top_left.x + reference_rect->size.width
        && reference_rect->top_left.x < rect->top_left.x + rect->size.width)
        // intersection on the y axis
        if (rect->top_left.y < reference_rect->top_left.y + reference_rect->size.height
            && reference_rect->top_left.y < rect->top_left.x + rect->size.height)
            return 1;

    return 0;
}


int ei_rect_contained(ei_rect_t *rect, ei_rect_t *container)
{
    if (container->top_left.x <= rect->top_left.x && 
        rect->top_left.x + rect->size.width <= container->top_left.x + container->size.width)
        if (container->top_left.y <= rect->top_left.y && 
            rect->top_left.y + rect->size.height <= container->top_left.y + container->size.height)
            return 1;

    return 0;
}


ei_rect_t ei_rect_intersection(ei_rect_t *rect1, ei_rect_t *rect2)
{
    int x_min = max(rect1->top_left.x, rect2->top_left.x),
        x_max = min(rect1->top_left.x + rect1->size.width,
                    rect2->top_left.x + rect2->size.width);

    int y_min = max(rect1->top_left.y, rect2->top_left.y),
        y_max = min(rect1->top_left.y + rect1->size.height,
                    rect2->top_left.y + rect2->size.height);

    return ei_rect(ei_point(x_min, y_min), ei_size(x_max - x_min, y_max - y_min));
}


ei_point_t ei_calculate_where(ei_rect_t container, ei_size_t *size,
                              ei_anchor_t anchor, int over_head)
{
    container.size.width -= 2 * over_head; container.size.height -= 2 * over_head;
    container.top_left.x += over_head; container.top_left.y += over_head;

    ei_point_t where = container.top_left;

    if (size->width <= container.size.width)
    {
        switch (anchor)
        {
            case ei_anc_center: case ei_anc_north: case ei_anc_south:
                where.x += (container.size.width - size->width) / 2;
                break;

            case ei_anc_east: case ei_anc_northeast: case ei_anc_southeast:
                where.x += container.size.width - size->width;
                break;

            default:
                break;
        }
    }

    if (size->height <= container.size.height)
    {
        switch (anchor)
        {
            case ei_anc_center: case ei_anc_west: case ei_anc_east:
                where.y += (container.size.height - size->height) / 2;
                break;

            case ei_anc_south: case ei_anc_southeast: case ei_anc_southwest:
                where.y += container.size.height - size->height;
                break;

            default:
                break;
        }
    }

    return where;
}


void ei_calculate_topleft(ei_rect_t *rect, ei_point_t *anchor_point, ei_anchor_t anchor)
{
    rect->top_left.x = anchor_point->x;
    switch (anchor)
    {
        case ei_anc_center: case ei_anc_north: case ei_anc_south:
            rect->top_left.x -= rect->size.width / 2;
            break;

        case ei_anc_east: case ei_anc_northeast: case ei_anc_southeast:
            rect->top_left.x -= rect->size.width;
            break;

        default:
            break;
    }

    rect->top_left.y = anchor_point->y;
    switch (anchor)
    {
        case ei_anc_center: case ei_anc_west: case ei_anc_east:
            rect->top_left.y -= rect->size.height / 2;
            break;

        case ei_anc_south: case ei_anc_southeast: case ei_anc_southwest:
            rect->top_left.y -= rect->size.height;
            break;

        default:
            break;
    }
}
