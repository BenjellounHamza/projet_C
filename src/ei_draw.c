#include <stdlib.h>
#include <stdio.h>
#include "ei_draw.h"
#include "ei_utils.h"

#include "ei_draw_utils.h"


/*
Converts the three red, green and blue component of a color in a 32 bits integer
*/
uint32_t ei_map_rgba(ei_surface_t surface, const ei_color_t* color)
{
    int ir, ig, ib, ia;
    hw_surface_get_channel_indices(surface, &ir, &ig, &ib, &ia);

    uint32_t mapped_color = (color->red<<8*ir)+(color->green<<8*ig)+(color->blue<<8*ib);

    if (ia != -1)
        mapped_color += (color->alpha<<8*ia);

    return mapped_color;
}


/*
Draws a line made of many line segments.
*/
void ei_draw_polyline(ei_surface_t surface,
                      const ei_linked_point_t *first_point,
                      const ei_color_t color,
                      const ei_rect_t *clipper)
{
    if (first_point != NULL)
    {
        ei_linked_point_t *second_point = first_point->next;

        while (second_point != NULL)
        {
            ei_draw_line(surface, &color, &(first_point->point),
                         &(second_point->point), clipper);
            first_point = second_point;
            second_point = first_point->next;
        }
    }
}


void ei_fill(ei_surface_t surface,
             const ei_color_t *color,
             const ei_rect_t *clipper)
{
    if (color == NULL)
    {
        ei_color_t opaque_color = {0xFF, 0xFF, 0xFF, 0xFF};
        color = &opaque_color;
    }

    ei_size_t size = hw_surface_get_size(surface);
    uint32_t *addr_pixel = (uint32_t *)hw_surface_get_buffer(surface);

    int x_min = 0, x_max, y_min = 0, y_max, y_addr_increment = 0;
    if (clipper != NULL)
    {
        x_min = (clipper->top_left).x; x_max = min(x_min + (clipper->size).width, size.width);
        y_min = (clipper->top_left).y; y_max = min(y_min + (clipper->size).height, size.height);
        y_addr_increment = max(size.width - (clipper->size).width, x_min);
    }
    else
    {
        x_max = size.width; y_max = size.height;
    }

    addr_pixel += x_min + y_min * size.width;
    for (int y = y_min; y < y_max; y++, addr_pixel += y_addr_increment)
        for (int x = x_min; x < x_max; x++, addr_pixel++)
            ei_fill_pixel(surface, addr_pixel, color);
}


void ei_draw_polygon(ei_surface_t surface,
                     const ei_linked_point_t *first_point,
                     const ei_color_t color,
                     const ei_rect_t *clipper)
{
    if (first_point != NULL)
    {
        ei_scanline_t *tc_table = NULL;
        ei_side_t *tca_table = NULL;
        int tc_size = ei_initialise_tc_table(&tc_table, first_point);

        ei_size_t size = hw_surface_get_size(surface);
        uint32_t *addr_pixel = (uint32_t *)hw_surface_get_buffer(surface);

        int current_scanline_number = tc_table[0].y;
        addr_pixel += current_scanline_number * size.width;

        int j_min = 0, j_max = tc_size;
        if (clipper != NULL)
        {
            j_max = min((clipper->top_left).y + (clipper->size).height -
                        current_scanline_number, tc_size );
            j_min = max(0, (clipper->top_left).y - current_scanline_number);
        }

        for (int j = 0; j < j_max; j++, addr_pixel += size.width,
                                   current_scanline_number++)
        {
            if (tc_table[j].sides != NULL)
            {
                ei_add_tc_to_tca(&tca_table, tc_table[j].sides);
                ei_sort_tca(tca_table);
            }

            ei_remove_from_tca(&tca_table, current_scanline_number);
            if (tca_table == NULL)
                break;

            if (j >= j_min)
                ei_fill_intervals(surface, tca_table, addr_pixel, &color, clipper);
            ei_update_tca(tca_table);
        }

        ei_free_tc_table(tc_table, tc_size);
    }
}


