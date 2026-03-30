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

int delete_baris(TextEditor *ed, int posisi) {
    int i;
    if (ed->jumlah_baris == 0) {
        printf("Dokumen sudah kosong.\n");
        return -1;
    }
    if (posisi < 0 || posisi >= ed->jumlah_baris) {
        printf("Nomor baris tidak valid: %d\n", posisi + 1);
        return -1;
    }

    free(ed->buffer[posisi]);
    ed->buffer[posisi] = NULL;

    for (i = posisi; i < ed->jumlah_baris - 1; i++)
    ed->buffer[i] = ed->buffer[i + 1];
    ed->buffer[ed->jumlah_baris - 1] = NULL;
    ed->jumlah_baris--;
    ed->is_modified = 1;

    if (ed->kursor_baris >= ed->jumlah_baris && ed->kursor_baris > 0)
    ed->kursor_baris = ed->jumlah_baris - 1;
    ed->kursor_kolom = 0;

    return 0;
}

int insert_karakter(TextEditor *ed, int baris, int kolom, char c) {
    char *buf, *tmp;
    size_t len;

    if (baris < 0 || baris >= ed->jumlah_baris) return -1;
    buf = ed->buffer[baris];
    len = strlen(buf);
    if (kolom < 0 || kolom > (int)len) return -1;

    tmp = (char *)realloc(buf, len + 2);
    if (tmp == NULL) { 
        fprintf(stderr, "Error: realloc gagal.\n"); 
        return -1; 
    }
    ed->buffer[baris] = tmp;

    memmove(&tmp[kolom + 1], &tmp[kolom], len - kolom + 1);
    tmp[kolom] = c;

    ed->is_modified = 1;
    ed->kursor_kolom = kolom + 1;
    return 0;
}

int delete_karakter(TextEditor *ed, int baris, int kolom) {
    char *buf, *tmp;
    size_t len;
    char *buf_atas, *gabung;
    int i;

    if (baris < 0 || baris >= ed->jumlah_baris) return -1;
    buf = ed->buffer[baris];
    len = strlen(buf);

    // hapus karakter di tengah/akhir baris
    if (kolom > 0 && kolom <= (int)len) {
        // geser sisa karakter ke kiri untuk menimpa karakter yang dihapus
        memmove(&buf[kolom - 1], &buf[kolom], len - kolom + 1);
        // sesuaikan ukuran memori setelah karakter hilang
        tmp = (char *)realloc(buf, len); 
        if (tmp != NULL) ed->buffer[baris] = tmp;

        ed->kursor_kolom = kolom - 1; 
        ed->is_modified  = 1;
        return 0;
    }

    return -1;
}

int go_to_line(TextEditor *ed, int nomor) {
    int idx = nomor - 1; // konversi nomor baris dari sisi user

    // jika kosong, tidak bisa navigasi
    if (ed->jumlah_baris == 0) { 
        printf("Dokumen kosong.\n"); 
        getchar();
        return -1; 
    }
    // validasi nomor baris
    if (idx < 0 || idx >= ed->jumlah_baris) {                              
        printf("Nomor baris %d tidak valid (1-%d).\n", nomor, ed->jumlah_baris);
        getchar();
        return -1;
    }
    // update posisi kursor
    ed->kursor_baris = idx; // pindahkan kursor ke baris yang diminta
    ed->kursor_kolom = 0;   // reset kolom ke awal baris 
    return 0;             
}