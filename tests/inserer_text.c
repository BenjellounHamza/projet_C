#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "../include/ei_draw.h"
#include "../include/ei_utils.h"

void test_insertion_texte(const char *text, ei_rect_t *clipper);

int ei_main(int argc, char **argv) {
  printf("on ecrit segmentation fault dans une surface blanche sans \
            utiliser de clipper\n");
  test_insertion_texte("Segmentation fault", NULL);

  printf("on ecrit enfin pas de segmentation fault dans une surface \
            blanche en utiliser de clipper\n");
  ei_rect_t clipper = ei_rect(ei_point(380, 395), ei_size(100, 50));
  test_insertion_texte("Enfin pas de segmentation fault", &clipper);

  return EXIT_SUCCESS;
}

void test_insertion_texte(const char *text, ei_rect_t *clipper) {
  ei_size_t win_size = ei_size(800, 800);
  ei_surface_t main_window = NULL;

  ei_color_t blue = {0x00, 0x00, 0xff, 0xff};
  ei_color_t red = {0xff, 0x00, 0x00, 0xff};

  ei_rect_t *clipper_ptr = NULL;

  hw_init();

  main_window = hw_create_window(&win_size, EI_FALSE);

  /* Lock the drawing surface, paint it white. */
  hw_surface_lock(main_window);
  ei_fill(main_window, &red, clipper_ptr);

  /* insertion texte */
  ei_point_t where = {380, 395};
  ei_draw_text(main_window, &where, text, ei_default_font, &blue, clipper);

  /* Unlock and update the surface. */
  hw_surface_unlock(main_window);
  hw_surface_update_rects(main_window, NULL);

  getchar();
  hw_quit();
}
