/* 
   File: kernel.C

   Author: R. Bettati
   Department of Computer Science
   Texas A&M University

   Date  : 05/01/12

   The "main()" function is the entry point for the kernel. 

*/



/*--------------------------------------------------------------------------*/
/* INCLUDES */
/*--------------------------------------------------------------------------*/

#include "console.H"

using namespace std;

/* ======================================================================= */
/* MAIN -- THIS IS WHERE THE OS KERNEL WILL BE STARTED UP */
/* ======================================================================= */

int main()
{

    /* -- INITIALIZE CONSOLE */
    Console::init();
    Console::puts("Welcome to my Kernel!\n"); 
    Console::puts("Husain Alshehhi - CSCE 410.\n");
  
    /* -- LOOP FOREVER! */
    for(;;);
  
}
