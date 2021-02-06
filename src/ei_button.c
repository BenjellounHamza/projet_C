#include <stdlib.h>

#include "ei_widget.h"
#include "ei_button.h"
#include "ei_widget_draw_utils.h"
#include "ei_placer_utils.h"


void * ei_button_allocfunc()
{
    ei_button_t *button = NULL;
    button = calloc(1, sizeof(ei_button_t));
    return button;
}


void ei_button_releasefunc(struct ei_widget_t *widget)
{
    ei_button_t *button_ptr = (ei_button_t *) widget;
    if (button_ptr->img_rect != NULL)
        free(button_ptr->img_rect);
    if (button_ptr->user_param != NULL)
        free(button_ptr->user_param);
}


void ei_button_drawfunc(struct ei_widget_t *widget, ei_surface_t surface,
                        ei_surface_t pick_surface, ei_rect_t* clipper)
{
    ei_button_t *button_ptr = (ei_button_t *) widget;

    ei_draw_cadre(surface, &(widget->screen_location), button_ptr->relief,
                  button_ptr->border_width, button_ptr->corner_radius,
                  button_ptr->color, clipper);

    ei_draw_cadre(pick_surface, &(widget->screen_location), ei_relief_none,
                  0, button_ptr->corner_radius, *widget->pick_color, clipper);

    if (button_ptr->text != NULL && button_ptr->text_anchor != ei_anc_none)
    {
        ei_size_t text_size;
        hw_text_compute_size(button_ptr->text, button_ptr->text_font,
                             &(text_size.width), &(text_size.height));

        int over_head = button_ptr->border_width;
        ei_point_t where = ei_calculate_where(widget->screen_location,
                                              &text_size,
                                              button_ptr->text_anchor,
                                              over_head);

        ei_draw_text(surface, &where, button_ptr->text, button_ptr->text_font,
                     &(button_ptr->text_color), widget->content_rect);
    }
    else if (button_ptr->img != NULL && button_ptr->img_anchor != ei_anc_none)
    {
        int over_head = button_ptr->border_width;
        ei_point_t where = ei_calculate_where(widget->screen_location,
                                              &(button_ptr->img_rect->size),
                                              button_ptr->text_anchor,
                                              over_head);

        ei_rect_t dest_rect = ei_rect(where, button_ptr->img_rect->size);
        ei_copy_surface(surface, &dest_rect, button_ptr->img,
                        button_ptr->img_rect, EI_TRUE);
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


void ei_button_setdefaultsfunc(struct ei_widget_t *widget)
{
    ei_button_t *button_ptr = (ei_button_t *) widget;

    button_ptr->border_width = k_default_button_border_width;
    button_ptr->corner_radius = k_default_button_corner_radius;
    button_ptr->color = ei_default_background_color;
    button_ptr->relief = ei_relief_raised;

    widget->requested_size = ei_size(0, 0);
    if (button_ptr->text != NULL)
      hw_text_compute_size(button_ptr->text, button_ptr->text_font,
                           &(widget->requested_size.width),
                           &(widget->requested_size.height));
    else if (button_ptr->img != NULL)
    {
      widget->requested_size = hw_surface_get_size(button_ptr->img);
    }
    int over_head = max(button_ptr->border_width, button_ptr->corner_radius);
    widget->requested_size.width  += 2 * over_head;
    widget->requested_size.height += 2 * over_head;

    button_ptr->text = NULL;
    button_ptr->text_font = ei_default_font;
    button_ptr->text_color = ei_font_default_color;
    button_ptr->text_anchor = ei_anc_center;

    button_ptr->img = NULL;
    button_ptr->img_rect = NULL;
    button_ptr->img_anchor = ei_anc_center;

    button_ptr->callback = NULL;
    button_ptr->user_param = NULL;
}


void ei_button_geomnotifyfunc(struct ei_widget_t *widget, ei_rect_t rect)
{
    widget->screen_location = rect;

    ei_button_t *button_ptr = (ei_button_t *) widget;

    if (widget->content_rect == NULL)
        widget->content_rect = malloc(sizeof(ei_rect_t));
    *(widget->content_rect) = widget->screen_location;

    int over_head = button_ptr->border_width;
    if (over_head != 0)
    {
        widget->content_rect->top_left.x  += over_head;
        widget->content_rect->top_left.y  += over_head;
        widget->content_rect->size.width  -= 2 * over_head;
        widget->content_rect->size.height -= 2 * over_head;
    }

    ei_rect_t *container = widget->parent->content_rect;
    if (ei_rect_intersect(widget->content_rect, container))
        *(widget->content_rect) = ei_rect_intersection(widget->content_rect, container);
    else if (!ei_rect_contained(widget->content_rect, container))
        widget->content_rect->size = ei_size(0, 0);

    //ei_geomnotify_children(widget);
}


ei_bool_t ei_button_handlefunc(struct ei_widget_t *widget, struct ei_event_t *event)
{
    return EI_FALSE;
}
