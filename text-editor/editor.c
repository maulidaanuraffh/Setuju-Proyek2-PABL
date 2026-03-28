/* ============================================================
   editor.c 
   Proyek 2 |  Maulida Nur Afifah  (017)
 * ============================================================ */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "editor.h"

void init_editor(TextEditor *ed) {
    int i;
    for (i = 0; i < MAX_BARIS; i++) 
    ed->buffer[i] = NULL;
    ed->jumlah_baris = 0;
    ed->filepath[0] = '\0';
    ed->filename[0] = '\0';
    ed->is_modified = 0;
    ed->kursor_baris = 0;
    ed->kursor_kolom = 0;
    ed->mode = MODE_PERINTAH;
    ed->show_line_num = 1;
    ed->jumlah_hasil = 0;
    ed->index_cari = 0;
    ed->keyword_terakhir[0] = '\0';
}

void bebaskan_buffer(TextEditor *ed) {
    int i;
    for (i = 0; i < ed->jumlah_baris; i++) {
        if (ed->buffer[i] != NULL) {
            free(ed->buffer[i]);
            ed->buffer[i] = NULL;
        }
    }
    ed->jumlah_baris = 0;
    ed->is_modified = 0;
    ed->kursor_baris = 0;
    ed->kursor_kolom = 0;
    ed->jumlah_hasil = 0;
    ed->index_cari = 0;
    ed->keyword_terakhir[0] = '\0';
}

char *alokasi_baris(const char *teks) {
    size_t len;
    char  *hasil;

    if (teks == NULL) return NULL;
    len   = strlen(teks);
    hasil = (char *)malloc(len + 1);
    if (hasil == NULL) {
        fprintf(stderr, "Error: gagal alokasi memori baris.\n");
        return NULL;
    }
    strcpy(hasil, teks);
    return hasil;
}