#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ei_application.h"
#include "ei_utils.h"
#include "ei_frame.h"
#include "ei_button.h"
#include "ei_toplevel.h"
#define WINDOW_BAR    30


extern ei_widgetclass_t *linked_widgetclasses;

extern ei_surface_t pick_surface;


/* verifie si class_name existe dans la bibliotheque */
ei_widgetclass_t* ei_widgetclass_from_name(ei_widgetclass_name_t name)
{
    ei_widgetclass_t* current_widgetclass = linked_widgetclasses;

    while (current_widgetclass != NULL)
    {
        if (!strcmp(ei_widgetclass_stringname(current_widgetclass->name),
                    ei_widgetclass_stringname(name)))
          return current_widgetclass;

        current_widgetclass = current_widgetclass->next;
    }

    return NULL;
}


/* generates up to 180k different numbers */
int ei_pick_id_generator()
{
    static int number = 0;
    static int i = 0;
    number = (number + (123456 << (i % 10)) + 0xFF) % 8000000;
    i++;
    return number;
}


ei_widget_t* ei_widget_create(ei_widgetclass_name_t	class_name,
                              ei_widget_t *parent)
{
    ei_widgetclass_t *widgetclass = ei_widgetclass_from_name(class_name);

    if (widgetclass == NULL)
    {
        printf("ERROR: Class not found.\n");
        return NULL;
    }

    /* allocation de la memoire necessaire */
    ei_widget_t *new_widget = widgetclass->allocfunc();
    new_widget->wclass = widgetclass;

    int id = ei_pick_id_generator();
    new_widget->pick_id = id;
    ei_color_t pick_color = {id>>16, id>>8, id, 255};
    new_widget->pick_color = malloc(sizeof(ei_color_t));
    *new_widget->pick_color = pick_color;

    /*  Widget Hierachy Management */
    new_widget->parent = parent;
    new_widget->next_sibling = NULL;

    /*on a ajoute une nouvelle widget a 'parent' donc il faut modifier la
     * queue de la liste chainee du widget parent (la queue ou la tete
     * dans un cas tres particulier ou la liste des fils est NULL) */
    if (parent->children_head == NULL)
    {
        /* cas particulier ou la liste des fils du parent est NULL*/
        parent->children_head = new_widget;
        parent->children_tail = new_widget;
    }
    else
    {
        parent->children_tail->next_sibling = new_widget;
        parent->children_tail = new_widget;
    }

    /* Geometry Management */
    new_widget->placer_params = NULL;

    /* metre les parametres du widget par defaut */
    widgetclass->setdefaultsfunc(new_widget);

    return new_widget;
}


void ei_release_widget(struct ei_widget_t *widget)
{
    if (widget->pick_color != NULL)
        free(widget->pick_color);
    if (widget->placer_params != NULL)
        free(widget->placer_params);
    if (widget->content_rect != NULL)
        free(widget->content_rect);
}


/* la difference entre cette procedure et widget_destroy est que ici
 * on ne s'interesse pas a modifier les fils du parents pour garder
 * les autres freres, ici on detruit tout */
void ei_destroy_widget_and_children(ei_widget_t *widget)
{
    ei_widget_t *current_child = widget->children_head,
                *next_child;
    // destruction avec parcours en profondeur
    while (current_child != NULL)
    {
        next_child = current_child->next_sibling;
        ei_destroy_widget_and_children(current_child);
        current_child = next_child;
    }
    ei_release_widget(widget);
    widget->wclass->releasefunc(widget);
    //free(widget);
}


/* detruit le widget et l'enleve de l ecran, detruit tous ses descandants */
void ei_widget_destroy(ei_widget_t *widget)
{
    if (widget != NULL)
    {
        ei_widget_t *parent = widget->parent;
        // si c'est la racine.
        if (parent != NULL)
        {
            if (parent->children_head == widget)
            {
                if (parent->children_tail == widget)
                    parent->children_tail = NULL;
                parent->children_head = widget->next_sibling;
            }
            // on cherche le widget frére ainé immedi
            else
            {
                ei_widget_t *previous = parent->children_head; // not NULL

                while (previous->next_sibling != widget)
                    previous = previous->next_sibling;
                previous->next_sibling = widget->next_sibling;

                if (widget == parent->children_tail)
                    parent->children_tail = previous;
            }
        }

        ei_destroy_widget_and_children(widget);
    }
    else
        printf("ERROR: widget is either root or NULL.\n");
}


