/*
 * tab:2
 *
 * main.c - I/O and histogram problem setup
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose, without fee, and without written agreement is
 * hereby granted, provided that the above copyright notice and the following
 * two paragraphs appear in all copies of this software.
 *
 * IN NO EVENT SHALL THE AUTHOR OR THE UNIVERSITY OF ILLINOIS BE LIABLE TO
 * ANY PARTY FOR DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL
 * DAMAGES ARISING OUT  OF THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION,
 * EVEN IF THE AUTHOR AND/OR THE UNIVERSITY OF ILLINOIS HAS BEEN ADVISED
 * OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * THE AUTHOR AND THE UNIVERSITY OF ILLINOIS SPECIFICALLY DISCLAIM ANY
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.  THE SOFTWARE
 * PROVIDED HEREUNDER IS ON AN "AS IS" BASIS, AND NEITHER THE AUTHOR NOR
 * THE UNIVERSITY OF ILLINOIS HAS ANY OBLIGATION TO PROVIDE MAINTENANCE,
 * SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS."
 *
 * Author:        Andrew Smith
 * Version:       1
 * Creation Date: Fri Feb 07 2020
 * Filename:      main.c
 * History:
 *    AS    1    Fri Feb 07 2020
 *        First written.
 */

#include <pthread.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>

#define STB_IMAGE_IMPLEMENTATION

#include "stb_image.h"
#include "histogram.h"

void convert_greyscale(uint8_t* image, uint8_t* gs, uint32_t height, uint32_t width) {
  int r,g,b;
  for(int y = 0; y < height; y++) {
		for(int x = 0; x < width; x++) {
      r = image[3*(y*width+x)];
      g = image[3*(y*width+x)+1];
      b = image[3*(y*width+x)+2];
			gs[y*width+x] = (uint8_t) (0.21*r + 0.71*g + 0.07*b);
    }
  }
}

int main(int argc, char** argv) {
  if(argc < 4) {
    printf("usage: ./hist <num_threads 1 to 64> <num_bins 1 to 256> <input.bmp> \n");
    return -1;
  }

  int num_threads = 1;
  uint32_t length = 1;
  sscanf(argv[1], "%d", &num_threads);
  if(num_threads < 1 && num_threads > 64) {
    printf("Threads must be an integer between 1 and 64\n");
    return -1;
  }
  sscanf(argv[2], "%u", &length);
  if(num_threads < 1 && num_threads > 256) {
    printf("Length must be an integer between 1 and 256\n");
    return -1;
  }

  int height, width, channels;
  uint8_t* image = stbi_load(argv[3], &width, &height, &channels, STBI_rgb);
  uint32_t* bins = (uint32_t*)calloc(length, sizeof(uint32_t));

  uint8_t* greyscale = (uint8_t*)malloc(sizeof(uint8_t)*height*width);
  convert_greyscale(image, greyscale, height, width);

  // Launch Sequential Histogram:
  printf("Beginning Sequential Histogram\n");
  histogram_sequential(greyscale, bins, length, height, width);
  printf("Finished Sequential Histogram\n");
  // Print Histogram:
  printf("[");
  for(int i = 0; i < length; i++) {
    if(i == length - 1) {
      printf("%d]\n", bins[i]);
    }
    else {
      printf("%d,", bins[i]);
    }
  }

  // Clear out the histogram:
  for(int i = 0; i < length; i++){
    bins[i] = 0;
  }

  printf("\nBeginning Parallel Histogram\n");
  // Launch Threads:
  pthread_t* threads = (pthread_t*)malloc(num_threads*sizeof(pthread_t));
  hist_args_t* thread_args = (hist_args_t*)malloc(num_threads*sizeof(hist_args_t));
  for(int i = 0; i < num_threads; i++) {
    // Prepare thread args:
    thread_args[i].greyscale = greyscale;
    thread_args[i].histogram = bins;
    thread_args[i].length = length;
    thread_args[i].height = height;
    thread_args[i].width = width;
    thread_args[i].thread_id = i;
    thread_args[i].num_threads = num_threads;
    pthread_create(&threads[i], NULL, histogram_parallel, &thread_args[i]);
  }

  // Join Tthreads:
  for(int i = 0; i < num_threads; i++) {
    pthread_join(threads[i], NULL);
  }
  printf("Finished Parallel Histogram\n");

  // Print Histogram
  printf("[");
  for(int i = 0; i < length; i++) {
    if(i == length - 1) {
      printf("%d]\n", bins[i]);
    }
    else {
      printf("%d,", bins[i]);
    }
  }

  // Free Memory:
  free(image);
  //free(threads);
  //free(thread_args);
  free(bins);
  return 0;
}
