#include <stdio.h>
#include <stdlib.h>
#include "ei_draw_utils.h"


extern uint32_t ei_map_rgba(ei_surface_t surface, const ei_color_t* color);


void swap(int *first, int *second)
{
    int tmp = *first;
    *first = *second;
    *second = tmp;
}


ei_color_t ei_unmap_color(ei_surface_t surface, uint32_t mapped_color)
{
    int ir, ig, ib, ia;
    hw_surface_get_channel_indices(surface, &ir, &ig, &ib, &ia);

    ei_color_t color;
    color.red = (unsigned char)(mapped_color>>8*ir);
    color.green = (unsigned char)(mapped_color>>8*ig);
    color.blue = (unsigned char)(mapped_color>>8*ib);

    if (ia != -1)
        color.alpha = (unsigned char)(mapped_color>>8*ia);

    return color;
}


ei_color_t ei_mixed_color(ei_color_t old_color, const ei_color_t *new_color)
{
    old_color.red = (new_color->red*new_color->alpha + old_color.red*(255-new_color->alpha))/255;
    old_color.green = (new_color->green*new_color->alpha + old_color.green*(255-new_color->alpha))/255;
    old_color.blue = (new_color->blue*new_color->alpha + old_color.blue*(255-new_color->alpha))/255;
    old_color.alpha = new_color->alpha;

    return old_color;
}


void ei_fill_pixel(ei_surface_t surface, uint32_t* pixel_addr, const ei_color_t* color)
{
    ei_color_t old_color = ei_unmap_color(surface, *pixel_addr);
    ei_color_t new_color = ei_mixed_color(old_color, color);
    *pixel_addr = ei_map_rgba(surface, &new_color);
}


void ei_draw_line(ei_surface_t surface,
                  const ei_color_t *color,
                  const ei_point_t *first_point,
                  const ei_point_t *second_point,
                  const ei_rect_t *clipper)
{
    int i, i_min, i_max;
    int j, j_min, j_max;
    int x1 = first_point->x, x2 = second_point->x;
    int y1 = first_point->y, y2 = second_point->y;
    int delta_x = abs(x2 - x1), delta_y = abs(y2 - y1);
    int erreur = 0;

    ei_size_t size = hw_surface_get_size(surface);
    uint32_t *addr_pixel = (uint32_t *)hw_surface_get_buffer(surface);

    if (clipper != NULL)
    {
        i_min = clipper->top_left.x, i_max = i_min + (clipper->size).width;
        j_min = clipper->top_left.y, j_max = j_min + (clipper->size).height;
    }

    if (delta_y <= delta_x)
    {
        if (x1 > x2)
        {
            swap(&x1, &x2);
            swap(&y1, &y2);
        }
        addr_pixel += x1 + size.width * y1;
        int j_increment = (y1 < y2 ? 1 : -1);
        for (i = x1, j = y1; i <= x2; i++, addr_pixel++)
        {
            if (clipper != NULL)
            {
                if (i_min <= i && i < i_max && j_min <= j && j < j_max)
                    ei_fill_pixel(surface, addr_pixel, color);
            }
            else
                ei_fill_pixel(surface, addr_pixel, color);

            erreur += delta_y;
            if (2 * erreur > delta_x)
            {
                erreur -= delta_x;
                addr_pixel += j_increment * size.width;
                j += j_increment;
            }
        }
    }
    else
    {
        swap(&delta_x, &delta_y);
        if (y1 > y2)
        {
            swap(&x1, &x2);
            swap(&y1, &y2);
        }
        addr_pixel += x1 + size.width * y1;
        int i_increment = (x1 < x2 ? 1 : -1);
        for (i= x1, j = y1; j <= y2; j++, addr_pixel += size.width)
        {
            if (clipper != NULL)
            {
                if (i_min <= i && i < i_max && j_min <= j && j < j_max)
                    ei_fill_pixel(surface, addr_pixel, color);
            }
            else
                ei_fill_pixel(surface, addr_pixel, color);

            erreur += delta_y;
            if (2 * erreur > delta_x)
            {
                erreur -= delta_x;
                addr_pixel += i_increment;
                i += i_increment;
            }
        }
    }
}


void ei_min_max_scanlines(const ei_linked_point_t *first_point,
                          int *min, int *max)
{
    while (first_point != NULL)
    {
        int y = first_point->point.y;
        if (y < *min)
            *min = y;
        else if (y > *max)
            *max = y;
        first_point = first_point->next;
    }
}


void ei_free_tc_table(ei_scanline_t *tc_table, int tc_size)
{
    for (int scanline = 0; scanline < tc_size; scanline++)
    {
        if (tc_table[scanline].sides != NULL)
            free(tc_table[scanline].sides);
    }
    free(tc_table);
}


