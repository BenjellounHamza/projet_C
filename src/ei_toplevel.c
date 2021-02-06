#include <stdlib.h>
#include <string.h>

#include "ei_widget.h"
#include "ei_toplevel.h"
#include "ei_widget_draw_utils.h"
#include "ei_placer_utils.h"


void * ei_toplevel_allocfunc()
{
    ei_toplevel_t *toplevel;
    toplevel = calloc(1, sizeof(ei_toplevel_t));
    return toplevel;
}


void ei_toplevel_releasefunc(struct ei_widget_t *widget)
{
    ei_toplevel_t *toplevel_ptr = (ei_toplevel_t *) widget;
    if (toplevel_ptr->min_size != NULL)
        free(toplevel_ptr->min_size);
}


void ei_toplevel_drawfunc(struct ei_widget_t *widget, ei_surface_t surface,
                          ei_surface_t pick_surface, ei_rect_t* clipper)
{
    ei_toplevel_t *toplevel_ptr = (ei_toplevel_t *) widget;
    ei_color_t dark_gray = { 100, 100, 100, 255 };

    ei_draw_rounded_frame(surface,
                          &(widget->screen_location),
                          0, 0, dark_gray,
                          clipper);

    ei_draw_rounded_frame(pick_surface,
                          &(widget->screen_location),
                          0, 0, *widget->pick_color,
                          clipper);

    ei_draw_rounded_frame(surface,
                          widget->content_rect,
                          0, 0, toplevel_ptr->color,
                          clipper);

    ei_point_t where = widget->screen_location.top_left;
    where.x += max(10, toplevel_ptr->border_width);

    if (toplevel_ptr->closable == EI_TRUE)
    {
        where.y += WINDOW_BAR / 2 - BUTTON_RADIUS;
        ei_rect_t button_rect = ei_rect(where, ei_size(2*BUTTON_RADIUS, 2*BUTTON_RADIUS));
        ei_color_t red = { 200, 0, 0, 255 };
        ei_draw_cadre(surface,
                      &button_rect,
                      ei_relief_sunken,
                      4,
                      BUTTON_RADIUS,
                      red,
                      clipper);
        where.x += 3 * BUTTON_RADIUS;
        where.y -= WINDOW_BAR / 2 - BUTTON_RADIUS;
    }

    ei_color_t white = { 255, 255, 255, 255 };
    ei_draw_text(surface,
                 &where,
                 toplevel_ptr->title,
                 ei_default_font,
                 &white,
                 clipper);

    ei_widget_t *current_child = widget->children_head;
    while (current_child != NULL)
    {
        if (current_child->placer_params != NULL)
            current_child->wclass->drawfunc(current_child, surface,
                    pick_surface, widget->content_rect);
        current_child = current_child->next_sibling;
    }


    if (toplevel_ptr->resizable != ei_axis_none)
    {
        where.x = widget->screen_location.top_left.x +
                  widget->screen_location.size.width -
                  (RESIZE_SIZE + toplevel_ptr->border_width);
        where.y = widget->screen_location.top_left.y +
                  widget->screen_location.size.height -
                  (RESIZE_SIZE + toplevel_ptr->border_width);
        ei_rect_t button_rect = ei_rect(where, ei_size(RESIZE_SIZE, RESIZE_SIZE));
        ei_draw_rounded_frame(surface, &button_rect, 0, 0, dark_gray, clipper);
        ei_draw_rounded_frame(pick_surface, &button_rect, 0, 0, *widget->pick_color, clipper);
    }
}


void ei_toplevel_setdefaultsfunc(struct ei_widget_t *widget)
{
    ei_toplevel_t *toplevel_ptr = (ei_toplevel_t *) widget;

    widget->requested_size = ei_size(320, 240);
    toplevel_ptr->color = ei_default_background_color;
    toplevel_ptr->border_width = 4;

    toplevel_ptr->title = malloc(32*sizeof(char));
    strcpy(toplevel_ptr->title, "Toplevel");

    toplevel_ptr->closable = EI_TRUE;
    toplevel_ptr->resizable = ei_axis_both;

    toplevel_ptr->min_size = malloc(sizeof(ei_size_t));
    *toplevel_ptr->min_size = ei_size(160, 120);
}


void ei_toplevel_geomnotifyfunc(struct ei_widget_t *widget, ei_rect_t rect)
{
    widget->screen_location = rect;

    ei_toplevel_t *toplevel_ptr = (ei_toplevel_t *) widget;


    if (widget->content_rect == NULL)
        widget->content_rect = malloc(sizeof(ei_rect_t));
    *(widget->content_rect) = widget->screen_location;

    int over_head = toplevel_ptr->border_width;
    widget->content_rect->size.width -= 2 * over_head;
    widget->content_rect->size.height -= over_head + WINDOW_BAR;
    widget->content_rect->top_left.x  += over_head;
    widget->content_rect->top_left.y  += WINDOW_BAR;

    ei_rect_t *container = widget->parent->content_rect;
    if (ei_rect_intersect(widget->content_rect, container))
        *(widget->content_rect) = ei_rect_intersection(widget->content_rect, container);
    else if (!ei_rect_contained(widget->content_rect, container))
        widget->content_rect->size = ei_size(0, 0);

    // ei_geomnotify_children(widget);
}


ei_bool_t ei_toplevel_handlefunc(struct ei_widget_t *widget, struct ei_event_t *event)
{
    return EI_FALSE;
}
