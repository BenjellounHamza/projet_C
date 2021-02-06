#include "widgets_test.h"

ei_surface_t pick_surface;

// ----------- PRINTS ---------- //

void print_params(ei_placer_params_t *params);

void print_point(ei_point_t *point);

void print_size(ei_size_t *size);

void print_rect(ei_rect_t *rect);

void print_text(char* text);

void print_fontstyle(ei_fontstyle_t fontstyle);

void print_color(ei_color_t *color);

void print_relief(ei_relief_t relief);

void print_anchor(ei_anchor_t anchor);

void print_widget(ei_widget_t *widget);

void print_frame(ei_frame_t *frame);

void print_button(ei_button_t *button);

void print_widgetclasses();

void print_widget_siblings(ei_widget_t *widget);

void print_widget_children(ei_widget_t *widget);

int widget_count(ei_widget_t *widget);
void print_widget_count(ei_widget_t *root);


// ---------- TESTS --------- //

void test_widget_register();

void test_widgetclass_from_name();

ei_widget_t *create_root_widget(ei_surface_t *main_window);

// ------------------------- //


int ei_main(int argc, char** argv)
{
    /*printf("\ntesting widget_resiger\n"); getchar();
    test_widget_register();

    printf("\ntesting widgetclass_from_name\n"); getchar();
    test_widgetclass_from_name();*/

    // initialising //
    ei_frame_register_class();
    ei_button_register_class();
    ei_toplevel_register_class();
    ei_size_t win_size = ei_size(800, 600);
    ei_color_t white = { 0xff, 0xff, 0xff, 0xff };
    hw_init();
	ei_surface_t main_window = hw_create_window(&win_size, EI_FALSE);
    pick_surface = hw_surface_create(main_window, &win_size, EI_FALSE);
	hw_surface_lock(main_window);
    ei_fill(main_window, &white, NULL);
    // continue testing //

    // printf("\ncreating root widget:\n"); getchar();
    ei_widget_t *root_widget = create_root_widget(&main_window);
    // printf("root_widget:\n"); print_widget(root_widget);

    // test widget_create && setdefaultsfunc for frame
    // printf("\ncreating frame1 frame:\n"); getchar();
    // ei_button_t *button1 = (ei_button_t *) ei_widget_create("button", root_widget);
    // printf("button1 :\n"); print_button(button1);

    // printf("\nprinting number of widgets\n"); getchar();
    // print_widget_count(root_widget);

    // testing widgets hierachy
    // printf("\ncreating button1 button:\n"); getchar();
    ei_toplevel_t *toplevel1 = (ei_toplevel_t *) ei_widget_create("toplevel", root_widget);
    ei_button_t *button1_1 = (ei_button_t *) ei_widget_create("button", (ei_widget_t*)toplevel1);
    ei_button_t *button1_2 = (ei_button_t *) ei_widget_create("button", (ei_widget_t*)toplevel1);
    ei_button_t *button2 = (ei_button_t *) ei_widget_create("button", root_widget);
    ei_button_t *button2_1 = (ei_button_t *) ei_widget_create("button", (ei_widget_t*)button2);

    /*print_widget_children(root_widget);
    print_widget_children((ei_widget_t*) toplevel1);
    print_widget_children((ei_widget_t*) button2);*/

    // printf("\nprinting widgets ids:\n"); getchar();
    /*printf("root: %d, button1: %d, button2: %d, button3: %d, button4: %d,\
            button2_1: %d, button3_1: %d, button3_2: %d,\
            button4_1: %d, button4_2: %d, button4_3: %d\n",
            root_widget->pick_id, ((ei_widget_t*)button1)->pick_id,
            ((ei_widget_t*)button2)->pick_id, ((ei_widget_t*)button3)->pick_id,
            ((ei_widget_t*)button4)->pick_id, ((ei_widget_t*)button2_1)->pick_id,
            ((ei_widget_t*)button3_1)->pick_id, ((ei_widget_t*)button3_2)->pick_id,
            ((ei_widget_t*)button4_1)->pick_id, ((ei_widget_t*)button4_2)->pick_id,
            ((ei_widget_t*)button4_3)->pick_id);*/

    // test button configure
    ei_color_t blue = { 0, 0, 200, 255 }, black = { 0, 0, 0, 255 },
               red = { 200, 0, 0, 255 }, green = { 0, 200, 0, 255 },
               dark_white = { 230, 230, 230, 255 };
    ei_relief_t relief = ei_relief_none; int corner_radius = 10;

    int border_width = 4; char *text1 = "LOLOLOLLOL"; ei_bool_t closable = EI_TRUE;
    ei_size_t requested_size = ei_size(500, 200); ei_axis_set_t resizable = ei_axis_both;
    ei_toplevel_configure((ei_widget_t*)toplevel1, &requested_size, &dark_white,
                        &border_width, &text1, &closable, &resizable, NULL);
    float rel_x = 1.0, rel_y = 1.0;
    int x = -10, y = -10; ei_anchor_t anchor = ei_anc_southeast;
    ei_place((ei_widget_t *)toplevel1, &anchor, &x, &y, NULL, NULL, &rel_x, &rel_y, NULL, NULL);
    ei_placer_run((ei_widget_t *)toplevel1);

    relief = ei_relief_raised; char *text11 = "CHILD 1";
    ei_anchor_t text_anchor = ei_anc_southwest; requested_size = ei_size(100, 20);
    ei_button_configure((ei_widget_t*)button1_1, NULL, &blue,
                        &border_width, NULL, &relief, &text11, NULL, &red, &text_anchor, NULL,
                        NULL, NULL, NULL, NULL);
    rel_x = 0.5; rel_y = 0.5; x = 0; y = -10; anchor = ei_anc_northwest;
    ei_place((ei_widget_t *)button1_1, &anchor, &x, &y, NULL, NULL, &rel_x, &rel_y, NULL, NULL);
    ei_placer_run((ei_widget_t *)button1_1);

    border_width = 5; relief = ei_relief_sunken; char *text12 = "CHILD 2";
    text_anchor = ei_anc_east;
    ei_button_configure((ei_widget_t*)button1_2, NULL, &green,
                        &border_width, NULL, &relief, &text12, NULL, &black, &text_anchor,
                        NULL, NULL, NULL, NULL, NULL);
    rel_x = 0.8; rel_y = 1.0; int w = 100, h = 100; anchor = ei_anc_southwest;
    ei_place((ei_widget_t *)button1_2, &anchor, NULL, NULL, &w, &h, &rel_x, &rel_y, NULL, NULL);
    ei_placer_run((ei_widget_t *)button1_2);

    border_width = 5; char *text2 = "BROTHER"; text_anchor = ei_anc_northwest;
    ei_button_configure((ei_widget_t*)button2, NULL, &ei_default_background_color,
                        &border_width, &corner_radius, &relief, &text2, NULL, &white,
                        &text_anchor, NULL, NULL, NULL, NULL, NULL);
    x = 10; y = 10; w = 200; h = 300; anchor = ei_anc_northwest;
    ei_place((ei_widget_t *)button2, &anchor, &x, &y, &w, &h, NULL, NULL, NULL, NULL);
    ei_placer_run((ei_widget_t *)button2);

    relief = ei_relief_raised;
    ei_button_configure((ei_widget_t*)button2_1, NULL, &red,
                        NULL, NULL, &relief, NULL, NULL, NULL, NULL, NULL,
                        NULL, NULL, NULL, NULL);
    anchor = ei_anc_northwest;
    ei_place((ei_widget_t *)button2_1, &anchor, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
    ei_placer_run((ei_widget_t *)button2_1);

    border_width = 5; closable = EI_TRUE; char *textx = "NEW WINDOW";
    ei_toplevel_configure((ei_widget_t*)toplevel1, NULL, NULL,
                          &border_width, &textx, &closable, NULL, NULL);
    rel_x = 1.0; rel_y = 0.0; x = -10, y = 10; anchor = ei_anc_northeast;
    float rel_width = 0.5, rel_height = 0.5;
    ei_place((ei_widget_t *)toplevel1, &anchor, &x, &y, NULL, NULL, &rel_x, &rel_y, &rel_width, &rel_height);
    ei_placer_run((ei_widget_t *)toplevel1);


    printf("\ntoplevel :\n"); print_widget((ei_widget_t*)toplevel1);
    printf("\nbutton1_1 :\n"); print_button(button1_1);
    printf("\nbutton1_2 :\n"); print_button(button1_2);
    // printf("\nbutton2 :\n"); print_button(button2);

    // test ei_button_drawfunc
    printf("\ndrawing the button on the screen\n");
    ei_widget_t *toplevel1_widget = (ei_widget_t *) toplevel1;
    ei_widget_t *button1_1_widget = (ei_widget_t *) button1_1;
    ei_widget_t *button1_2_widget = (ei_widget_t *) button1_2;
    ei_widget_t *button2_widget = (ei_widget_t *) button2;
    ei_widget_t *button2_1_widget = (ei_widget_t *) button2_1;
    toplevel1_widget->wclass->drawfunc(toplevel1_widget, pick_surface, main_window, toplevel1_widget->parent->content_rect);
    //button1_1_widget->wclass->drawfunc(button1_1_widget, main_window, main_window, button1_1_widget->parent->content_rect);
    //button1_2_widget->wclass->drawfunc(button1_2_widget, main_window, main_window, button1_2_widget->parent->content_rect);
    //button2_widget->wclass->drawfunc(button2_widget, main_window, main_window, button2_widget->parent->content_rect);
    //button2_1_widget->wclass->drawfunc(button2_1_widget, main_window, main_window, button2_1_widget->parent->content_rect);
    // done testing //

    hw_surface_unlock(main_window);
    hw_surface_update_rects(main_window, NULL);
    getchar();
    hw_quit();

	return EXIT_SUCCESS;
}
