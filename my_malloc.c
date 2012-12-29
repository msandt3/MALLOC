#include "my_malloc.h"

/* You *MUST* use this macro when calling my_sbrk to allocate the
 * appropriate size. Failure to do so may result in an incorrect
 * grading!
 */
#define SBRK_SIZE 2048

/* If you want to use debugging printouts, it is HIGHLY recommended
 * to use this macro or something similar. If you produce output from
 * your code then you will receive a 20 point deduction. You have been
 * warned.
 */
#ifdef DEBUG
#define DEBUG_PRINT(x) printf x
#else
#define DEBUG_PRINT(x)
#endif


/* make sure this always points to the beginning of your current
 * heap space! if it does not, then the grader will not be able
 * to run correctly and you will receive 0 credit. remember that
 * only the _first_ call to my_malloc() returns the beginning of
 * the heap. sequential calls will return a pointer to the newly
 * added space!
 * Technically this should be declared static because we do not
 * want any program outside of this file to be able to access it
 * however, DO NOT CHANGE the way this variable is declared or
 * it will break the autograder.
 */
void* heap;

/* our freelist structure - this is where the current freelist of
 * blocks will be maintained. failure to maintain the list inside
 * of this structure will result in no credit, as the grader will
 * expect it to be maintained here.
 * Technically this should be declared static for the same reasons
 * as above, but DO NOT CHANGE the way this structure is declared
 * or it will break the autograder.
 */
metadata_t* freelist[8];
/**** SIZES FOR THE FREE LIST ****
 * freelist[0] -> 16
 * freelist[1] -> 32
 * freelist[2] -> 64
 * freelist[3] -> 128
 * freelist[4] -> 256
 * freelist[5] -> 512
 * freelist[6] -> 1024
 * freelist[7] -> 2048
 */

static metadata_t* offset = 0;
static int firstRun = 0;

int remove_from_list(metadata_t* block, int index)
{
    metadata_t* temp = freelist[index];
    //if it is at the head of the list
    if(temp == block)
    {
        freelist[index] = block->next;
        if(block->next != NULL)
            block->next->prev = NULL;
        block->next = NULL;
        block->prev = NULL;
        return 1;
    }
    else
    {
        while(temp != NULL)
        {
            if(temp == block)
            {
                if(temp->next == NULL)
                {
                    temp->prev->next = NULL;
                    block->next = NULL;
                    block->prev = NULL;
                    return 1;
                }
                else
                {
                    temp->next->prev = temp->prev;
                    temp->prev->next = temp->next;
                    block->next = NULL;
                    block->prev = NULL;
                    return 1;
                }
            }
            temp = temp->next;
        }
    }
    return 0;

}

void* my_malloc(size_t size)
{
    //adjust size of block to account for metadata
    int int_size = (int)size;
    size_t adjSize = size + sizeof(metadata_t);
    if(int_size <= 0)
    {
        return NULL;
    }
    if(adjSize > 2048)
    {
        return NULL;
    }
    int index = find_index(adjSize);
    metadata_t* retBlock = NULL;

    //if there exists a block of needed size in the freelist, return it to the user, we're done
    if(freelist[index] != NULL)
    {
        retBlock = freelist[index];
        remove_from_list(retBlock,index);
        retBlock->in_use = 1;
        char* address = (char*)retBlock;

        for(int i=0; i<sizeof(metadata_t); i++)
        {
            address++;
        }

        retBlock = (metadata_t*)(address);

        return retBlock;
    }
    else
    {

        metadata_t* splitBlock = NULL;
        int splitIndex = 0;
        //find next available block to split
        for(int i=8; i>index; i--)
        {
            if(freelist[i] != NULL)
            {
                splitBlock = freelist[i];
                splitIndex = i;
            }
        }
        //if there are no available blocks allocate more heap space
        if(splitBlock == NULL)
        {
            freelist[7] = my_sbrk(SBRK_SIZE);
            if(firstRun == 0)
            {
                offset = freelist[7];
                heap = freelist[7];
                firstRun = 1;
            }
            //if the allocation of heap space failed return NULL
            if(freelist[7] == NULL)
            {
                return NULL;
            }
            else
            {
                freelist[7]->size = 2048;
                freelist[7]->prev = NULL;
                freelist[7]->next = NULL;
                freelist[7]->in_use = 0;
            }
        }
        //otherwise split block into two smaller blocks
        else
        {
            split(splitBlock,splitIndex);
        }


        //recurse
        retBlock = my_malloc(size);

    }
    return retBlock;
}

