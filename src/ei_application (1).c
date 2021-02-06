ei_surface_t root window;
ei_widget_t* root_widget = malloc(sizeof(ei_widget_t));
ei_surface_t pick_surface;
ei_linked_rect_t *rects_must_be_update = malloc(sizeof(ei_linked_rect_t));
void ei_app_create(ei_size_t* main_window_size, ei_bool_t fullscreen)
{
  hw_init();
  /* creer root_windows */
  root_window = hw_create_window(main_window_size,fullscreen);
  /* creer root_widget */
  /* on cree d'abord les classes frame button et toplevel*/
  ei_frame_register_class();
  ei_button_register_class();
  ei_toplevel_register_class();
  root_widget->wclass = ei_widgetclass_from_name("frame");
  root_widget->pick_id = ei_pick_id_generator(); /* initialiser à 255 cad alpha  = 255 er R G et B sont a 0*/
  root_widget->pick_color = ei_unmap_color(root_widget->pick_id);
  root_widget->parent = NULL;
  root_widget->children_head = NULL;
  root_widget->children_tail = NULL;
  root_widget->next_sibling = NULL;
  root_widget->placer_params = NULL;
  root_widget->requested_size = main_window_size;
  root_widget->screen_location = hw_surface_get_rect(root_window);
  root_widget->content_rect = &root_widget->screen_location;
  /* cree la surface de pick */
  pick_surface =  hw_surface_create(root_window,main_window_size,EI_TRUE);
}

void ei_app_free()
{
  /* on detruit recursivement les widgets*/
  ei_widget_destroy(root_widget);
  /* on vide la bibliotheque cad la liste chainee linked_widgetclasses*/
  ei_widgetclass_t current_class = linked_widgetclasses;
  while(current_class != NULL)
  {
    free(current_class);
    current_class = current_class->next;
  }
  /* on quitte l'application*/
  hw_quit();
}

void ei_app_run()
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

void ei_app_invalidate_rect(ei_rect_t* rect)
{
  ei_linked_rect_t* new_rect = malloc(sizeof(ei_linked_rect_t));
  new_rect->rect = *rect;
  /* ajout en tete */
  new_rect->next = rects_must_be_update;
  rects_must_be_update = new_rect;
}
