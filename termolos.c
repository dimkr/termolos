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
#include <stdbool.h>

#define USAGE "Usage: %s\n"

#define RAND_MIN (0x33)

static void rand_color(unsigned int *seed,
                       unsigned char col[3],
                       const unsigned int min,
                       const unsigned int max)
{
	col[0] = (unsigned char) (min + (rand_r(seed) % (max - min)));
	col[1] = (unsigned char) (min + (rand_r(seed) % (max - min)));
	col[2] = (unsigned char) (min + (rand_r(seed) % (max - min)));
}

int main(int argc, char *argv[])
{
	unsigned char cmap[16][3];
	int ret = EXIT_FAILURE;
	int fd;
	unsigned int seed;

	if (1 != argc) {
		(void) fprintf(stderr, USAGE, argv[0]);
		goto end;
	}

	fd = open(_PATH_CONSOLE, O_WRONLY | O_NOCTTY);
	if (-1 == fd)
		goto end;

	seed = (unsigned int) time(NULL);

	/* 0 must be very dark - the background color of pretty much everything */
	rand_color(&seed, cmap[0], 0, 0x12);

	rand_color(&seed, cmap[1], RAND_MIN, UCHAR_MAX);
	rand_color(&seed, cmap[2], RAND_MIN, UCHAR_MAX);
	rand_color(&seed, cmap[3], RAND_MIN, UCHAR_MAX);

	/* 4 must be blue - MC, ytree and others assume this and pick a matching
	 * forgeround color */
	cmap[4][0] = 0;
	cmap[4][1] = 0;
	cmap[4][2] = 0xaa;

	rand_color(&seed, cmap[5], RAND_MIN, UCHAR_MAX);

	/* 6 must be cyan, often used as for text on blue background */
	cmap[6][0] = 0;
	cmap[6][1] = 0xaa;
	cmap[6][2] = 0xaa;

	/* 7 must be light, used for text */
	cmap[7][0] = 0xaa;
	cmap[7][1] = 0xaa;
	cmap[7][2] = 0xaa;

	/* 8 must be gray, used for text */
	rand_color(&seed, cmap[8], 0x33, 0x55);

	rand_color(&seed, cmap[9], RAND_MIN, UCHAR_MAX);
	rand_color(&seed, cmap[10], RAND_MIN, UCHAR_MAX);
	rand_color(&seed, cmap[11], RAND_MIN, UCHAR_MAX);

	/* 12 must be dark - ytree uses it for highlighted text, with a light
	 * background */
	rand_color(&seed, cmap[12], 0x33, 0x55);

	rand_color(&seed, cmap[13], RAND_MIN, UCHAR_MAX);

	/* 14 must be light cyan */
	cmap[14][0] = 0x55;
	cmap[14][1] = 0xff;
	cmap[14][2] = 0xff;

	/* 15 must be white */
	cmap[15][0] = 0xef;
	cmap[15][1] = 0xef;
	cmap[15][2] = 0xef;

 	if (-1 == ioctl(fd, PIO_CMAP, (void *) cmap))
		goto close_fd;

	ret = EXIT_SUCCESS;

close_fd:
	(void) close(fd);

end:
	return ret;
}