void split(metadata_t* block, int index)
{
    //remove the block we're splitting from freelist at that index
    //init size to minimum size
    int size = block->size;
    //we now have the size of the block we're going to split
    char* split_address = (char*)block;
    for(int i=0; i<size/2; i++)
    {
        split_address++;
    }

    //metadata_t* split_address = block + (char)(size/2);



    //create two blocks of approx. 1/2 the size of the initial block
    metadata_t* temp1 = block;
    metadata_t* temp2 = (metadata_t*)(split_address);
    //remove the block from its current position in the freelist
    freelist[index] = block->next;
    if(block->next != NULL)
    {
        //make sure freelist is still in tact
        block->next->prev = NULL;
    }
    //set up linked list structure of two new blocks
    temp1->next = temp2;
    temp1->prev = NULL;
    temp2->next = NULL;
    temp2->prev = temp1;
    temp1->in_use = 0;
    temp2->in_use = 0;
    temp1->size = size/2;
    temp2->size = size/2;
    //place first split block into new position of freelist
    freelist[index-1] = temp1;

    //we have successfully split a block
}

void* my_realloc(void* ptr, size_t new_size)
{
  /* FIX ME */
  //if ptr is null we're in the malloc case
  if(ptr == NULL)
  {
      ptr = my_malloc(new_size);
      //if malloc failed, return null
      if(ptr == NULL)
      {
          return NULL;
      }
      return ptr;
  }
  //check if we're in free case
  else if(new_size == 0)
  {
      my_free(ptr);
      return NULL;
  }

  //otherwise
  else
  {
      void* temp = my_malloc(new_size);
      if(temp == NULL)
      {
          return NULL;
      }
      metadata_t* src_block = (metadata_t*)((char*)temp - (char)sizeof(metadata_t));
      size_t src_size = (size_t)(src_block->size) - sizeof(metadata_t);
      if(new_size > src_size)
      {
          temp = my_memcpy(temp,ptr,src_size);
      }
      else if(new_size < src_size)
      {
          temp = my_memcpy(temp,ptr,new_size);
      }


      my_free(ptr);
      return temp;
  }
  return NULL;
}

void my_free(void* ptr)
{
    if(ptr != NULL)
    {
        metadata_t* freedBlock = (metadata_t*)((char*)ptr - (char)sizeof(metadata_t));
        freedBlock->in_use = 0;
        metadata_t* freedBuddy = findBuddy(freedBlock);
        //subtract heap offset

        //if the buddy is null, we should just put the block on the freelist
        if(freedBuddy == NULL)
        {
            int index = find_index(freedBlock->size);
            freedBlock->next = freelist[index];
            if(freedBlock->next != NULL)
            {
                freelist[index]->prev = freedBlock;
            }
            freelist[index] = freedBlock;
        }
        //otherwise
        else
        {
            //we attempt to merge the two buddies
            merge(freedBlock);
        }
    }

}

int log_base_2(int size)
{
    int retVal = 4;
    if(size <= 16)
    {
        return retVal;
    }
    else
    {
        while(size > 16)
        {
            size = size/2;
            retVal++;
        }
    }
    return retVal;
}

