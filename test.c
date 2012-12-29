#include "my_malloc.h"
#include <stdlib.h>
extern metadata_t* freelist[8];
void print_freelist(void)
{
    for(int i=0; i<8; i++)
    {
        printf("At index %d ",i);
        metadata_t* temp = freelist[i];
        while(temp != NULL)
        {
            printf(" %p ->",(void*)temp);
            temp = temp->next;
        }
        printf(" \n");
    }
}

int main() {

	/*
	 * Test code goes here
	 */




	 void* a = my_malloc(470);
	 print_freelist();
	 printf("Address of a is: %p\n",a);
	 printf(" \n");

	 void* b = my_malloc(37);
	 print_freelist();
	 printf("Address of b is: %p\n",b);
	 printf(" \n");

	 void* c = my_malloc(2);
	 print_freelist();
	 printf("Address of c is: %p\n",c);
	 printf(" \n");
    /**
	 void* d = my_malloc(2);
	 print_freelist();
	 printf("Address of d is: %p\n",d);
	 printf(" \n");

	 my_free(d);
	 print_freelist();
	 printf(" \n");
    **/

     a = my_realloc(a, 1000);
	 print_freelist();
	 printf("Address of a is: %p\n",a);
	 printf(" \n");

	 b = my_realloc(b, 200);
	 print_freelist();
	 printf("Address of b is: %p\n",b);
	 printf(" \n");

	 c = my_realloc(c, 2000);
	 print_freelist();
	 printf("Address of c is: %p\n",c);
	 printf(" \n");

	 my_free(c);
	 print_freelist();
	 printf("Address of c is: %p\n",c);
	 printf(" \n");
	 my_free(b);
	 print_freelist();
	 printf("Address of b is: %p\n",b);
	 printf(" \n");
	 my_free(a);
	 print_freelist();
	 printf("Address of a is: %p\n",a);
	 printf(" \n");

     printf("Second Round of Mallocs\n");
	 a = my_malloc(500);
	 print_freelist();
	 printf("Address of a is: %p\n",a);
	 printf(" \n");


	 b = my_malloc(250);
	 print_freelist();
	 printf("Address of b is: %p\n",b);
	 printf(" \n");

	 c = my_malloc(1);
	 print_freelist();
	 printf("Address of c is: %p\n",c);
	 printf(" \n");
     printf("Allocating negative space: should return NULL\n");
	 void* d = my_malloc(-1);
	 print_freelist();
	 printf("Address of d is: %p\n",d);
	 printf(" \n");


	 printf("Freeing c with my_realloc\n");
	 c = my_realloc(c,0);
	 print_freelist();
	 printf("Address of c is: %p\n",c);
	 printf(" \n");

	 printf("Allocating d\n");
	 d = my_realloc(NULL,110);
	 print_freelist();
	 printf("Address of d is: %p\n",d);
	 printf(" \n");

	 //lets try freeing a NULL ptr
	 my_free(c);
	 //now lets try reallocing a NULL ptr
	 printf("Reallocating null C\n");
	 c = my_realloc(c,160);
	 print_freelist();
	 printf("Address of c is: %p\n",c);
	 printf(" \n");

	 //attempt to allocate something of NULL size
	 void* e = my_malloc((size_t)NULL);
	 print_freelist();
	 printf("Address of e is: %p\n",e);
	 printf(" \n");
	 c = my_realloc(c,1025);
	 print_freelist();
	 printf("Address of c is: %p\n",c);
	 printf(" \n");

	 e = my_malloc(1229);
	 print_freelist();
	 printf("Address of e is: %p\n",e);
	 printf(" \n");

    /**
	 void* ai = my_malloc(0);

	 for(int i=0; i<1000; i++)
	 {
	     ai = my_realloc(ai,rand()%2048);
	     metadata_t* temp = (metadata_t*)((char*)ai - ((char)sizeof(metadata_t)));
	     if(ai == NULL)
	     {
	         printf("OUT OF MEMORY\n");
	     }
	     else
	     {
            printf("block # %d allocated\n",i);
            printf("address of block: %p\n",ai);
            printf("size of block: %d\n",temp->size);
	     }
	     char* address = (char*)ai;
	     for(int j=0; j<sizeof(metadata_t); j++)
	     {
	         address--;
	     }
	     printf("size of block is: %d",((metadata_t*)address)->size);
	     print_freelist();

	 }
	 //printf("Address of ai is: %p\n",ai);
    **/


    /**
	 void* c = my_malloc(2);
	 print_freelist();
	 printf(" \n");
	 printf("Address of c is: %p\n",c);

	 //this should return null as we need space for the metadata
	 //void* too_big = my_malloc(2048);
	 print_freelist();
	 printf(" \n");
	 //printf("Address of too_big is: %p\n",too_big);
	 char* character = my_malloc(sizeof(char));
	 print_freelist();
	 printf(" \n");
	 printf("Address of character is: %p\n",character);
	 int* integer = my_malloc(sizeof(int));
	 print_freelist();
	 printf(" \n");
	 printf("Address of integer is: %p\n",integer);
	 char* word = my_malloc(50*sizeof(char));
	 print_freelist();
	 printf(" \n");
	 printf("Address of word is: %p\n",word);
    **/


	return 0;
}