void ei_draw_text (ei_surface_t		    surface,
				   const ei_point_t*	where,
				   const char*		    text,
				   const ei_font_t	    font,
				   const ei_color_t*    color,
				   const ei_rect_t* 	clipper)
{
    ei_size_t size = hw_surface_get_size(surface);

    // surface intermediaire contient le text creer par la fct auxiliaire hw_text_create_surface
    ei_surface_t surface_interm = hw_text_create_surface(text,font,color);
    hw_surface_lock(surface_interm);

    int text_width, text_height;
    hw_text_compute_size(text,font,&text_width,&text_height);

    // maintenant il faut appliquer la fonction ei_copy_surface mais
    // et on a besoin de src_rect et dest_rect qui depend du clipper
    ei_rect_t dest_rect, src_rect;

    // cas ou clipper est NULL c'est facile
    if (clipper == NULL)
    {
        dest_rect = ei_rect(*where, ei_size(text_width, text_height));
        ei_copy_surface(surface, &dest_rect, surface_interm, NULL, EI_TRUE);
    }
    // le clipper est non nul, on calcule l'intersection du clipping et de la surface_intermediaire
    else
    {
        int x_min_c = (clipper->top_left).x,           y_min_c = (clipper->top_left).y;
        int x_max_c = x_min_c + (clipper->size).width, y_max_c = y_min_c + (clipper->size).height;

        int x_min_s = where->x,              y_min_s = where->y;
        int x_max_s = x_min_s + text_width,  y_max_s = y_min_s + text_height;

        // on verifie s'il y a une intersection entre le clipper et la surface
        if(x_min_s <= x_max_c && x_min_c <= x_max_s &&
           y_min_s <= y_max_c && y_min_c <= y_max_s)
        {
            // formons le rectangle src_dest inter clipping
            dest_rect.top_left = ei_point(max(x_min_c, x_min_s), max(y_min_c, y_min_s));
            int x_max = min(x_max_c, x_max_s) - dest_rect.top_left.x,
                y_max = min(y_max_c, y_max_s) - dest_rect.top_left.y;
            dest_rect.size = ei_size(min(x_max, size.width - x_max),
                                     min(y_max, size.height - y_max));

            //formons le rectangle src_rect
            src_rect = ei_rect(ei_point(dest_rect.top_left.x - x_min_s,
                                        dest_rect.top_left.y - y_min_s),
                               dest_rect.size);

            ei_copy_surface(surface, &dest_rect, surface_interm, &src_rect, EI_TRUE);
        }
    }

    hw_surface_unlock(surface_interm);
    hw_surface_free(surface_interm);
}


int ei_copy_surface(ei_surface_t destination,
                    const ei_rect_t *dst_rect,
                    const ei_surface_t source,
                    const ei_rect_t *src_rect,
                    const ei_bool_t alpha)
{
    // pointage vers le premier pixel de la surface source et màj size de src/dest
    uint32_t *addr_dst = (uint32_t *)hw_surface_get_buffer(destination);
    uint32_t *addr_src = (uint32_t *)hw_surface_get_buffer(source);

    // par défaut la surface est entiérement utilisée
    ei_size_t size_src = hw_surface_get_size(source);
    ei_size_t size_dst = hw_surface_get_size(destination);

    int width_window_src = size_src.width,
        width_window_dst = size_dst.width;

    if (src_rect != NULL)
    {
        size_src = src_rect->size;
        addr_src += src_rect->top_left.x + src_rect->top_left.y * width_window_src;
    }
    if (dst_rect != NULL)
    {
        size_dst = dst_rect->size;
        addr_dst += dst_rect->top_left.x + dst_rect->top_left.y * width_window_dst;
    }

    // dimensions finales de la surface dest et src
    int width_src = size_src.width, height_src = size_src.height,
        width_dst = size_dst.width, height_dst = size_dst.height;

    // required: égalité des surface de src et dest
    if (height_src != height_dst || width_src != width_dst)
        return EXIT_FAILURE;

    // Coloriage
    int i, j;
    int addr_dst_increment = width_window_dst - width_dst,
        addr_src_increment = width_window_src-width_src;
    if (alpha == EI_FALSE)
    { // les nouvelles couleurs vont "cacher les anciennes
        for (i = 0; i < height_dst; i++, addr_dst += addr_dst_increment,
                                         addr_src += addr_src_increment)
            for (j = 0; j < width_dst; j++, addr_dst++, addr_src++)
            {
                ei_color_t src_color = ei_unmap_color(source, *addr_src);
                *addr_dst = ei_map_rgba(destination, &src_color);
            }
    }
    else
    { // transparance
        for (i = 0; i < height_dst; i++, addr_dst += addr_dst_increment,
                                         addr_src += addr_src_increment)
            for (j = 0; j < width_dst; j++, addr_dst++, addr_src++)
            {
                ei_color_t dst_color = ei_unmap_color(destination, *addr_dst);
                ei_color_t src_color = ei_unmap_color(source, *addr_src);
                dst_color = ei_mixed_color(dst_color, &src_color);
                dst_color.alpha = 255;
                *addr_dst = ei_map_rgba(destination, &dst_color);
            }
    }

    return EXIT_SUCCESS;
}