int ei_initialise_tc_table(ei_scanline_t **tc_table,
                           const ei_linked_point_t *first_point)
{
    int min = BIG_NUMBER, max = 0;
    ei_min_max_scanlines(first_point, &min, &max);
    int number_scanlines = max - min + 1;

    ei_scanline_t *scanlines = malloc(number_scanlines * sizeof(ei_scanline_t));
    for (int i = 0; i < number_scanlines; i++)
    {
      scanlines[i].y = i + min;
      scanlines[i].sides = NULL;
    }

    ei_linked_point_t *second_point = first_point->next;
    while (second_point != NULL)
    {
        int y1 = first_point->point.y, y2 = second_point->point.y;
        if (y1 != y2)
        {
            ei_side_t *side = malloc(sizeof(ei_side_t));

            int x1 = first_point->point.x, x2 = second_point->point.x;
            if (y2 < y1)
            {
              swap(&x1, &x2);
              swap(&y1, &y2);
            }

            side->next = scanlines[y1 - min].sides;
            scanlines[y1 - min].sides = side;
            side->y_max = y2;
            side->x_ymin = x1;
            side->fract = (x2 - x1) / (float)(y2 - y1);
        }

        first_point = second_point;
        second_point = first_point->next;
    }

    *tc_table = scanlines;
    return number_scanlines;
}


/* Pour debugger */
void ei_print_sides(ei_side_t *side)
{
    if (side == NULL)
      printf("NULL\n");
    while (side != NULL)
    {
        printf("addr: %p, y_max: %d, x_ymin: %f, fract: %f, next: %p\n",
               side, side->y_max, side->x_ymin, side->fract, side->next);
        side = side->next;
    }
    printf("\n");
}


/* Pour debugger */
void ei_print_tc(ei_scanline_t *tc_table, int tc_size)
{
    for (int i = 0; i < tc_size; i++)
    {
      printf("scanline numero: %d\n", tc_table[i].y);
      ei_print_sides(tc_table[i].sides);
      printf("\n");
    }
}


void ei_add_tc_to_tca(ei_side_t **tca_table, ei_side_t *sides)
{
    if (*tca_table == NULL)
        *tca_table = sides;
    else
    {
        ei_side_t *current_side = *tca_table;
        while (current_side->next != NULL)
        {
            current_side = current_side->next;
        }
        current_side->next = sides;
    }
}


void swap_sides(ei_side_t *side, ei_side_t *other)
{
    int tmp1 = side->y_max;
    float tmp2 = side->x_ymin, tmp3 = side->fract;

    side->y_max = other->y_max;
    side->x_ymin = other->x_ymin;
    side->fract = other->fract;

    other->y_max = tmp1;
    other->x_ymin = tmp2;
    other->fract = tmp3;
}


void ei_sort_tca(ei_side_t *tca_table)
{
    ei_side_t *current_side = tca_table, *swap_side, *scan_side;
    float current_min;

    while (current_side != NULL)
    {
        current_min = current_side->x_ymin;
        scan_side = current_side->next;
        swap_side = NULL;

        while (scan_side != NULL)
        {
            if (scan_side->x_ymin < current_min ||
                (scan_side->x_ymin == current_min &&
                 scan_side->fract < current_side->fract))
            {
                current_min = scan_side->x_ymin;
                swap_side = scan_side;
            }
            scan_side = scan_side->next;
        }

        if (swap_side != NULL)
            swap_sides(current_side, swap_side);
        current_side = current_side->next;
    }
}


void ei_fill_intervals(ei_surface_t surface,
                       ei_side_t *tca_table,
                       uint32_t *addr_pixel,
                       const ei_color_t *color,
                       const ei_rect_t *clipper)
{
    ei_side_t *start_side = tca_table, *stop_side = start_side->next;

    int x_min = 0, x_max = BIG_NUMBER;
    if (clipper != NULL)
    {
        x_min = (clipper->top_left).x;
        x_max = x_min + (clipper->size).width;
    }

    for (;;)
    {
        int x_start = start_side->x_ymin,
            x_stop = stop_side->x_ymin;

        if (x_stop >= x_min && x_start <= x_max)
        {
            x_stop = min(x_stop, x_max);
            x_start = max(x_start, x_min);
            for (int x = x_start; x < x_stop; x++)
            {
                ei_fill_pixel(surface, addr_pixel + x, color);
            }
        }

        start_side = stop_side->next;
        if (start_side != NULL)
            stop_side = start_side->next;
        else
            break;
    }
}


void ei_update_tca(ei_side_t *tca_table)
{
    ei_side_t *current_side = tca_table;
    while (current_side != NULL)
    {
        current_side->x_ymin += current_side->fract;
        current_side = current_side->next;
    }
}


void ei_remove_from_tca(ei_side_t **tca_table, int y)
{
    ei_side_t *current_side = *tca_table;
    while (current_side != NULL && current_side->y_max == y)
    {
        current_side = current_side->next;
        *tca_table = current_side;
    }

    ei_side_t *next_side;
    while (current_side != NULL)
    {
        next_side = current_side->next;
        while (next_side != NULL && next_side->y_max == y)
        {
            next_side = next_side->next;
        }
        current_side->next = next_side;
        current_side = next_side;
    }
}
