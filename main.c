#include <stdio.h>
#include <stdlib.h>

#define STB_IMAGE_IMPLEMENTATION
#include "include/stb_image.h"

#define STB_IMAGE_RESIZE_IMPLEMENTATION
#include "include/stb_image_resize2.h"

#define MAX_DIMENSION 500

/*
 * Grayscale formulas you can try:
 * (r+g+b)/3
 * 0.3r+0.59g+0.11b
 * max(rgb)
 * min(rgb)
 * (max(rgb)+min(rgb))/2
 */

const char *density = " _.,-=+:;cba!?0123456789$W#@N";
const int density_length = 29;

void print_colored_char(char c, int r, int g, int b) {
  printf("\033[38;2;%d;%d;%dm%c\033[0m", r, g, b, c);
}

int main(int argc, char **argv) {
  if (argc != 3) {
    printf("Usage: %s <image_filename> [color_mode]\n", argv[0]);
    printf("color_mode: 0 = grayscale (default), 1 = color\n");
    return 1;
  }

  int width, height, channels;
  char *filename = argv[1];
  int colored_mode = 0;

  unsigned char *image = stbi_load(filename, &width, &height, &channels, 0);
  if (image == NULL) {
    printf("Error in loading the image\n");
    return 1;
  }
  colored_mode = atoi(argv[2]);

  printf(
      "Loaded image with a width of %dpx, a height of %dpx and %d channels\n",
      width, height, channels);

  int new_width, new_height;
  if (width > height) {
    new_width = MAX_DIMENSION;
    new_height = (height * MAX_DIMENSION) / width;
  } else {
    new_height = MAX_DIMENSION;
    new_width = (width * MAX_DIMENSION) / height;
  }

  printf("Resizing to %dx%d to maintain aspect ratio\n", new_width, new_height);

  unsigned char *img = NULL;
  int free_img = 0;

  if (width != new_width || height != new_height) {
    img = (unsigned char *)malloc(new_width * new_height * channels);
    if (!img) {
      printf("Failed to allocate memory for resized image\n");
      stbi_image_free(image);
      return 1;
    }
    stbir_resize_uint8_linear(image, width, height, 0, img, new_width,
                              new_height, 0, channels);
    width = new_width;
    height = new_height;
    free_img = 1;
  } else {
    img = image;
  }

  printf("\e[1;1H\e[2J");

  for (int y = 0; y < height; y += 2) {
    for (int x = 0; x < width; x++) {
      int r = img[(y * width + x) * channels + 0];
      int g = img[(y * width + x) * channels + 1];
      int b = img[(y * width + x) * channels + 2];
      int gray = (r + g + b) / 3;
      int index = (gray * density_length) / 255;
      if (index >= density_length)
        index = density_length - 1;
      if (colored_mode) {
        print_colored_char(density[index], r, g, b);
      } else {
        printf("%c", density[index]);
      }
    }
    printf("\n");
  }

  stbi_image_free(image);
  if (free_img) {
    free(img);
  }

  return 0;
}