ei_bool_t in_rect(const ei_rect_t *rect, const ei_point_t point)
{
    if (point.x < rect->top_left.x + rect->size.width
        && rect->top_left.x < point.x)
        if (point.y < rect->top_left.y + rect->size.height
            && rect->top_left.y < point.y)
            return EI_TRUE;
    return EI_FALSE;
}


// il me faut une surface et une widget racine !!!!
ei_widget_t *ei_widget_pick(ei_point_t *where)
{
    ei_widget_t* root_widget = ei_app_root_widget();

    uint32_t *addr_deb = (uint32_t *)hw_surface_get_buffer(pick_surface);
    int width_surface =  hw_surface_get_size(pick_surface).width;


    uint32_t *addr_pixel = addr_deb + where->x + where->y * width_surface;
    ei_widget_t* courant_widget = root_widget;

    if (courant_widget != NULL) {
       if (courant_widget->pick_id == *addr_pixel) {
            if (courant_widget != root_widget)
                    return courant_widget;
              else {
                  return NULL;
                }
        }
        ei_bool_t same_rect = in_rect(courant_widget->content_rect, *where);
        while (courant_widget != NULL){ 
              while (same_rect) { // essayer pour le widget et tous ses fils
                   if (courant_widget->pick_id == *addr_pixel) {
                       if (courant_widget != root_widget){
                           return courant_widget;
                        }
                        else {
                            return NULL;
                        }
                    } else {
                        courant_widget = courant_widget->children_head;
                        same_rect = in_rect(courant_widget->content_rect, *where);
                    }
                } // sinon checker pour les freres
                courant_widget = courant_widget->next_sibling;
                same_rect = in_rect(courant_widget->content_rect, *where);
        }
    }
    return NULL;
}


void ei_frame_configure(ei_widget_t * widget, ei_size_t * requested_size,
                        const ei_color_t *color, int *border_width,
                        ei_relief_t *relief, char **text, ei_font_t *text_font,
                        ei_color_t *text_color, ei_anchor_t *text_anchor,
                        ei_surface_t *img,  ei_rect_t **img_rect,
                        ei_anchor_t *img_anchor)
{
    ei_frame_t *frame_ptr = (ei_frame_t *) widget;

    if (border_width != NULL)
        frame_ptr->border_width = *border_width;
    if (requested_size != NULL)
    {
        widget->requested_size = *requested_size;
        widget->requested_size.width  += 2 * frame_ptr->border_width;
        widget->requested_size.height += 2 * frame_ptr->border_width;
    }
    if (color != NULL)
        frame_ptr->color = *color;
    if (relief != NULL)
        frame_ptr->relief = *relief;
    if (text_font != NULL)
        frame_ptr->text_font = *text_font;
    if (text_anchor != NULL)
        frame_ptr->text_anchor = *text_anchor;
    if (text_color != NULL)
        frame_ptr->text_color = *text_color;
    if (img_rect != NULL)
        frame_ptr->img_rect = *img_rect;
    if (img_anchor != NULL)
        frame_ptr->img_anchor = *img_anchor;
    if (text != NULL)
    {
        if (requested_size == NULL)
        {
            hw_text_compute_size(*text, frame_ptr->text_font,
                                 &(widget->requested_size.width),
                                 &(widget->requested_size.height));
            widget->requested_size.width  += 2 * frame_ptr->border_width;
            widget->requested_size.height += 2 * frame_ptr->border_width;
        }
        frame_ptr->text = *text;
    }
    else if (img != NULL)
    {
        if (requested_size == NULL)
        {
            widget->requested_size = hw_surface_get_size(*img);
            widget->requested_size.width  += 2 * frame_ptr->border_width;
            widget->requested_size.height += 2 * frame_ptr->border_width;
        }
        frame_ptr->img = *img;
    }

    if (widget->placer_params != NULL &&
        (requested_size != NULL || text != NULL
         || img != NULL || border_width != NULL))
    {
            ei_point_t new_top_left = widget->content_rect->top_left;
            new_top_left.x -= frame_ptr->border_width;
            new_top_left.y -= frame_ptr->border_width;
            widget->wclass->geomnotifyfunc(widget, ei_rect(new_top_left,
                                                           widget->requested_size));
    }
}


