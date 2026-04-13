#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image.h"
#include "stb_image_write.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

static unsigned char* box_blur_rgb(
    unsigned char *src, int width, int height,
    int channels, int radius)
{
    int size = width * height * channels;
    unsigned char *dst = malloc(size);
    if (!dst) return NULL;

    int kernel_size = 2 * radius + 1;
    int kernel_area = kernel_size * kernel_size;

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            for (int c = 0; c < channels; c++) {
                if (c == 3 && channels == 4) {
                    dst[(y * width + x) * channels + c] = src[(y * width + x) * channels + c];
                    continue;
                }

                int sum = 0;
                for (int ky = -radius; ky <= radius; ky++) {
                    int ny = y + ky;
                    if (ny < 0) ny = 0;
                    if (ny >= height) ny = height - 1;
                    for (int kx = -radius; kx <= radius; kx++) {
                        int nx = x + kx;
                        if (nx < 0) nx = 0;
                        if (nx >= width) nx = width - 1;
                        sum += src[(ny * width + nx) * channels + c];
                    }
                }

                dst[(y * width + x) * channels + c] = (unsigned char)((sum + kernel_area / 2) / kernel_area);
            }
        }
    }

    return dst;
}

int main() {
    const char *input_path = "input.png";
    const char *output_path = "output.png";

    int width, height, channels;
    unsigned char *src = stbi_load(input_path, &width, &height, &channels, 0);
    if (!src) {
        fprintf(stderr, "Gagal membaca file %s\n", input_path);
        return 1;
    }

    printf("Gambar sumber: %dx%d (%d channel)\n", width, height, channels);

    int blur_radius = 3;
    unsigned char *blurred = box_blur_rgb(src, width, height, channels, blur_radius);
    if (!blurred) {
        fprintf(stderr, "Gagal mengalokasikan memori untuk blur\n");
        stbi_image_free(src);
        return 1;
    }

    if (!stbi_write_png(output_path, width, height, channels, blurred, width * channels)) {
        fprintf(stderr, "Gagal menyimpan file %s\n", output_path);
        stbi_image_free(src);
        free(blurred);
        return 1;
    }

    printf("Gambar blur disimpan ke: %s\n", output_path);

    stbi_image_free(src);
    free(blurred);
    return 0;
}
