/*
 * tab:2
 *
 * histogram.c - Implementation of histogram algorithms
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
 * Filename:      histogram.c
 * History:
 *    AS    1    Fri Feb 07 2020
 *        First written.
 */

#include <pthread.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>

#include "histogram.h"


// void* histogram_parallel(void* args)
//  The thread function for the parallel histogram implementation.
//  Loops over the input image and assigns pixels to bins, iterates
//  with a NUM_THREADS stride in the Y dimension.
//  	Inputs:
//      void* to the arg struct for the thread function
//    returns:
//      NULL

static pthread_mutex_t hist_mutex = PTHREAD_MUTEX_INITIALIZER;

void* histogram_parallel(void* args) {
  hist_args_t* a = (hist_args_t*)args;
  int bin = 0;
  for(int y = 0; y < a->height; y+=a->num_threads) {
    for(int x = 0; x < a->width; x++) {
      pthread_mutex_lock(&hist_mutex);
      if(y+a->thread_id < a->height) {
        bin = a->greyscale[(y+a->thread_id)*a->width+x]*a->length/256;
        a->histogram[bin]++;
      }
      pthread_mutex_unlock(&hist_mutex);
    }
  }
  return NULL;
}


// histogram_sequential
//  The thread function for the parallel histogram implementation.
//  Loops over the input image and assigns pixels to bins, iterates
//  with a NUM_THREADS stride in the Y dimension.
//  	Inputs:
//      uint8_t* greyscale, pointer to greyscale image
//      uint32_t* histogram, pointer to histogram bins
//      uint32_t length, the number of bins in the histogram
//      uint32_t height, the height of the image
//      uint32_t width, the width of the image
//    returns:
//      none
void histogram_sequential(uint8_t* greyscale, uint32_t* histogram, uint32_t length, uint32_t height, uint32_t width) {
  uint32_t bin = 0;
  for(int y = 0; y < height; y++) {
    for(int x = 0; x < width; x++) {
      bin = greyscale[y*width+x]*length/256;
      histogram[bin]++;
    }
  }
}
