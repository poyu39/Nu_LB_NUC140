//
// SEMIHOST_CPUASM : semihost debug mode with assembly code
//
// For SemiHosted Debugging mode, 
// Keil MDK Flash Configure Tools menu C/C++ need
// DEBUG_ENABLE_SEMIHOST (defined in Precessor Symbol)
// add retarget.c to Library Group

#include <stdio.h>
#include "NUC100Series.h"
#include "MCU_init.h"
#include "SYS_init.h"

__asm void my_strcpy(const char *src, char *dst)
{
loop
      LDRB  r2, [r0]	; Load byte into r2 from memory pointed to by r0 (src pointer)
      ADDS  r0, #1      ; Increment src pointer
      STRB  r2, [r1]	; Store byte in r2 into memory pointed to by (dst pointer)
      ADDS  r1, #1      ; Increment dst pointer
      CMP   r2, #0      ; Was the byte 0?
      BNE   loop        ; If not, repeat the loop
      BX    lr          ; Else return from subroutine
}

__asm void my_capitalize(char *str)
{
cap_loop
      LDRB  r1, [r0]	; Load byte into r1 from memory pointed to by r0 (str pointer)
      CMP   r1, #'a'-1  ; compare it with the character before 'a'
      BLS   cap_skip	; If byte is lower or same, then skip this byte
	
      CMP   r1, #'z'	; Compare it with the 'z' character
      BHI   cap_skip	; If it is higher, then skip this byte
	
      SUBS  r1,#32	; Else subtract out difference to capitalize it
      STRB  r1, [r0]	; Store the capitalized byte back in memory
cap_skip
      ADDS  r0, r0, #1; Increment str pointer
      CMP   r1, #0	; Was the byte 0?
      BNE   cap_loop	; If not, repeat the loop
      BX    lr		; Else return from subroutine
}

int32_t main()
{
    const char a[12] = "Hello world!";
    char b[12];
	
    SYS_Init();

    printf("\nStart SEMIHOST: \n");
    printf("CPU= %dHz\n", SystemCoreClock);
	
    // CPUAsm
    my_strcpy (a, b); // strcpy (assembly code)
    printf("%s\n",b); 
	
    my_capitalize(b); // capitalize (assembly code)
    printf("%s\n",b);

    while(1);
}
