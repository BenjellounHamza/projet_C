#include <stdio.h>
#include <stdlib.h>

#include "../src/ei_placer.c"


extern ei_widgetclass_t *linked_widgetclasses;


void print_anchor(ei_anchor_t anchor)
{
    switch (anchor)
    {
        case ei_anc_center: printf("Center\n"); break;
        case ei_anc_east: printf("East\n"); break;
        case ei_anc_none: printf("None\n"); break;
        case ei_anc_north: printf("North\n"); break;
        case ei_anc_northeast: printf("NorthEast\n"); break;
        case ei_anc_northwest: printf("NorthWest\n"); break;
        case ei_anc_south: printf("South\n"); break;
        case ei_anc_southeast: printf("SouthEast\n"); break;
        case ei_anc_southwest: printf("SouthWest\n"); break;
        case ei_anc_west: printf("West\n");
    }
}


void print_params(ei_placer_params_t *params)
{
    if (params == NULL)
        printf("NULL\n");
    else
    {
        printf("anchor: "); print_anchor(params->anchor_data);
        printf("x: %d, y: %d, rx: %.2f, ry: %.2f\n", params->x_data,
                params->y_data, params->rx_data, params->ry_data);
        printf("w: %d, h: %d, rw: %.2f, rh: %.2f\n", params->w_data,
                params->h_data, params->rw_data, params->rh_data);
    }
}


void print_point(ei_point_t *point)
{
    if (point == NULL)
        printf("NULL\n");
    else
        printf("x: %d, y: %d", point->x, point->y);
}


void print_size(ei_size_t *size)
{
    if (size == NULL)
        printf("NULL\n");
    else
        printf("width: %d, height: %d\n", size->width, size->height);
}


void print_rect(ei_rect_t *rect)
{
    if (rect == NULL)
        printf("NULL\n");
    else
    {
        print_point(&(rect->top_left));
        printf(" | ");
        print_size(&(rect->size));
    }

}


void print_text(char* text)
{
    if (text == NULL)
        printf("NULL\n");
    else
        printf("%s\n", text);
}


void print_fontstyle(ei_fontstyle_t fontstyle)
{
    switch (fontstyle)
    {
        case ei_style_bold: printf("style_bold\n"); break;
        case ei_style_normal: printf("style_normal\n"); break;
        case ei_style_italic: printf("style_italic\n"); break;
        case ei_style_underline: printf("style_underline\n"); break;
        case ei_style_strikethrough: printf("style_strikethrough\n"); break;
    }
}


void print_color(ei_color_t *color)
{
    if (color == NULL)
        printf("NULL\n");
    else
        printf("R=%d G=%d B=%d ALPHA=%d\n", color->red,
                color->green, color->blue, color->alpha);
}


void print_relief(ei_relief_t relief)
{
    switch (relief)
    {
        case ei_relief_none: printf("None\n"); break;
        case ei_relief_raised: printf("Releve\n"); break;
        case ei_relief_sunken: printf("Enfonce\n");
    }
}


void print_widget(ei_widget_t *widget)
{
    if (widget == NULL)
        printf("NULL\n");
    else
    {
        printf("address: %p\n", widget);
        printf("wclass: %s\n", (char*)widget->wclass->name);
        printf("pick_id: %d\n", widget->pick_id);
        printf("pick color: "); print_color(widget->pick_color);
        printf("parent: %p\n", widget->parent);
        printf("children_head: %p\n", widget->children_head);
        printf("children_tail: %p\n", widget->children_tail);
        printf("next_sibling: %p\n", widget->next_sibling);
        printf("placer_params:\n"); print_params(widget->placer_params);
        printf("requested_size: "); print_size(&(widget->requested_size));
        printf("screen_location:\n"); print_rect(&(widget->screen_location));
        printf("content_rect:\n"); print_rect(widget->content_rect);
    }
}


void print_frame(ei_frame_t *frame)
{
    if (frame == NULL)
        printf("NULL");
    else
    {
        print_widget((ei_widget_t *) frame);
        printf("color: "); print_color(&(frame->color));
        printf("border width: %d\n", frame->border_width);
        printf("relief: "); print_relief(frame->relief);
        printf("text: "); print_text(frame->text);
        if (frame->text != NULL)
        {
            printf("text_font: %p\n", frame->text_font);
            printf("text_color: "); print_color(&(frame->text_color));
            printf("text_anchor: "); print_anchor(frame->text_anchor);
        }
        printf("img: %p\n", frame->img);
        if (frame->img != NULL)
        {
            printf("img_rect:\n"); print_rect(frame->img_rect);
            printf("img_anchor:\n"); print_anchor(frame->img_anchor);
        }
    }
}


