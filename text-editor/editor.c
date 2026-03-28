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

int insert_baris(TextEditor *ed, int posisi, const char *isi) {
    int   i;
    char *baris_baru;

    if (ed->jumlah_baris >= MAX_BARIS) {
        printf("Dokumen penuh (%d baris maksimum).\n", MAX_BARIS);
        return -1;
    }
    if (posisi < 0 || posisi > ed->jumlah_baris) {
        printf("Posisi tidak valid: %d\n", posisi);
        return -1;
    }

    baris_baru = alokasi_baris(isi ? isi : "");
    if (baris_baru == NULL) return -1;

    // geser dari bawah ke atas 
    for (i = ed->jumlah_baris; i > posisi; i--)
    ed->buffer[i] = ed->buffer[i - 1];
    ed->buffer[posisi] = baris_baru;
    ed->jumlah_baris++;
    ed->is_modified  = 1;
    ed->kursor_baris = posisi;
    ed->kursor_kolom = 0;

    return 0;
}