#ifndef DEF_TOPLEVEL
#define DEF_TOPLEVEL

typedef struct ei_toplevel_t {
    ei_widget_t     widget;
    ei_color_t		color;
    int			    border_width;
    char*			title;
    ei_bool_t		closable;
    ei_axis_set_t	resizable;
    ei_size_t*		min_size;
} ei_toplevel_t;


void * ei_toplevel_allocfunc();

void ei_toplevel_releasefunc(struct ei_widget_t *widget);

void ei_toplevel_setdefaultsfunc(struct ei_widget_t *widget);

void ei_toplevel_geomnotifyfunc(struct ei_widget_t *widget, ei_rect_t rect);

ei_bool_t ei_toplevel_handlefunc(struct ei_widget_t *widget, struct ei_event_t *event);

void ei_toplevel_drawfunc(struct ei_widget_t *widget,
                          ei_surface_t surface,
                          ei_surface_t pick_surface,
                          ei_rect_t* clipper);

#endif
