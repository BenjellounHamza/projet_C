#include <stdlib.h>

#include "ei_widget.h"
#include "ei_frame.h"
#include "ei_widget_draw_utils.h"
#include "ei_placer_utils.h"


void * ei_frame_allocfunc()
{
    ei_frame_t *ptr_frame = NULL;
    ptr_frame = calloc(1, sizeof(ei_frame_t));
    return ptr_frame;
}


void ei_frame_releasefunc(struct ei_widget_t* widget)
{
    ei_frame_t *frame_ptr = (ei_frame_t *) widget;
    if (frame_ptr->img_rect != NULL)
        free(frame_ptr->img_rect);
}


void ei_frame_drawfunc(struct ei_widget_t*	widget,
					   ei_surface_t		    surface,
					   ei_surface_t		    pick_surface,
					   ei_rect_t*		    clipper)
{
    ei_frame_t *frame_ptr = (ei_frame_t *) widget;

    ei_draw_cadre(surface, &(widget->screen_location), frame_ptr->relief,
                  frame_ptr->border_width, 0, frame_ptr->color, clipper);

    ei_draw_cadre(pick_surface, &(widget->screen_location), ei_relief_none,
                  0, 0, *widget->pick_color, clipper);

    if (frame_ptr->text != NULL && frame_ptr->text_anchor != ei_anc_none)
    {
        ei_size_t text_size;
        hw_text_compute_size(frame_ptr->text, frame_ptr->text_font,
                             &(text_size.width), &(text_size.height));
        ei_point_t where = ei_calculate_where(widget->screen_location,
                                              &text_size,
                                              frame_ptr->text_anchor,
                                              frame_ptr->border_width);
        ei_draw_text(surface, &where, frame_ptr->text,
                     frame_ptr->text_font, &(frame_ptr->text_color), widget->content_rect);
    }
    else if (frame_ptr->img != NULL && frame_ptr->img_anchor != ei_anc_none)
    {
        ei_point_t where = ei_calculate_where(widget->screen_location,
                                              &(frame_ptr->img_rect->size),
                                              frame_ptr->text_anchor,
                                              frame_ptr->border_width);

        ei_rect_t dest_rect = ei_rect(where, frame_ptr->img_rect->size);
        ei_copy_surface(surface, &dest_rect, frame_ptr->img,
                        frame_ptr->img_rect, EI_TRUE);
    }

    ei_widget_t *current_child = widget->children_head;
    while (current_child != NULL)
    {
        if (current_child->placer_params != NULL)
            current_child->wclass->drawfunc(current_child, surface,
                    pick_surface, widget->content_rect);
        current_child = current_child->next_sibling;
    }
}


void ei_frame_setdefaultsfunc(struct ei_widget_t *widget)
{
    ei_frame_t *frame_ptr = (ei_frame_t *) widget;

    frame_ptr->color = ei_default_background_color;
    frame_ptr->border_width = 0;
    frame_ptr->relief = ei_relief_none;

    widget->requested_size = ei_size(0, 0);
    if (frame_ptr->text != NULL)
        hw_text_compute_size(frame_ptr->text, frame_ptr->text_font,
                             &(widget->requested_size.width),
                             &(widget->requested_size.height));
    else if (frame_ptr->img != NULL)
    {
        widget->requested_size = hw_surface_get_size(frame_ptr->img);
    }

    frame_ptr->text = NULL;
    frame_ptr->text_font = ei_default_font;
    frame_ptr->text_color = ei_font_default_color;
    frame_ptr->text_anchor = ei_anc_center;

    frame_ptr->img = NULL;
    frame_ptr->img_rect = NULL;
    frame_ptr->img_anchor = ei_anc_center;
}


void ei_frame_geomnotifyfunc(struct ei_widget_t *widget, ei_rect_t rect)
{
    widget->screen_location = rect;

    ei_frame_t *frame_ptr = (ei_frame_t *) widget;

    if (widget->content_rect == NULL)
        widget->content_rect = malloc(sizeof(ei_rect_t));
    *(widget->content_rect) = widget->screen_location;

    int border_width = frame_ptr->border_width;
    if (border_width != 0)
    {
        widget->content_rect->top_left.x  += border_width;
        widget->content_rect->top_left.y  += border_width;
        widget->content_rect->size.width  -= 2 * border_width;
        widget->content_rect->size.height -= 2 * border_width;
    }

    ei_rect_t *container = widget->parent->content_rect;
    if (ei_rect_intersect(widget->content_rect, container))
        *(widget->content_rect) = ei_rect_intersection(widget->content_rect, container);
    else if (!ei_rect_contained(widget->content_rect, container))
        widget->content_rect->size = ei_size(0, 0);

    //ei_geomnotify_children(widget);
}


ei_bool_t ei_frame_handlefunc(struct ei_widget_t *widget, struct ei_event_t *event)
{
    return EI_FALSE;
}
