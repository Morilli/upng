#include <stdio.h>
#include <stdlib.h>

#include "upng.h"

#define HI(w) (((w) >> 8) & 0xFF)
#define LO(w) ((w) & 0xFF)

int main(int argc, char** argv) {
    FILE* fh;
    upng_t* upng;
    unsigned width, height, depth;
    unsigned x, y, d;

    if (argc != 3) {
        printf("Syntax: ./png2tga path/to/input.png path/to/output.tga\n");
        exit(EXIT_SUCCESS);
    }

    upng = upng_new_from_file(argv[1]);
    if (upng_get_error(upng) == UPNG_ENOTFOUND) {
        fprintf(stderr, "Error: Couldn't access input file \"%s\".\n", argv[1]);
        exit(EXIT_FAILURE);
    }
    upng_decode(upng);
    if (upng_get_error(upng) != UPNG_EOK) {
        printf("error: %u %u\n", upng_get_error(upng), upng_get_error_line(upng));
        exit(EXIT_FAILURE);
    }

    width = upng_get_width(upng);
    height = upng_get_height(upng);
    depth = upng_get_bpp(upng) / 8;
    printf("width: %d, height: %d, depth: %d\n", width, height, depth * 8);

    printf("size:	%ux%ux%u (%u)\n", width, height, upng_get_bpp(upng), upng_get_size(upng));
    printf("format:	%u\n", upng_get_format(upng));

    if (upng_get_format(upng) == UPNG_RGB8 || upng_get_format(upng) == UPNG_RGBA8) {
        fh = fopen(argv[2], "wb");
        if (!fh) {
            fprintf(stderr, "Error: Couldn't access output file \"%s\".\n", argv[2]);
            exit(EXIT_FAILURE);
        }

        fprintf(fh, "%c%c%c", 0, 0, 2);
        fprintf(fh, "%c%c%c%c%c", 0, 0, 0, 0, 0);
        fprintf(fh, "%c%c%c%c%c%c%c%c%c%c", 0, 0, 0, 0, LO(width), HI(width), LO(height), HI(height), upng_get_bpp(upng), upng_get_bitdepth(upng));

        if (upng_get_format(upng) == UPNG_RGBA8) {
            for (y = 0; y != height; ++y) {
                for (x = 0; x != width; ++x) {
                    for (d = 1; d != depth; ++d) {
                        // printf("accessing index %d\n", (height - y - 1) * width * depth + x * depth + (depth - d - 1));
                        putc(upng_get_buffer(upng)[(height - y - 1) * width * depth + x * depth + (depth - d - 1)], fh);
                    }
                    // printf("accessing index %d\n", (height - y - 1) * width * depth + x * depth + (depth - 1));
                    putc(upng_get_buffer(upng)[(height - y - 1) * width * depth + x * depth + (depth - 1)], fh);
                }
            }
        } else {
            for (y = 0; y != height; ++y) {
                for (x = 0; x != width; ++x) {
                    for (d = 0; d != depth; ++d) {
                        // printf("accessing index %d\n", (height - y - 1) * width * depth + x * depth + (depth - d - 1));
                        putc(upng_get_buffer(upng)[(height - y - 1) * width * depth + x * depth + (depth - d - 1)], fh);
                    }
                }
            }
        }

        fwrite("\x00\x00\x00\x00\x00\x00\x00\x00TRUEVISION-XFILE.", 1, 26, fh);

        fclose(fh);
    }

    upng_free(upng);
}