void merge(void* block)
{
    metadata_t* freedBlock = (metadata_t*)block;
    metadata_t* buddy = findBuddy(freedBlock);
    int index = log_base_2(freedBlock->size)-4;
    //int c_buddy = (char*)buddy - (char*)offset;
    int c_block = (char*)block - (char*)offset;
    //if the block is of max size or has no free buddy
    if(freedBlock->size == 2048 || buddy == NULL)
    {

    }
    else
    {
        //if we have the second in the pair
        if( (c_block>>log_base_2(freedBlock->size)) & 0x1  )
        {
            //create a block of double the size out of the buddy's pointer
            metadata_t* temp = buddy;
            temp->size = buddy->size*2;

            //remove buddy from freelist
            remove_from_list(freedBlock,index);
            remove_from_list(buddy,index);
            //place new block on freelist

            if(freelist[index+1] == NULL)
            {
                freelist[index+1] = temp;
            }
            else
            {
                temp->next = freelist[index+1];
                freelist[index+1]->prev = temp;
                freelist[index+1] = temp;
            }
            //recurse using new block as point to merge from
            merge(temp);
        }
        //otherwise
        else if( !((c_block>>log_base_2(buddy->size)) & 0x1) )
        {
            //create block of double the size
            metadata_t* temp = freedBlock;
            temp->size = freedBlock->size*2;

            //remove buddy from freelist
            remove_from_list(freedBlock,index);
            remove_from_list(buddy,index);

            //place new block on freelist
            if(freelist[index+1] == NULL)
            {
                freelist[index+1] = temp;
            }
            else
            {
                temp->next = freelist[index+1];
                freelist[index+1]->prev = temp;
                freelist[index+1] = temp;
            }
            //recurse using new block as point to merge from
            merge(temp);
        }
    }
}
//this method will take in a block metadata_t block to find its buddy
//it will return NULL if the buddy is in use, null, or of different size
//otherwise it will check the n+1 bits and see if they are identical
metadata_t* findBuddy(metadata_t* block)
{
    char* address = (char*)block;
    char* prevaddress = (char*)block;
    if(block == offset && block->size == 2048)
    {
        return NULL;
    }
    for(int i=0; i<block->size; i++)
    {
        address++;
        if(block != offset)
        {
            prevaddress--;
        }
    }
    metadata_t* next_buddy = (metadata_t*)address;
    metadata_t* prev_buddy = (metadata_t*)prevaddress;
    int c_next_buddy = (char*)next_buddy - (char*)offset;
    int c_prev_buddy = (char*)prev_buddy - (char*)offset;
    int c_block = (char*)block - (char*)offset;
    metadata_t* ret_buddy = NULL;


    if(  (c_next_buddy>>(log_base_2(block->size)+1) & 0x1) ==  (((c_block)>>(log_base_2(block->size)+1)) & 0x1) )
    {
        ret_buddy = next_buddy;
    }
    else if ( ((c_prev_buddy>>(log_base_2(block->size)+1) & 0x1) ==  (((c_block)>>(log_base_2(block->size)+1)) & 0x1)) && (block != offset)  )
    {
        ret_buddy = prev_buddy;
    }
    else
    {
        ret_buddy = NULL;
    }

    //if buddy returns NULL
    if(ret_buddy == NULL)
    {
        return NULL;
    }
    //if the blocks are of two different sizes
    if(ret_buddy->size != block->size)
    {
        return NULL;
    }
    if(ret_buddy->in_use == 1)
    {
        return NULL;
    }

    return ret_buddy;


}

void* my_memcpy(void* dest, const void* src, size_t num_bytes)
{
  /* FIX ME */

  char* c_dest = (char*)(dest);
  char* c_src = (char*)(src);
  size_t size = num_bytes;
  while(size--)
  {
      *c_dest++ = *c_src++;
  }
  return dest;
}

int find_index(size_t size)
{
    int notFound = 1;
    size_t in_comp = 2048;
    int index = 7;
    while(notFound)
    {
        if(size <= in_comp && size > in_comp/2)
        {
            notFound = 0;
        }
        else if(size < 16)
        {
            index = 0;
            notFound = 0;
        }
        else
        {
            in_comp = in_comp/2;
            index--;
        }
    }
    return index;
}
