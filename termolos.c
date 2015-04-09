/*
 * this file is part of termolos.
 *
 * Copyright (c) 2015 Dima Krasner
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include <stdlib.h>
#include <stdio.h>
#include <paths.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>
#include <sys/ioctl.h>
#include <linux/kd.h>
#include <limits.h>

#define USAGE "Usage: %s\n"

#define RAND_BG(seed, out) \
	do {                            \
		out[0] = rand_r(seed) % 25; \
		out[1] = rand_r(seed) % 25; \
		out[2] = rand_r(seed) % 25; \
	} while (0)

#define RAND_FG(seed, out) \
	do {                                                   \
		out[0] = 200 + (rand_r(seed) % (UCHAR_MAX - 200)); \
		out[1] = 200 + (rand_r(seed) % (UCHAR_MAX - 200)); \
		out[2] = 200 + (rand_r(seed) % (UCHAR_MAX - 200)); \
	} while (0)

#define RAND_COLOR(seed, out) \
	do {                                                   \
		out[0] = 120 + (rand_r(seed) % (UCHAR_MAX - 120)); \
		out[1] = 120 + (rand_r(seed) % (UCHAR_MAX - 120)); \
		out[2] = 120 + (rand_r(seed) % (UCHAR_MAX - 120)); \
	} while (0)

#define BRIGHT_COMPONENT(seed, in, out) \
	do {                                                  \
		if (UCHAR_MAX != in)                              \
			out = in + rand_r(seed) % (UCHAR_MAX - in);   \
		else                                              \
			out = 75 + (rand_r(seed) % (UCHAR_MAX - 75)); \
	} while (0)

#define BRIGHT_COLOR(seed, in, out) \
	do {                                       \
		BRIGHT_COMPONENT(seed, in[0], out[0]); \
		BRIGHT_COMPONENT(seed, in[1], out[1]); \
		BRIGHT_COMPONENT(seed, in[2], out[2]); \
	} while (0)

int main(int argc, char *argv[])
{
	unsigned char cmap[16][3];
	int ret = EXIT_FAILURE;
	int fd;
	unsigned int seed;
	int i;

	if (1 != argc) {
		(void) fprintf(stderr, USAGE, argv[0]);
		goto end;
	}

	fd = open(_PATH_CONSOLE, O_WRONLY | O_NOCTTY);
	if (-1 == fd)
		goto end;

	seed = (unsigned int) time(NULL);

	RAND_BG(&seed, cmap[0]);
	BRIGHT_COLOR(&seed, cmap[0], cmap[8]);

	RAND_FG(&seed, cmap[7]);
	BRIGHT_COLOR(&seed, cmap[7], cmap[15]);

	for (i = 1; 7 > i; ++i) {
		RAND_COLOR(&seed, cmap[i]);
		BRIGHT_COLOR(&seed, cmap[i], cmap[8 + i]);
	}

 	if (-1 == ioctl(fd, PIO_CMAP, (void *) cmap))
		goto close_fd;

	ret = EXIT_SUCCESS;

close_fd:
	(void) close(fd);

end:
	return ret;
}
