#include <stdint.h>
#define NOB_REBUILD_URSELF(binary_path, source_path) "gcc", "-DNOB_REBUILT", "-o", binary_path, source_path, "-lm"

#ifndef NOB_REBUILT
#define NOB_IMPLEMENTATION
#include "nob.h"

int main(int argc, char **argv) {
	Nob_Cmd cmd = {0};
	nob_cmd_append(&cmd, "touch", __FILE__);
	if (!nob_cmd_run(&cmd)) return 1;

	NOB_GO_REBUILD_URSELF(argc, argv);

	nob_cmd_append(&cmd, argv[0]);
	if (!nob_cmd_run(&cmd)) return 1;
}
#else

#define NOB_IMPLEMENTATION
#include "nob.h"
#define STB_IMAGE_IMPLEMENTATION
#define STBI_NO_BMP
#define STBI_NO_PSD
#define STBI_NO_TGA
#define STBI_NO_GIF
#define STBI_NO_HDR
#define STBI_NO_PIC
#define STBI_NO_PNM
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

size_t num_files;
size_t w, h;
uint16_t *acc_img;
uint8_t *final_img;

int main(int argc, char **argv) {
	NOB_GO_REBUILD_URSELF(argc, argv);

	Nob_File_Paths image_files = {0};
	nob_read_entire_dir("images/", &image_files);
	assert(image_files.count != 0);
	nob_da_foreach(const char *, it, &image_files) {
		const char *file = nob_temp_sprintf("images/%s", *it);
		if (nob_get_file_type(file) != NOB_FILE_REGULAR) continue;
		printf("Found image: %s\n", file);
	}

	num_files = image_files.count;
	int x, y, n;
	stbi_info(nob_temp_sprintf("images/%s", image_files.items[0]), &x, &y, &n);
	w = x;
	h = y;

	printf("Image dimensions: %lux%lu\n", w, h);

	acc_img = calloc(w*h, 6);
	final_img = calloc(w*h, 3);

	nob_da_foreach(const char *, it, &image_files) {
		const char *file = nob_temp_sprintf("images/%s", *it);
		if (nob_get_file_type(file) != NOB_FILE_REGULAR) continue;
		printf("Adding components from %s...\n", file);
		uint8_t *img = stbi_load(file, &x, &y, &n, 3);
		assert(x == w && y == h && n == 3);
		for (size_t i = 0; i < w*h; ++i) {
			acc_img[3*i + 0] += img[3*i + 0];
			acc_img[3*i + 1] += img[3*i + 1];
			acc_img[3*i + 2] += img[3*i + 2];
		}
		stbi_image_free(img);
	}
	printf("Calculating average image...\n");
	for (size_t i = 0; i < w*h; ++i) {
		final_img[3*i + 0] = acc_img[3*i + 0] / num_files;
		final_img[3*i + 1] = acc_img[3*i + 1] / num_files;
		final_img[3*i + 2] = acc_img[3*i + 2] / num_files;
	}

	printf("Writing image file!\n");
	stbi_write_png("output.png", w, h, 3, final_img, 3*w);
}

#endif
