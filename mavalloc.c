/*
      Amey Shinde
      ID : 1001844387


      First Fit, Next Fit , mavalloc_init , mavalloc_free
      mavalloc_destroy, mavalloc_size work completely fine.
      Issues with Best Fit and Worst Fit

      Code taken from professors repo isnt commented. 
*/


// The MIT License (MIT)
// 
// Copyright (c) 2022 Trevor Bakker 
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
// 
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

#include "mavalloc.h"
#include <stdlib.h>
#include <stdio.h>
#include <limits.h>

enum TYPE
{
    FREE = 0,
    USED
};

struct Node {
  size_t size;
  enum TYPE type;
  void * arena;
  struct Node * next;
  struct Node * prev;
};

struct Node *alloc_list;
struct Node *previous_node;


void * arena;
int cnt;

enum ALGORITHM allocation_algorithm = FIRST_FIT;

int mavalloc_init( size_t size, enum ALGORITHM algorithm )
{

  if(size<0)
  {
    return -1;
  }
  if(!(arena = malloc( ALIGN4( size ) )))
  {
    return -1;
  }
  
  allocation_algorithm = algorithm;

  alloc_list = ( struct Node * )malloc( sizeof( struct Node )); //new_node

  alloc_list -> arena = arena;
  alloc_list -> size  = ALIGN4(size);
  alloc_list -> type  = FREE;
  alloc_list -> next  = NULL;
  alloc_list -> prev  = NULL;

  previous_node  = alloc_list;

  return 0;
}

void mavalloc_destroy( )
{
  free( arena );
  struct Node * ptr = alloc_list;
  struct Node * next = NULL;

  // iterate over the linked list and free the nodes
  while( alloc_list )
  {
    next = alloc_list->next; // temperary node
    free( alloc_list );
    alloc_list = next; 
  }

  return;
}

void * mavalloc_alloc( size_t size )
{
  struct Node * node;

  if( allocation_algorithm != NEXT_FIT )
  { 
    node = alloc_list;
  }
  else if ( allocation_algorithm == NEXT_FIT )
  {
    node = previous_node;
  }
  else
  {
    printf("ERROR: Unknown allocation algorithm!\n");
    exit(0);
  }

  size_t aligned_size = ALIGN4( size );

  //implement First Fit
  if( allocation_algorithm == FIRST_FIT )
  {
    while( node )
    {
      if( node -> size >= aligned_size  && node -> type == FREE )
      {
        int leftover_size = 0;
  
        node -> type  = USED; 
        leftover_size = node -> size - aligned_size;
        node -> size =  aligned_size;
        if( leftover_size > 0 )
        {
          struct Node * previous_next = node -> next;
          struct Node * leftover_node = ( struct Node * ) malloc ( sizeof( struct Node ));
  
          leftover_node -> arena = node -> arena + size;
          leftover_node -> type  = FREE;
          leftover_node -> size  = leftover_size;
          leftover_node -> next  = previous_next;
  
          node -> next = leftover_node;
        }
        previous_node = node;
        return ( void * ) node -> arena;
      }
      node = node -> next;
    }
  }

  // Implement Next Fit
  else if( allocation_algorithm == NEXT_FIT )
  {
    int num_blocks = mavalloc_size(),cnt=0; // counter for number of times 
                                            // we tried to allocate mem.
    while( node )
    {
      if( node -> size >= aligned_size  && node -> type == FREE )
      {
        int leftover_size = 0;
  
        node -> type  = USED; 
        leftover_size = node -> size - aligned_size;
        node -> size =  aligned_size;
        cnt++;
        if( leftover_size > 0 )
        {
          struct Node * previous_next = node -> next;
          struct Node * leftover_node = ( struct Node * ) malloc ( sizeof( struct Node ));
  
          leftover_node -> arena = node -> arena + size;
          leftover_node -> type  = FREE;
          leftover_node -> size  = leftover_size;
          leftover_node -> next  = previous_next;
  
          node -> next = leftover_node;
        }
        previous_node = node;
        return ( void * ) node -> arena;
      }  
        node = node -> next;
    }

    if((num_blocks-cnt)!=0) //for rolling over the linked list
        {
          int a;
          node=alloc_list;
          for(a=0;a<num_blocks;a++) // roll from top to allocate memory
          {
            if( node -> size >= aligned_size  && node -> type == FREE )
            {
              int leftover_size = 0;
  
              node -> type  = USED; 
              leftover_size = node -> size - aligned_size;
              node -> size =  aligned_size;

              if( leftover_size > 0 )
              {
                struct Node * previous_next = node -> next;
                struct Node * leftover_node = ( struct Node * ) malloc ( sizeof( struct Node ));
  
                leftover_node -> arena = node -> arena + size;
                leftover_node -> type  = FREE;
                leftover_node -> size  = leftover_size;
                leftover_node -> next  = previous_next;
  
                node -> next = leftover_node;
              }
              previous_node = node;
              return (void * ) node -> arena;
            }
            node = node -> next;
          }
        }
    
  }
  // Implement Worst Fit (Most code derived from First Fit)
  else if(allocation_algorithm == WORST_FIT)
  {
    int num_blocks = mavalloc_size(),cnt=0;
    if(num_blocks==1)
    {
      while( node )
    {
      if( node -> size >= aligned_size  && node -> type == FREE )
      {
        int leftover_size = 0;
  
        node -> type  = USED; 
        leftover_size = node -> size - aligned_size;
        node -> size =  aligned_size;
        cnt++;
        if( leftover_size > 0 )
        {
          struct Node * previous_next = node -> next;
          struct Node * leftover_node = ( struct Node * ) malloc ( sizeof( struct Node ));
  
          leftover_node -> arena = node -> arena + size;
          leftover_node -> type  = FREE;
          leftover_node -> size  = leftover_size;
          leftover_node -> next  = previous_next;
  
          node -> next = leftover_node;
        }
        previous_node = node;
        return ( void * ) node -> arena;
      }
      node = node -> next;
    }
    }
  }
  // Implement Best Fit (Most code derived from First Fit)
  else if(allocation_algorithm == BEST_FIT)
  {
    int num_blocks = mavalloc_size(),cnt=0;
    if(num_blocks==1)
    {
      while( node )
    {
      if( node -> size >= aligned_size  && node -> type == FREE )
      {
        int leftover_size = 0;
  
        node -> type  = USED; 
        leftover_size = node -> size - aligned_size;
        node -> size =  aligned_size;
        cnt++;
        if( leftover_size > 0 )
        {
          struct Node * previous_next = node -> next;
          struct Node * leftover_node = ( struct Node * ) malloc ( sizeof( struct Node ));
  
          leftover_node -> arena = node -> arena + size;
          leftover_node -> type  = FREE;
          leftover_node -> size  = leftover_size;
          leftover_node -> next  = previous_next;
  
          node -> next = leftover_node;
        }
        previous_node = node;
        return ( void * ) node -> arena;
      }
      node = node -> next;
    }
    }
  }

  return NULL;
}

void mavalloc_free( void * ptr )
{
  struct Node * node = alloc_list;
  while( node )
  {
    if( node -> arena == ptr )
    {
      if( node -> type == FREE)
      {
        //printf("Warning: Double free detected\n");
      }
      node -> type = FREE;
      break;
    }
    node = node -> next;
  }
  
  return;  
}

int mavalloc_size( )
{
  int number_of_nodes = 0;
  struct Node * ptr = alloc_list;

  while( ptr )
  {
    number_of_nodes ++;
    ptr = ptr -> next; 
  }

  return number_of_nodes;
}



