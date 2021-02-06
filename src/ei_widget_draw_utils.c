#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "ei_draw.h"
#include "ei_widget_draw_utils.h"


ei_linked_point_t * ei_create_arc(ei_point_t center, int radius,
                                  int starting_angle, int ending_angle,
                                  ei_linked_point_t *queue)
{
    int number_of_steps;
    float step, current_angle = starting_angle;
    if (radius == 0)
        number_of_steps = 0;
    else
    {
        number_of_steps = (ending_angle - starting_angle) / ANGLE_STEP;
        step = (ending_angle - starting_angle) / (float)number_of_steps;
    }

    ei_linked_point_t *current_linked_point, *next_linked_point = queue;
    for (int i = 0; i <= number_of_steps; i++, current_angle += step)
    {
        current_linked_point = malloc(sizeof(ei_linked_point_t));

        current_linked_point->point = ei_point_add(center,
                ei_point(radius * cosd(current_angle),
                         radius * sind(current_angle)));

        current_linked_point->next = next_linked_point;

        next_linked_point = current_linked_point;
    }

    return current_linked_point;
}


void ei_free_rounded_frame(ei_linked_point_t *points)
{
    ei_linked_point_t *previous_point;
    while (points != NULL)
    {
        previous_point = points;
        points = points->next;
        free(previous_point);
    }
}


void ei_generate_frame_divider(ei_rect_t *rect, ei_point_t *point1,
                               ei_point_t *point2)
{
    if (rect->size.height < rect->size.width)
    {
        point1->x = rect->top_left.x + rect->size.height / 2;
        point1->y = rect->top_left.y + rect->size.height / 2;
        point2->x = rect->top_left.x + rect->size.width - rect->size.height / 2;
        point2->y = point1->y;
    }
    else
    {
        point2->x = rect->top_left.x + rect->size.width / 2;
        point2->y = rect->top_left.y + rect->size.width / 2;
        point1->x = point2->x;
        point1->y = rect->top_left.y + rect->size.height - rect->size.width / 2;
    }
}


void ei_darken_color(const ei_color_t *color, ei_color_t *dest_color)
{
    dest_color->red = color->red * 0.7;
    dest_color->green = color->green * 0.7;
    dest_color->blue = color->blue * 0.7;
    dest_color->alpha = color->alpha;
}


void ei_lighten_color(const ei_color_t *color, ei_color_t *dest_color)
{
    dest_color->red = min(255, color->red * 1.3);
    dest_color->green = min(255, color->green * 1.3);
    dest_color->blue = min(255, color->blue * 1.3);
    dest_color->alpha = color->alpha;
}


/*    _________ <---- higher rect
 *  _|_________|_
 * | |         | |<-- larger rect
 * |_|_________|_|
 *   |_________|
 *
 *   frame_type < 0 -> down side
 *   frame_type > 0 -> upper side
 *   frame_type = 0 -> whole button
 */
void ei_draw_rounded_frame(ei_surface_t surface,
                           ei_rect_t *rect,
                           int corner_radius,
                           int frame_type,
                           ei_color_t color,
                           ei_rect_t *clipper)
{
    int rect_x_min = rect->top_left.x, rect_y_min = rect->top_left.y;

    int higher_x_max = rect_x_min + rect->size.width - corner_radius,
        larger_y_max = rect_y_min + rect->size.height - corner_radius;

    ei_point_t center_top_right = ei_point(higher_x_max,
                                           rect_y_min + corner_radius),
               center_bottom_left = ei_point(rect_x_min + corner_radius,
                                             larger_y_max),
               center_others;

    ei_linked_point_t *current_point = NULL, *first_point;

    if (frame_type <= 0)
    {
        current_point = malloc(sizeof(ei_linked_point_t));
        current_point->point = ei_point_add(center_top_right,
                ei_point(corner_radius * cosd(315), corner_radius * sind(315)));
        current_point->next = NULL;
        first_point = current_point;

        if (corner_radius != 0)
            current_point = ei_create_arc(center_top_right, corner_radius, 315+ANGLE_STEP, 360, current_point);
        center_others = ei_point(higher_x_max, larger_y_max);
        current_point = ei_create_arc(center_others, corner_radius, 0, 90, current_point);
        current_point = ei_create_arc(center_bottom_left, corner_radius, 90, 135, current_point);
    }
    else
    {
        current_point = malloc(sizeof(ei_linked_point_t));
        current_point->point = ei_point_add(center_bottom_left,
                ei_point(corner_radius * cosd(135), corner_radius * sind(135)));
        current_point->next = NULL;
        first_point = current_point;
    }

    if (frame_type >= 0)
    {
        if (corner_radius != 0)
            current_point = ei_create_arc(center_bottom_left, corner_radius, 135+ANGLE_STEP, 180, current_point);
        center_others = ei_point(rect_x_min + corner_radius, rect_y_min + corner_radius);
        current_point = ei_create_arc(center_others, corner_radius, 180, 270, current_point);
        current_point = ei_create_arc(center_top_right, corner_radius, 270, 315, current_point);
    }

    if (frame_type != 0 && rect->size.width != rect->size.height)
    {
        ei_linked_point_t *point1 = malloc(sizeof(ei_linked_point_t)),
                          *point2 = malloc(sizeof(ei_linked_point_t));
        if (frame_type < 0)
            ei_generate_frame_divider(rect, &(point1->point), &(point2->point));
        else
            ei_generate_frame_divider(rect, &(point2->point), &(point1->point));
        point2->next = point1; point1->next = current_point;
        current_point = point2;
    }

    ei_linked_point_t *rounded_frame = malloc(sizeof(ei_linked_point_t));
    rounded_frame->next = current_point;
    rounded_frame->point = first_point->point;

    ei_draw_polygon(surface, rounded_frame, color, clipper);
    ei_free_rounded_frame(rounded_frame);
}

void ei_draw_cadre(ei_surface_t surface,
                   ei_rect_t *rect,
                   ei_relief_t relief_type,
                   int border_width,
                   int corner_radius,
                   ei_color_t color,
                   ei_rect_t *clipper)
{
    ei_color_t upper_frame_color, bottom_frame_color;                 
    if (relief_type == ei_relief_sunken) {
        ei_darken_color(&color, &upper_frame_color);
        ei_lighten_color(&color, &bottom_frame_color);
    } else {
        ei_darken_color(&color, &bottom_frame_color);
        if (relief_type == ei_relief_none)
            upper_frame_color = bottom_frame_color;
        else
            ei_lighten_color(&color, &upper_frame_color);
    }

    ei_point_t inner_rect_top_left = rect->top_left;
    inner_rect_top_left.x += border_width; inner_rect_top_left.y += border_width;
    ei_size_t inner_rect_size = rect->size;
    inner_rect_size.width -= 2*border_width; inner_rect_size.height -= 2*border_width;
    ei_rect_t inner_rect;
    inner_rect.top_left = inner_rect_top_left; inner_rect.size = inner_rect_size;

    ei_draw_rounded_frame(surface, rect, corner_radius, 1, upper_frame_color, clipper);
    ei_draw_rounded_frame(surface, rect, corner_radius,-1, bottom_frame_color, clipper);

    if (corner_radius > border_width)
        corner_radius -= border_width;
    ei_draw_rounded_frame(surface, &inner_rect, corner_radius, 0, color, clipper);
}