void ei_button_configure(ei_widget_t * widget, ei_size_t * requested_size,
                         const ei_color_t *color, int *border_width,
                         int *corner_radius, ei_relief_t *relief, char **text,
                         ei_font_t *text_font, ei_color_t *text_color,
                         ei_anchor_t *text_anchor, ei_surface_t *img,
                         ei_rect_t **img_rect, ei_anchor_t *img_anchor,
                         ei_callback_t *callback, void **user_param)
{
    ei_button_t *button_ptr = (ei_button_t *) widget;

    if (border_width != NULL)
        button_ptr->border_width = *border_width;
    if (requested_size != NULL)
    {
        widget->requested_size = *requested_size;
        widget->requested_size.width  += 2 * button_ptr->border_width;
        widget->requested_size.height += 2 * button_ptr->border_width;
    }
    if (color != NULL)
        button_ptr->color = *color;
    if (relief != NULL)
        button_ptr->relief = *relief;
    if (text_font != NULL)
        button_ptr->text_font = *text_font;
    if (text_anchor != NULL)
        button_ptr->text_anchor = *text_anchor;
    if (text_color != NULL)
        button_ptr->text_color = *text_color;
    if (img_rect != NULL)
        button_ptr->img_rect = *img_rect;
    if (img_anchor != NULL)
        button_ptr->img_anchor = *img_anchor;
    if (corner_radius != NULL)
        button_ptr->corner_radius = *corner_radius;
    if (callback != NULL)
        button_ptr->callback = *callback;
    if (user_param != NULL)
        button_ptr->user_param = *user_param;
    if (text != NULL)
    {
        if (requested_size == NULL)
        {
            hw_text_compute_size(*text, button_ptr->text_font,
                                 &(widget->requested_size.width),
                                 &(widget->requested_size.height));
            int over_head = button_ptr->border_width;
            widget->requested_size.width  += 2 * over_head;
            widget->requested_size.height  += 2 * over_head;
        }
        button_ptr->text = *text;
    }
    else if (img != NULL)
    {
        if (requested_size == NULL)
        {
            widget->requested_size = hw_surface_get_size(*img);
            int over_head = button_ptr->border_width;
            widget->requested_size.width  += 2 * over_head;
            widget->requested_size.height  += 2 * over_head;
        }
        button_ptr->img = *img;
    }

    if (widget->placer_params != NULL && 
        (requested_size != NULL || text != NULL
         || img != NULL || border_width != NULL))
    {
            ei_point_t new_top_left = widget->content_rect->top_left;
            new_top_left.x -= button_ptr->border_width;
            new_top_left.y -= button_ptr->border_width;
            widget->wclass->geomnotifyfunc(widget, ei_rect(new_top_left,
                                                           widget->requested_size));
    }
}


void ei_toplevel_configure(ei_widget_t * widget, ei_size_t * requested_size,
                           ei_color_t * color, int *border_width,
                           char **title, ei_bool_t *closable,
                           ei_axis_set_t *resizable, ei_size_t **min_size)
{
    ei_toplevel_t *toplevel_ptr = (ei_toplevel_t *) widget;

    if (border_width != NULL)
        toplevel_ptr->border_width = *border_width;
    if (requested_size != NULL)
    {
        widget->requested_size = *requested_size;
        widget->requested_size.width  += 2 * toplevel_ptr->border_width;
        widget->requested_size.height += toplevel_ptr->border_width + WINDOW_BAR;
    }
    if (color != NULL)
        toplevel_ptr->color = *color;
    if (title != NULL)
        toplevel_ptr->title = *title;
    if (closable != NULL)
        toplevel_ptr->closable = *closable;
    if (resizable != NULL)
        toplevel_ptr->resizable = *resizable;
    if (min_size != NULL)
        toplevel_ptr->min_size = *min_size;

    if (widget->placer_params != NULL && 
        (requested_size != NULL || border_width != NULL
         || min_size != NULL))
    {
            ei_point_t new_top_left = widget->content_rect->top_left;
            new_top_left.x -= toplevel_ptr->border_width;
            new_top_left.y -= WINDOW_BAR;
            widget->wclass->geomnotifyfunc(widget, ei_rect(new_top_left,
                                                           widget->requested_size));
    }
}
