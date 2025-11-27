/* Converts whichever image file is passed in into a JPEG image */

#define NOB_REBUILD_URSELF(binary_path, source_path) "gcc", "-DNOB_REBUILT", "-o", binary_path, source_path, "-lm"

#ifndef NOB_REBUILT
#define NOB_IMPLEMENTATION
#include "nob.h"

int main(int argc, char **argv) {
	Nob_Cmd cmd = {0};
	nob_cmd_append(&cmd, "touch", __FILE__);
	if (!nob_cmd_run(&cmd)) return 1;

	NOB_GO_REBUILD_URSELF(argc, argv);
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

int w, h;
uint8_t *rotated;

int main(int argc, char **argv) {
	NOB_GO_REBUILD_URSELF(argc, argv);

	nob_shift(argv, argc);

	if (argc != 1) {
		nob_log(NOB_ERROR, "Expected a single argument (%d found), the file to convert to JPEG", argc);

		return 1;
	}

	nob_log(NOB_INFO, "Loading file...");
	uint8_t *img = stbi_load(argv[0], &w, &h, NULL, 3);
	if (img == NULL) {
		nob_log(NOB_ERROR, "Couldn't load image file '%s'", argv[0]);
		return 1;
	}

	nob_log(NOB_INFO, "Rotating image...");
	rotated = calloc(w*h, 3);
	for (int y = 0; y < h; ++y) {
		for (int x = 0; x < w; ++x) {
			const int idx = y*w + x;
			const int idx_rot = x*h + y;
			rotated[idx_rot*3 + 0] = img[idx*3 + 0];
			rotated[idx_rot*3 + 1] = img[idx*3 + 1];
			rotated[idx_rot*3 + 2] = img[idx*3 + 2];
		}
	}
	stbi_image_free(img);

	nob_log(NOB_INFO, "Writing image file!");
	stbi_write_jpg("output.jpg", h, w, 3, rotated, 3*h);
	free(rotated);
}

#endif
