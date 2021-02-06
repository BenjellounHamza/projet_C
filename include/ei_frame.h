#ifndef DEF_FRAME
#define DEF_FRAME

typedef struct ei_frame_t {
    ei_widget_t     widget;

    ei_color_t      color;
    int             border_width;
    ei_relief_t     relief;

    char            *text;
    ei_font_t       text_font;
    ei_color_t      text_color;
    ei_anchor_t     text_anchor;

    ei_surface_t    img;
    ei_rect_t*      img_rect;
    ei_anchor_t     img_anchor;
} ei_frame_t;


void * ei_frame_allocfunc();

void ei_frame_releasefunc(struct ei_widget_t* widget);

void ei_frame_setdefaultsfunc(struct ei_widget_t *widget);

void ei_frame_geomnotifyfunc(struct ei_widget_t *widget, ei_rect_t rect);

ei_bool_t ei_frame_handlefunc(struct ei_widget_t *widget, struct ei_event_t *event);

void ei_frame_drawfunc(struct ei_widget_t*	widget,
					   ei_surface_t		    surface,
					   ei_surface_t		    pick_surface,
					   ei_rect_t*		    clipper);

#endif
