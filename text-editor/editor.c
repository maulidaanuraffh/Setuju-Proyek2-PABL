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

int find_teks(TextEditor *ed, const char *keyword) {
    int i;
    size_t klen;
    char *ptr, *baris;

    // utk reset status pencarian sblmnya
    ed->jumlah_hasil = 0; // menghapus jml temuan sblmnya
    ed->index_cari = 0; // mengatur ulang navigasi pencarian
    ed->keyword_terakhir[0] = '\0'; // mengosongkan memori kata kunci terakhir

    // jika keyword yg dicari tidak ada, pencarian berhenti
    if (keyword == NULL || strlen(keyword) == 0) return 0;

    // menyimpan keyword ke memori utk fitur find next nanti
    klen = strlen(keyword); 
    strncpy(ed->keyword_terakhir, keyword, sizeof(ed->keyword_terakhir) - 1);
    ed->keyword_terakhir[sizeof(ed->keyword_terakhir) - 1] = '\0';
    
    // menelusuri baris demi baris
    for (i = 0; i < ed->jumlah_baris && ed->jumlah_hasil < MAX_HASIL; i++) {
        baris = ed->buffer[i];
        if (baris == NULL) continue; // skip baris kosong

        ptr = baris;
        // mencari keyword di baris yg sedang aktif
        while ((ptr = strstr(ptr, keyword)) != NULL) {
            if (ed->jumlah_hasil >= MAX_HASIL) break; // berhenti jika penampung hasil pencarian sdh penuh
            // menyimpan posisi baris dan kolom keyword yg ditemukan
            ed->hasil_cari[ed->jumlah_hasil].baris = i;
            ed->hasil_cari[ed->jumlah_hasil].kolom = (int)(ptr - baris);
            ed->jumlah_hasil++;
            ptr += klen; // geser pointer lanjut mencari kemunculan kata yang sama di baris yang sama
        }
    }
    // ketika keyword ditemukan kursor pindah ke lokasi pertama
    if (ed->jumlah_hasil > 0) {
        ed->kursor_baris = ed->hasil_cari[0].baris;
        ed->kursor_kolom = ed->hasil_cari[0].kolom;
        printf("Ditemukan %d kemunculan \"%s\".\n", ed->jumlah_hasil, keyword);
    } else {
        printf("Teks \"%s\" tidak ditemukan.\n", keyword);
    }

    return ed->jumlah_hasil;
}

void find_next(TextEditor *ed) {
    HasilCari *h;

    if (ed->jumlah_hasil == 0) {
        printf("Tidak ada hasil pencarian aktif. Gunakan fitur find text dulu.\n");
        return;
    }

    ed->index_cari++; // maju ke hasil berikutnya
    // jika sdh sampai hasil terakhir, balik lagi ke hasil pertama
    if (ed->index_cari >= ed->jumlah_hasil) { 
        ed->index_cari = 0;
    }
    // ambil koordinat dari array hasil_cari berdasarkan indeks
    h = &ed->hasil_cari[ed->index_cari];
    ed->kursor_baris = h->baris;
    ed->kursor_kolom = h->kolom;
    // menampilkan informasi urutan temuan
    printf("Kemunculan %d/%d: baris %d, kolom %d\n",
        ed->index_cari + 1, ed->jumlah_hasil, // ditambah 1 krn tampilan untuk user dimulai dari 1 bkn 0
        h->baris + 1, h->kolom + 1);
}

int replace_teks(TextEditor *ed, const char *cari, const char *ganti) {
    int i, count = 0;
    size_t len_cari, len_ganti;
    char *src, *pos, *hasil;
    size_t cap, used, prefix;

    // memastikan kata yang dicari tidak null
    if (cari == NULL || strlen(cari) == 0) return -1;
    // jika pengganti null, anggap sebagai string kosong (menghapus kata)
    if (ganti == NULL) ganti = "";

    // menghitung panjang string yg dicari & string pengganti
    len_cari = strlen(cari);
    len_ganti = strlen(ganti);

    for (i = 0; i < ed->jumlah_baris; i++) {
        if (ed->buffer[i] == NULL) continue;
        if (strstr(ed->buffer[i], cari) == NULL) continue;

        // menyiapkan buffer hasil utk menampung teks baru
        cap = strlen(ed->buffer[i]) * 2 + 64; // alokasi buffer hasil di heap dengan kapasitas awal 2x panjang baris + margin 64 byte
        hasil = (char *)malloc(cap);
        if (hasil == NULL) return -1;
        used = 0;
        src = ed->buffer[i]; // menunjuk ke posisi baca saat ini di baris asli
        // cari dan ganti semua kemunculan berikutnya
        while ((pos = strstr(src, cari)) != NULL) {
            prefix = (size_t)(pos - src); // menghitungg pjg teks sblm kemunculan kata yg ingin diganti

            // memastikan buffer hasil cukup utk menampung prefix dan kata pengganti
            while (used + prefix + len_ganti + 1 >= cap) {
                char *tmp;
                cap *= 2;
                tmp = (char *)realloc(hasil, cap);
                if (tmp == NULL) { free(hasil); return -1; }
                hasil = tmp;
            }
            // menyalin teks sebelum keyword yang ditemukan
            memcpy(hasil + used, src, prefix);
            used += prefix;
            // menyalin kata pengganti ke buffer hasil
            memcpy(hasil + used, ganti, len_ganti);
            used += len_ganti;
            // geser posisi baca melewati kemunculan keyword yang baru diganti
            src = pos + len_cari;
            count++;
        }

        // menyalin sisa teks di baris tsb setelah kemunculan terakhir kata yg dicari
        prefix = strlen(src);
        while (used + prefix + 1 >= cap) {
            char *tmp;
            cap *= 2;
            tmp = (char *)realloc(hasil, cap);
            if (tmp == NULL) { free(hasil); return -1; }
            hasil = tmp;
        }
        memcpy(hasil + used, src, prefix + 1); // +1 utk menyertakan null terminator
        // bebaskan memori baris lama dan ganti dengan baris baru hasil modifikasi
        free(ed->buffer[i]);
        ed->buffer[i] = hasil;
    }

    if (count > 0) {
        ed->is_modified = 1;
        printf("Berhasil mengganti %d kemunculan \"%s\" menjadi \"%s\".\n",
               count, cari, ganti);
    } else {
        printf("Teks \"%s\" tidak ditemukan.\n", cari);
    }

    return count;
}