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

    buf[strcspn(buf, "\r\n")] = '\0';

    if ((int)strlen(buf) == max - 1) flush_stdin();

     return (int)strlen(buf);
}

/*   edit_baris_inline()   */
int edit_baris_inline(char *buf, int max, const char *isi_lama) {
    int len;

    if (buf == NULL || max <= 0) return -1;

    /* tampilkan isi lama di baris */
    printf("  Isi lama  : ");
    if (isi_lama && strlen(isi_lama) > 0) {
        printf("%s\n", isi_lama);
    } else {
        printf("(kosong)\n");
    }
    printf("  Isi baru  : ");
    fflush(stdout);

    if (fgets(buf, max, stdin) == NULL) {
        buf[0] = '\0';
        return -1;
    }

    buf[strcspn(buf, "\r\n")] = '\0';
    if ((int)strlen(buf) == max - 1) flush_stdin();

    len = (int)strlen(buf);

    /* Enter kosong=batalkan, pertahankan isi lama */
    if (len == 0) {
        if (isi_lama) strncpy(buf, isi_lama, max - 1);
        buf[max - 1] = '\0';
        printf("  (tidak ada perubahan)\n");
        return 0; /* 0 = tidak berubah, berbeda dari -1 = error */
    }

    return len;
}
