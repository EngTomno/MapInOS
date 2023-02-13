
/**********************************************************************

   File          : cmpsc473-p1-lru.c

   Description   : This is LRU page replacement algorithm

   Last Modified : Jan 11 09:54:33 EST 2023
   By            : Trent Jaeger

***********************************************************************/
/**********************************************************************
Copyright (c) 2023 The Pennsylvania State University
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of The Pennsylvania State University nor the names of its contributors may be used to endorse or promote products derived from this softwiare without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
"AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
***********************************************************************/

/* Include Files */
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <assert.h>
#include <pthread.h>
#include <sched.h>

/* Project Include Files */
#include "cmpsc473-p1.h"

/* Definitions */

/* lru list */

typedef struct lru_entry {  
  unsigned int pid;
  ptentry_t *ptentry;
  struct lru_entry *next;
  struct lru_entry *prev;
} lru_entry_t;

typedef struct lru {
  lru_entry_t *first;
} lru_t;

lru_t *frame_list;

/**********************************************************************

    Function    : init_lru
    Description : initialize lru list
    Inputs      : fp - input file of data
    Outputs     : 0 if successful, -1 otherwise

***********************************************************************/

int init_lru( FILE *fp )
{
  printf("initiate lru...\n");
  frame_list = (lru_t *)malloc(sizeof(lru_t));
  frame_list->first = NULL;
  return 0;
}


/**********************************************************************

    Function    : replace_lru
    Description : choose victim from lru list (first in list is oldest)
    Inputs      : victim - process id of victim frame 
                  frame - frame assigned from lru replacement
                  ptentry - pt entry mapping frame currently -- to be invalidated
    Outputs     : 0 if successful, -1 otherwise

***********************************************************************/

int replace_lru( unsigned int *victim, frame_t **frame, ptentry_t **ptentry )
{
  /* Task 3(b) */
 // Initialize variables
    int i, lru_min, min_index, curr_lru;
    frame_t *f;
    
    // Search the frames for a victim page
    for (i = 0; i < num_frames; i++) {
        f = &frame_table[i];
        curr_lru = f->lru;
        // Check if page is referenced
        if ((curr_lru & 0x4) == 0) {
            // Page not referenced, evict page
            *victim = f->page;
            *ptentry = f->ptentry;
            f->ptentry = NULL;
            f->page = 0;
            // Shift LRU values and set MSB to 1
            for (int j = 0; j < num_frames; j++) {
                frame_table[j].lru = frame_table[j].lru >> 1 | 0x4;
            }
            f->lru = 0;
            // Update the frame table
            *frame = f;
            return 0;
        } else {
            // Page referenced, check LRU value
            if ((curr_lru & 0x2) == 0) {
                // LRU value is 2
                f->lru = curr_lru >> 1 | 0x4;
            } else if ((curr_lru & 0x1) == 0) {
                // LRU value is 1
                f->lru = curr_lru >> 1;
            } else {
                // LRU value is 0, set as victim
                *victim = f->page;
                *ptentry = f->ptentry;
                f->ptentry = NULL;
                f->page = 0;
                // Shift LRU values and set MSB to 1
                for (int j = 0; j < num_frames; j++) {
                    frame_table[j].lru = frame_table[j].lru >> 1 | 0x4;
                }
                f->lru = 0;
                // Update the frame table
                *frame = f;
                return 0;
            }
        }
    }
    return -1;
} 



/**********************************************************************

    Function    : update_lru
    Description : update lru list on allocation (add entry to end)
    Inputs      : pid - process id
                  ptentry - mapped to frame
    Outputs     : 0 if successful, -1 otherwise

***********************************************************************/

int update_lru( unsigned int pid, ptentry_t *ptentry)
{
  /* Task 3(b) */
  // Initialize variables
    int i, lru_min, min_index;
    frame_t *f;
    
    // Find an empty frame
    for (i = 0; i < num_frames; i++) {
        f = &frame_table[i];
        if (f->page == 0) {
            // Empty frame found, update frame table
            f->page = ptentry->page;
            f->ptentry = ptentry;
            f->lru = 0;
            // Update the page table
            ptentry->frame = i;
            ptentry->valid = 1;
            ptentry->ref = 1;
            ptentry->mod = 0;
            return 0;
        }
    }
    
    // No empty frames, use 3-bit LRU to replace a page
    replace_lru(&i, &f, &ptentry);
    
    // Update the page table
  
}


