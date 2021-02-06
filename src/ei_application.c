#include <stdio.h>
#include <stdlib.h>

#include "ei_application.h"
#include "ei_frame.h"


ei_surface_t root_window, pick_surface;

ei_widget_t *root_widget;

ei_bool_t quit_request = EI_FALSE;

ei_widgetclass_t *linked_widgetclasses = NULL;


void ei_app_create(ei_size_t *main_window_size, ei_bool_t fullscreen)
{
    // initialisation de la couche graphique
    hw_init();

    // enregistrement des classes widgets disponibles
    ei_frame_register_class();
    ei_button_register_class();
    ei_toplevel_register_class();

    // creation de la fenetre principale de l'application
    root_window = hw_create_window(main_window_size, fullscreen);
    pick_surface = hw_surface_create(root_window, main_window_size, EI_FALSE);

    // mettre aux valeurs par defauts la attributs spécifiques à fra
    root_widget = (ei_widget_t*) ei_frame_allocfunc();
    ei_frame_setdefaultsfunc(root_widget);

    // configurer les attributs commun au wigets au vals nec pour frame
    root_widget->wclass = linked_widgetclasses;
    int id = 0;
    root_widget->pick_id = id;
    ei_color_t pick_color = {id>>16, id>>8, id, 255};
    root_widget->pick_color = malloc(sizeof(ei_color_t));
    *root_widget->pick_color = pick_color;

    root_widget->parent = NULL;
    root_widget->children_head = NULL;
    root_widget->children_tail = NULL;
    root_widget->next_sibling = NULL;

    root_widget->placer_params = NULL;
    root_widget->requested_size = hw_surface_get_size(root_window);
    root_widget->screen_location = hw_surface_get_rect(root_window);
    root_widget->content_rect = malloc(sizeof(ei_rect_t));
    *root_widget->content_rect = root_widget->screen_location;
}


void ei_app_free()
{
    /* on detruit recursivement les widgets*/
    ei_widget_destroy(root_widget);
    
    /* on libere les surfaces */
    hw_surface_free(root_window);
    hw_surface_free(pick_surface);

    /* on vide la bibliotheque cad la liste chainee linked_widgetclasses*/
    ei_widgetclass_t *current_class = linked_widgetclasses, *next_class;
    while (current_class != NULL)
    {
        next_class = current_class->next;
        free(current_class);
        current_class = next_class;
    }

    /* on quitte l'application*/
    hw_quit();
}


void ei_app_run()
{
    hw_surface_lock(root_window);
    hw_surface_lock(pick_surface);
    root_widget->wclass->drawfunc(root_widget, root_window, pick_surface, NULL);
    hw_surface_unlock(root_window);
    hw_surface_unlock(pick_surface);
    hw_surface_update_rects(root_window, NULL);
    hw_surface_update_rects(pick_surface, NULL);
    getchar();
}


void ei_app_invalidate_rect(ei_rect_t* rect)
{
}


void ei_app_quit_request()
{
    quit_request = EI_TRUE;
}


ei_widget_t* ei_app_root_widget()
{
    return root_widget;
}


ei_surface_t ei_app_root_surface()
{
    return root_window;
}
