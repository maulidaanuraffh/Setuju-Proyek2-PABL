/* ============================================================
 * display.c 
 * Proyek 2 |  Ikhwan Syahid Azizy (013)
 * ============================================================ */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "editor.h"
#include "display.h"

/* clear_screen()*/

void clear_screen(void){
#ifdef _WIN32
    system("cls");
#else 
    system("clear")
#endif
}