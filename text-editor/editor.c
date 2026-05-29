/* ============================================================
   editor.c 
   Proyek 2 |  Maulida Nur Afifah (017) & Ikhwan Syahid Azizy (013)
 * ============================================================ */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "editor.h"

// Maulida Nur Afifah (017)
// buat satu RowNode baru dengan isi string yang disalin
RowNode *buat_row_node(const char *teks) {
    RowNode *baris;
    int i;
    int len;

    baris = (RowNode *)malloc(sizeof(RowNode));
    if (baris == NULL) {
        fprintf(stderr, "Error: gagal alokasi RowNode.\n");
        return NULL;
    }

    if (teks == NULL) teks = "";
    len = (int)strlen(teks);

    baris->isi = (char *)malloc(len + 1);
    if(baris->isi == NULL){
        fprintf(stderr, "Error: gagal alokasi isi baris.\n");
        free(baris);
        return NULL;
    }
    memcpy(baris->isi, teks, len);
    baris->isi[len] = '\0';

    baris->prev_row = NULL;
    baris->next_row = NULL;
    return baris;
}

// Ikhwan Syahid Azizy (013)
// bebaskan satu rantai RowNode mulai dari head
void bebaskan_row_list(RowNode *head) {
    RowNode *sekarang  = head;
    RowNode *berikutnya;

    while (sekarang != NULL) {
        berikutnya = sekarang->next_row;
        free(sekarang->isi);
        free(sekarang);
        sekarang = berikutnya;
    }
}

// Ikhwan Syahid Azizy (013)
// bebaskan semua baris di editor dan pointer
void bebaskan_semua_baris(TextEditor *ed){
    bebaskan_row_list(ed->head);
    ed->head = NULL;
    ed->tail = NULL;
    ed->kursor_ptr = NULL;
    ed->kursor_baris = 0;
    ed->kursor_kolom = 0;
    ed->jumlah_baris = 0;
}


// Maulida Nur Afifah (017)
void init_editor(TextEditor *ed) {
    ed->head = NULL;
    ed->tail = NULL;
    ed->kursor_ptr = NULL;
    ed->kursor_baris = 0;
    ed->kursor_kolom = 0;
    ed->jumlah_baris = 0;
    ed->filepath[0] = '\0';
    ed->filename[0] = '\0';
    ed->is_modified = 0;
    ed->jumlah_baris = 0; 
    ed->mode = MODE_PERINTAH;
    ed->show_line_num = 1;
    ed->jumlah_hasil = 0;
    ed->index_cari = 0;
    ed->keyword_terakhir[0] = '\0';
}


// Maulida Nur Afifah (017)
// cari node di posisi tertentu dengan traversal 2 arah
RowNode *cari_node(const TextEditor *ed, int posisi) {
    RowNode *node;
    int i;
 
    if (posisi < 0 || posisi >= ed->jumlah_baris) return NULL;
 
    if (posisi <= ed->jumlah_baris / 2) {
        node = ed->head;
        i = 0;
        while (i < posisi){
            node = node->next_row;
            i++;
        }
    } else {
        node = ed->tail;
        i = ed->jumlah_baris - 1;
        while (i > posisi){
            node = node->prev_row;
            ;
        }
    }
    return node;
}


//Maulida Nur Afifah (017)
void reset_hasil_cari(TextEditor *ed){
    ed->jumlah_hasil = 0;
    ed->index_cari = 0;
    ed->keyword_terakhir[0] = '\0';
}


//Maulida Nur Afifah (017)
int insert_baris(TextEditor *ed, int posisi, const char *isi) {
    RowNode *baris;
    RowNode *target;
    RowNode *prev;

    baris = buat_row_node(isi);
    if (baris == NULL) return -1;
 
    // KASUS 1: dokumen kosong 
    if (ed->head == NULL) {
        ed->head = baris;
        ed->tail = baris;
    }
    // KASUS 2: sisip di akhir 
    else if (posisi == ed->jumlah_baris) {
        baris->prev_row = ed->tail;
        ed->tail->next_row = baris;
        ed->tail = baris;
    }
    // KASUS 3: sisip di awal atau tengah
    else {
        target = cari_node(ed, posisi);
        prev = target->prev_row;
 
        baris->next_row = target;
        baris->prev_row = prev;
        target->prev_row = baris;
 
        if (prev != NULL) {
            prev->next_row = baris;
        } else {
            ed->head = baris; // sisip di awal: baris ini akan jadi head 
        }
    }
 
    ed->jumlah_baris++;
    ed->kursor_ptr = baris;
    ed->kursor_baris = posisi;
    ed->kursor_kolom = 0;
    ed->is_modified = 1;
    reset_hasil_cari(ed);
    return 0;
}


