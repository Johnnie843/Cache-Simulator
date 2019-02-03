// -----------------------------------
// CSCI 340 - Operating Systems
// Spring 2019
// cache.c file
// 
// Homework 1
//
// -----------------------------------

#include "cache.h"
#include "memory.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

int initializeCache(unsigned int number_of_lines)
{
  int line, retVal;

  retVal = OK;

  cache = malloc(sizeof(cache_line*) * number_of_lines);

  if (cache != NULL) {

    for (line=0; line<number_of_lines; line++) {

      cache[line] = (cache_line*) malloc(sizeof(cache_line));
      cache[line]->tag = UNK;
      cache[line]->hit_count = ZERO;
      
    }
    
  } else
    retVal = FAIL;

  return retVal;

} // end initializeCache function


int cread(unsigned int cmf, unsigned int* hex_addr, unsigned int* found,
	  unsigned int* replace)
{

  unsigned int tag;
  unsigned int lineID;
  unsigned int offset = ((*hex_addr) >> 0) & ((1 << 2) -1);
  unsigned int sizeOfBlocks = 4;
  
  

  //Check Mapping Algorithm - True is Direct Mapping
  if (cmf == DM){
	  
	  //Bitwise Shift over 5 postions to isolate the tag
	  tag = (*hex_addr)>>5;
	  
	  //Bitwise Shift over 2 postions to drop the offset, then Bitwise shift right to drop the tag. Isolates the lineID
	  lineID = ((*hex_addr) >> 2) & ((1 << 3) -1);  
	  
	  //For Debugging
	  printf("Input Line Number: %02X\n", lineID);
  	  printf("Input Tag Number: %02X\n", tag);
  	  printf("Cache Tag Number at Input Line: %d\n", cache[lineID]->tag);
  
  	  //Check if tag matches
  	  if(cache[lineID]->tag == tag){
  		  
  		  //Cache HIT, Increase the hit_count and mark as found and don't replace
  		  printf("HIT\n");
  		  cache[lineID]->hit_count = (cache[lineID]->hit_count) + 1;
  		  (*found) = 1;
  		  (*replace) = 0;
  		  
  		  //For Debugging
  		  printf("Retrieved Data From Cache At Position: %d\n",(lineID));
  		  
  		  //Return data at from main memory array by mapping the loction
  		  //instead of using the hex_addr, since we are not storing the data in the Cache 		  
  		  return phy_memory[((((tag+1) * (lineID+1)) - 1) * sizeOfBlocks) + offset];
	  
  	  }
  	  else{
  		  
  		  //Cache MISS, Swap tags and mark as replaced.
  		  printf("MISS\nAdding New Tag to Cache\n");
  		  cache[lineID]->tag = tag;
  		  printf("Cache Tag Number At That Line Number Is Now: %d\n", cache[lineID]->tag);
  		  printf("Getting Data From Main Memory\n");
  		  (*found) = 0;
  		  (*replace) = 1;
  		  //Return data using hex_addr
  		  return phy_memory[(*hex_addr)-1];
  	  }
  }
  
  //Set Associative was selected
  else{
	   //Bitwise shift over 4 places to isolate the tag
	   tag = (*hex_addr)>>4;
	   
	   //Bitwise shift over 2 places, then bitwise shift over left 4 times to to isolate the Set Number
	   unsigned int setNum = ((*hex_addr) >> 2) & (3);
	   
	   //For Debugging
	   printf("Input Set Number: %02X\n", setNum);
	   printf("Input Tag Number: %02X\n", tag);
	   printf("Starting Index of the Set: %d\n", setNum*2);	   
	   printf("Cache Tag Numbers Stored in The Set: %d, %d\n", cache[setNum*2]->tag, cache[(setNum*2)+1]->tag);
	     
	   //Check if tag exist in the Cache
	   if((cache[setNum*2]->tag == tag) || (cache[(setNum*2)+1]->tag == tag)){
		   
		   //Cache Hit
		   //Determine which tag is a hit.
		   //Increase hit_count
		   //Flag for found and return data
		   
		   //Check first postion in the set
		   if((cache[setNum*2]->tag) == tag){

			     printf("HIT\n");
	   	   	     cache[setNum*2]->hit_count = (cache[setNum*2]->hit_count) + 1;
	   	   	     (*found) = 1;
	   	   	     (*replace) = 0;
	   	   	     printf("Retrieved Data From Cache At Position: %d\n",(setNum*2));
	   	   	    return phy_memory[((((tag+1) * (setNum+1)) - 1) * sizeOfBlocks) + offset];
		   }
		   
	   	  	//Check second postion in the set
	   	   else if((cache[(setNum*2)+1]->tag) == tag){
				 printf("HIT\n");
			   	 cache[setNum]->hit_count = (cache[setNum]->hit_count) + 1;
			   	 (*found) = 1;
			   	 (*replace) = 0;
			   	 printf("Retrieved Data From Cache At Position: %d\n",(setNum*2));
			   	 return phy_memory[((((tag+1) * (setNum+1)) - 1) * sizeOfBlocks) + offset];
		   }
	   }
	   //Cache Miss
	    else{
	    	
	    	//Swap for Least Frequently Used Line
	    	printf("MISS\nPreforming LFU Algorithm\n");
	    	
	    	//Check if first pistion in the set is lower then the second.
	    	if ((cache[setNum*2]->hit_count <= cache[(setNum*2)+1]->hit_count)){
	    		
	    		//swap first position with new tag
	    		cache[(setNum*2)]->tag = tag;
	    		
	    	}else{
	    		//swap second position with new tag
	    		cache[(setNum*2)+1]->tag = tag;
	    	}
	    	
	     	printf("Getting Data From Phy Memory\n");
	     	(*found) = 0;
	     	(*replace) = 1;
	     	
	   	  //return data from main memory
	     	return phy_memory[(*hex_addr)-1];
	    }   
   }
   return FAIL;
}// end cread function 

void cprint()
{

  unsigned int line;

  printf("\n---------------------------------------------\n");
  printf("line\ttag\tnum of hits\n");
  printf("---------------------------------------------\n");

  for (line=0; line<NUM_OF_LINES; line++) { 

    if (cache[line]->tag == UNK) {
			
      printf("%d\t%d\t%d\n", line, cache[line]->tag, cache[line]->hit_count);

    } else {

      printf("%d\t%02X\t%d\n", line, cache[line]->tag, cache[line]->hit_count);

    }

  }
}



