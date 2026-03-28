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

/*   render_status_bar  ()   */
void render_status_bar(const TextEditor *ed){
    const char *nama =(strlen(ed->filename) > 0) ? ed->filename : "[Tanpa Nama]";
    const char *ubah =ed->is_modified ? "[*]" : "";
    const char *mode_str =(ed->mode == MODE_INPUT) ? " | INPUT MODE |" : "";

    printf("%-80s\n", "________________________________________________________________________________");
        printf(" %s%s  |  Baris: %d/%d  |  Kolom: %d%s\n",
           ubah, nama,
            ed->jumlah_baris > 0 ? ed->kursor_baris + 1 : 0,
            ed->jumlah_baris,
            ed->kursor_kolom,
            mode_str);

    /* tampilkan info pencarian jika ada hasil aktif */
    if (ed->jumlah_hasil > 0 && strlen(ed->keyword_terakhir) > 0) {
        printf(" Cari: \"%s\"  —  %d kemunculan  |  aktif: %d/%d "
               "(baris %d, kolom %d)\n",
                ed->keyword_terakhir,
                ed->jumlah_hasil,
                ed->index_cari + 1,
                ed->jumlah_hasil,
                ed->hasil_cari[ed->index_cari].baris + 1,
                ed->hasil_cari[ed->index_cari].kolom + 1);
    }

    printf("%-80s\n", "________________________________________________________________________________");
}
