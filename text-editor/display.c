/* ============================================================
 * display.c 
 * Proyek 2 |  Ikhwan Syahid Azizy (013)
 * ============================================================ */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "editor.h"
#include "display.h"

/*   clear_screen()   */
void clear_screen(void){
#ifdef _WIN32
    system("cls");
#else 
    system("clear");
#endif
}

/*   flush_stdin()   */
void flush_stdin(void){
    int c;
    while ((c = getchar()) != '\n' && c != EOF)
        ;
}

/*   baca_baris_aman()   */
int baca_baris_aman(char *buf, int max ){
    if (buf == NULL || max <=0 ) return -1;
    if (fgets (buf, max, stdin) == NULL){
        buf[0] = '\0';
        return -1;
    }

    /* FIX: strip \r\n (Windows & Unix) */
    buf[strcspn(buf, "\r\n")] = '\0';

    /* deteksi overflow: ada sisa di stdin */
    if ((int)strlen(buf) == max - 1) flush_stdin();
}
