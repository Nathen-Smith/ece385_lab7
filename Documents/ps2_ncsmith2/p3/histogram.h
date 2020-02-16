/*
 * tab:2
 *
 * histogram.h - Histogram function prototypes, and arg struct for parallel impl.
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
 * Filename:      histogram.h
 * History:
 *    AS    1    Fri Feb 07 2020
 *        First written.
 */

#ifndef HISTOGRAM_H
#define HISTOGRAM_H

typedef struct {
  uint8_t* greyscale;
  uint32_t* histogram;
  uint32_t length;
  uint32_t height;
  uint32_t width;
  uint32_t thread_id;
  uint32_t num_threads;
} hist_args_t;


// void* histogram_parallel(void* args)
//  The thread function for the parallel histogram implementation.
//  Loops over the input image and assigns pixels to bins, iterates
//  with a NUM_THREADS stride in the Y dimension.
//  	Inputs:
//      void* to the arg struct for the thread function
//    returns:
//      NULL
void* histogram_parallel(void* args);


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
void histogram_sequential(uint8_t* greyscale, uint32_t* histogram, uint32_t length, uint32_t height, uint32_t width);

#endif