// Ikhwan Syahid Azizy (013)
int delete_baris(TextEditor *ed, int posisi) {
    RowNode *target;
    RowNode *prev;
    RowNode *next;
 
    if (ed->jumlah_baris == 0) {
        printf("Dokumen sudah kosong.\n");
        return -1;
    }
    if (posisi < 0 || posisi >= ed->jumlah_baris) {
        printf("Nomor baris tidak valid: %d\n", posisi + 1);
        return -1;
    }
 
    target = cari_node(ed, posisi);
    prev = target->prev_row;
    next = target->next_row;
 
    /* sambungkan tetangga kiri-kanan */
    if (prev != NULL) prev->next_row = next;
    else ed->head = next;
 
    if (next != NULL) next->prev_row = prev;
    else ed->tail = prev;
 
    /* bebaskan node target */
    free(target->isi);
    free(target);
 
    ed->jumlah_baris--;
    ed->is_modified = 1;
 
    /* posisikan ulang kursor */
    if (ed->jumlah_baris == 0) {
        ed->kursor_ptr = NULL;
        ed->kursor_baris = 0;
    } else if (posisi >= ed->jumlah_baris) {
        /* baris yang dihapus adalah baris terakhir, kursor mundur */
        ed->kursor_baris = ed->jumlah_baris - 1;
        ed->kursor_ptr = ed->tail;
    } else {
        /* kursor tetap di posisi, tapi node-nya sekarang adalah 'next' */
        ed->kursor_baris = posisi;
        ed->kursor_ptr = next;
    }
    ed->kursor_kolom = 0;
 
    reset_hasil_cari(ed);
    return 0;
}
   

//Maulida Nur Afifah (017)
int find_teks(TextEditor *ed, const char *keyword) {
    RowNode *node;
    int nomor = 0;
    size_t klen;
    char *ptr;
 
    ed->jumlah_hasil = 0;
    ed->index_cari = 0;
    ed->keyword_terakhir[0] = '\0';
 
    if (keyword == NULL || strlen(keyword) == 0) return 0;
 
    klen = strlen(keyword);
    strncpy(ed->keyword_terakhir, keyword, sizeof(ed->keyword_terakhir) - 1);
    ed->keyword_terakhir[sizeof(ed->keyword_terakhir) - 1] = '\0';
 
    // traversal semua baris dari head 
    node = ed->head;
    while (node != NULL && ed->jumlah_hasil < MAX_HASIL) {
        if (node->isi == NULL || node->isi[0] == '\0'){
            ptr = node->isi;

            while ((ptr = strstr(ptr, keyword)) != NULL) {
                if (ed->jumlah_hasil >= MAX_HASIL) break;
                ed->hasil_cari[ed->jumlah_hasil].baris = node;
                ed->hasil_cari[ed->jumlah_hasil].nomor_baris = nomor;
                ed->hasil_cari[ed->jumlah_hasil].kolom = (int)(ptr - node->isi);
                ed->jumlah_hasil++;
                ptr += klen;
            }
        }
    node = node->next_row; // pindah ke baris berikutnya
    nomor++; 
    }
 
    if (ed->jumlah_hasil > 0) {
        ed->kursor_ptr = ed->hasil_cari[0].baris;
        ed->kursor_baris = ed->hasil_cari[0].nomor_baris;
        ed->kursor_kolom = ed->hasil_cari[0].kolom;
        printf("Ditemukan %d kemunculan \"%s\".\n", ed->jumlah_hasil, keyword);
    } else {
        printf("Teks \"%s\" tidak ditemukan.\n", keyword);
    }
 
    return ed->jumlah_hasil;
}
 

//Maulida Nur Afifah (017)
void find_next(TextEditor *ed) {
    HasilCari *h;
 
    if (ed->jumlah_hasil == 0) {
        printf("Tidak ada hasil pencarian aktif. Gunakan fitur find text dulu.\n");
        return;
    }
 
    ed->index_cari++;
    if (ed->index_cari >= ed->jumlah_hasil) ed->index_cari = 0;
 
    h = &ed->hasil_cari[ed->index_cari];
    ed->kursor_ptr = h->baris;
    ed->kursor_baris = h->nomor_baris;
    ed->kursor_kolom = h->kolom;
 
    printf("Kemunculan %d/%d: baris %d, kolom %d\n",
           ed->index_cari + 1, ed->jumlah_hasil,
           h->nomor_baris + 1, h->kolom + 1);
}


