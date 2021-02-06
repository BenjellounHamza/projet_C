#include <stdlib.h>

#include "ei_widget.h"
#include "ei_placer_utils.h"


void ei_place(struct ei_widget_t* widget,
              ei_anchor_t*	anchor,
              int*			x,
              int*			y,
              int*			width,
              int*			height,
              float*		rel_x,
              float*		rel_y,
              float*		rel_width,
              float*		rel_height)
{
    ei_placer_params_t* new_placer;

    if (widget->placer_params != NULL)
        new_placer = widget->placer_params;
    else
        new_placer = calloc(1, sizeof(ei_placer_params_t));

    if (anchor != NULL)
        new_placer->anchor_data = *anchor;
    else if (new_placer->anchor == NULL)
        new_placer->anchor_data = ei_anc_northwest;
    new_placer->anchor = &(new_placer->anchor_data);

    ei_placer_set_int_param(new_placer, x, &(new_placer->x), &(new_placer->x_data));
    ei_placer_set_int_param(new_placer, y, &(new_placer->y), &(new_placer->y_data));
    ei_placer_set_int_param(new_placer, width, &(new_placer->w), &(new_placer->w_data));
    ei_placer_set_int_param(new_placer, height, &(new_placer->h), &(new_placer->h_data));
    ei_placer_set_float_param(new_placer, rel_x, &(new_placer->rx), &(new_placer->rx_data));
    ei_placer_set_float_param(new_placer, rel_y, &(new_placer->ry), &(new_placer->ry_data));
    ei_placer_set_float_param(new_placer, rel_width, &(new_placer->rw), &(new_placer->rw_data));
    ei_placer_set_float_param(new_placer, rel_height, &(new_placer->rh), &(new_placer->rh_data));

    widget->placer_params = new_placer;

    ei_placer_run(widget);
}


void ei_placer_run(struct ei_widget_t* widget)
{
    ei_placer_params_t *placer_params = widget->placer_params;

    if (widget != NULL && placer_params != NULL)
    {
        ei_rect_t new_location;
        ei_rect_t *container  = widget->parent->content_rect;

        if (placer_params->w_data != 0 || placer_params->rw_data != 0.0)
            new_location.size.width  = placer_params->w_data +
                                       container->size.width * placer_params->rw_data;
        else
            new_location.size.width = widget->requested_size.width;

        if (placer_params->h_data != 0 || placer_params->rh_data != 0.0)
            new_location.size.height = placer_params->h_data +
                                       container->size.height * placer_params->rh_data;
        else
            new_location.size.height = widget->requested_size.height;

        ei_point_t anchor_point;
        anchor_point.x = container->top_left.x + placer_params->x_data +
                         container->size.width * placer_params->rx_data;
        anchor_point.y = container->top_left.y + placer_params->y_data +
                         container->size.height * placer_params->ry_data;

        ei_calculate_topleft(&new_location, &anchor_point, placer_params->anchor_data);

        widget->wclass->geomnotifyfunc(widget, new_location);
    }
}


void ei_placer_forget(struct ei_widget_t *widget)
{
    if (widget != NULL)
    {
        if (widget->placer_params != NULL)
        {
            free(widget->placer_params);
            widget->placer_params = NULL;
        }

        ei_widget_t *widget_child = widget->children_head;
        while (widget_child != widget->children_tail)
        {
            ei_placer_forget(widget_child);
            widget_child = widget_child->next_sibling;
        }
        ei_placer_forget(widget_child);
    }
}