void print_button(ei_button_t *button)
{
    if (button == NULL)
        printf("NULL");
    else
    {
        print_widget((ei_widget_t *) button);
        printf("color: "); print_color(&(button->color));
        printf("border_width: %d\n", button->border_width);
        printf("corner_radius: %d\n", button->corner_radius);
        printf("relief: "); print_relief(button->relief);
        printf("text: "); print_text(button->text);
        if (button->text != NULL)
        {
            printf("text_font: %p\n", button->text_font);
            printf("text_color: "); print_color(&(button->text_color));
            printf("text_anchor: "); print_anchor(button->text_anchor);
        }
        printf("img: %p\n", button->img);
        if (button->img != NULL)
        {
            printf("img_rect:\n"); print_rect(button->img_rect);
            printf("img_anchor:\n"); print_anchor(button->img_anchor);
        }
    }
}


void print_widgetclasses()
{
    int counter = 0;
    printf("widget classes: ");
    if (linked_widgetclasses == NULL)
        printf("NULL");
    else
    {
        ei_widgetclass_t *current_class = linked_widgetclasses;
        while (current_class != NULL)
        {
            printf("%s ", (char *)(current_class->name));
            current_class = current_class->next;
            counter++;
        }
    }
    printf("\nwidget classes counter: %d\n", counter);
}


void print_widget_siblings(ei_widget_t *widget)
{
    if (widget == NULL)
        printf("ERROR: widget is NULL.\n");
    else
    {
        printf("widget %p's siblings: ", widget);
        ei_widget_t *parent = widget->parent;
        if (parent == NULL)
            printf("ERROR: widget has no parent.");
        else
        {
            ei_widget_t *current_sibling = parent->children_head;
            while (current_sibling != parent->children_tail)
            {
                printf("%p ", current_sibling);
                current_sibling = current_sibling->next_sibling;
            }
            printf("%p ", current_sibling);
        }
        printf("\n");
    }
}


void print_widget_children(ei_widget_t *widget)
{
    if (widget == NULL)
        printf("ERROR: widget is NULL.\n");
    else
    {
        printf("widget %p's children: ", widget);
        if (widget->children_head == NULL)
            printf("NULL");
        else
        {
            ei_widget_t *current_child = widget->children_head;
            while (current_child != widget->children_tail)
            {
                printf("%p ", current_child);
                current_child = current_child->next_sibling;
            }
            printf("%p ", current_child);
        }
        printf("\n");
    }
}


int widget_count(ei_widget_t *widget)
{
    static int counter = 0;
    if (widget != NULL)
    {
        counter++;
        ei_widget_t *current_chid = widget->children_head;
        while (current_chid != widget->children_tail)
        {
            widget_count(current_chid);
            current_chid = current_chid->next_sibling;
        }
        widget_count(current_chid);
    }
    return counter;
}


void print_widget_count(ei_widget_t *root)
{
    printf("number of widgets: %d\n", widget_count(root));
}



// ********* TESTS ********* //


void test_widgetclass_from_name()
{
    printf("\n((run test_widget_register first))\n");

    printf("\nfetching 'frame' from name:\n");
    ei_widgetclass_t *frame_class = ei_widgetclass_from_name("frame");
    printf("address: %p, name: %s\n", frame_class, (char*)(frame_class->name));

    printf("\nfetching 'zeb' from name:\n");
    ei_widgetclass_t *zeb_class = ei_widgetclass_from_name("zeb");
    printf("address: %p\n", zeb_class);

    printf("\nfetching 'toplevel' from name:\n");
    ei_widgetclass_t *toplevel_class = ei_widgetclass_from_name("toplevel");
    printf("address: %p, name: %s\n", toplevel_class, (char*)(toplevel_class->name));
}


void test_widget_register()
{
    print_widgetclasses();

    printf("\nregistering frame\n");
    ei_frame_register_class();
    print_widgetclasses();

    printf("\nregistering frame again\n");
    ei_frame_register_class();
    print_widgetclasses();

    printf("\nregistering button\n");
    ei_button_register_class();
    print_widgetclasses();

    printf("\nregistering toplevel\n");
    ei_toplevel_register_class();
    print_widgetclasses();

    printf("\nregistering button again\n");
    ei_button_register_class();
    print_widgetclasses();
}


ei_widget_t *create_root_widget(ei_surface_t *main_window)
{
    ei_widget_t *root_widget = malloc(sizeof(ei_widget_t));
    root_widget->wclass = NULL;
    root_widget->pick_id = 0;
    root_widget->pick_color = NULL;
    root_widget->parent = NULL;
    root_widget->children_head = NULL;
    root_widget->children_tail = NULL;
    root_widget->next_sibling = NULL;
    root_widget->placer_params = NULL;
    root_widget->requested_size = hw_surface_get_size(*main_window);
    root_widget->screen_location = ei_rect(ei_point(0, 0), root_widget->requested_size);
    root_widget->content_rect= &(root_widget->screen_location);
    return root_widget;
}


void test_widget_create()
{
    ei_frame_t *frame = (ei_frame_t *) ei_widget_create("frame", NULL);
    print_widget((ei_widget_t*) frame);
}