// Ikhwan Syahid Azizy (013)
int replace_teks(TextEditor *ed, const char *cari, const char *ganti) {
    RowNode *node;
    int count = 0;
    size_t len_cari, len_ganti, len_lama, len_baru, used, prefix;
    char *src, *pos, *baris_baru;
 
    if (cari == NULL || strlen(cari) == 0) return -1;
    if (ganti == NULL) ganti = "";
 
    len_cari = strlen(cari);
    len_ganti = strlen(ganti);
 
    for (node = ed->head; node != NULL; node = node->next_row) {
        if (node->isi == NULL || node->isi[0] == '\0') continue;
        if (strstr(node->isi, cari) == NULL) continue;
 
        /* hitung dulu berapa kemunculan di baris ini */
        int n_match = 0;
        src = node->isi;
        while ((pos = strstr(src, cari)) != NULL) {
            n_match++;
            src = pos + len_cari;
        }
 
        if (n_match == 0) continue;
 
        /* alokasi baris baru: panjang lama - (n_match * len_cari) + (n_match * len_ganti) + 1 */
        len_lama = strlen(node->isi);
        len_baru = len_lama - (n_match * len_cari) + (n_match * len_ganti);
        baris_baru = (char *)malloc(len_baru + 1);
        if (baris_baru == NULL) {
            fprintf(stderr, "Error: gagal alokasi baris hasil replace.\n");
            continue;
        }
 
        /* bangun baris baru dengan substitusi */
        used = 0;
        src = node->isi;
        while ((pos = strstr(src, cari)) != NULL) {
            prefix = (size_t)(pos - src);
            memcpy(baris_baru + used, src, prefix);
            used += prefix;
            memcpy(baris_baru + used, ganti, len_ganti);
            used += len_ganti;
            src = pos + len_cari;
            count++;
        }
        /* salin sisa setelah kemunculan terakhir */
        prefix = strlen(src);
        memcpy(baris_baru + used, src, prefix);
        used += prefix;
        baris_baru[used] = '\0';
 
        /* ganti isi baris dengan yang baru */
        free(node->isi);
        node->isi = baris_baru;
    }
 
    if (count > 0) {
        ed->is_modified = 1;
        printf("Berhasil mengganti %d kemunculan \"%s\" menjadi \"%s\".\n",
               count, cari, ganti);
        reset_hasil_cari(ed);
    } else {
        printf("Teks \"%s\" tidak ditemukan.\n", cari);
    }
 
    return count;
}


// Ikhwan Syahid Azizy (013)
void word_count(const TextEditor *ed) {
    RowNode *node;
    int total_kata = 0, total_char = 0, dalam_kata = 0;
    int j;
    unsigned char c;
 
    for (node = ed->head; node != NULL; node = node->next_row) {
        if (node->isi == NULL || node->isi[0] == '\0') continue;
 
        j = 0;
        while ((c = (unsigned char)node->isi[j]) != '\0') {
            total_char++;
            if (c == ' ' || c == '\t') {
                if (dalam_kata) {
                    total_kata++;
                    dalam_kata = 0;
                }
            } else {
                dalam_kata = 1;
            }
            j++;
        }
        if (dalam_kata) {
            total_kata++;
            dalam_kata = 0;
        }
    }
 
    printf("Statistik dokumen:\n");
    printf("  Jumlah baris    : %d\n", ed->jumlah_baris);
    printf("  Jumlah kata     : %d\n", total_kata);
    printf("  Jumlah karakter : %d\n", total_char);
}


// Ikhwan Syahid Azizy (013)
int go_to_line(TextEditor *ed, int nomor) {
    int idx = nomor - 1;
    RowNode *node;
 
    if (ed->jumlah_baris == 0) {
        printf("Dokumen kosong.\n");
        getchar();
        return -1;
    }
    if (idx < 0 || idx >= ed->jumlah_baris) {
        printf("Nomor baris %d tidak valid (1-%d).\n", nomor, ed->jumlah_baris);
        getchar();
        return -1;
    }
 
    node = cari_node(ed, idx);
    if (node == NULL) return -1;
 
    ed->kursor_ptr = node;
    ed->kursor_baris = idx;
    ed->kursor_kolom = 0;
    return 0;
}