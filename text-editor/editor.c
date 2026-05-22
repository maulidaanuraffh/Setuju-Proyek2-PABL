/* ============================================================
   editor.c 
   Proyek 2 |  Maulida Nur Afifah  (017)
 * ============================================================ */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "editor.h"

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

void reset_hasil_cari(TextEditor *ed){
    ed->jumlah_hasil = 0;
    ed->index_cari = 0;
    ed->keyword_terakhir[0] = '\0';
}

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

// int delete_baris(TextEditor *ed, int posisi) {
//     int i;
//     if (ed->jumlah_baris == 0) {
//         printf("Dokumen sudah kosong.\n");
//         return -1;
//     }
//     if (posisi < 0 || posisi >= ed->jumlah_baris) {
//         printf("Nomor baris tidak valid: %d\n", posisi + 1);
//         return -1;
//     }

//     for (i = posisi; i < ed->jumlah_baris - 1; i++)
//     memcpy(ed->buffer[i], ed->buffer[i + 1], MAX_KOLOM);
    
//     ed->buffer[ed->jumlah_baris - 1][0] = '\0';
//     ed->jumlah_baris--;
//     ed->is_modified = 1;

//     if (ed->kursor_baris >= ed->jumlah_baris && ed->kursor_baris > 0)
//     ed->kursor_baris = ed->jumlah_baris - 1;
//     ed->kursor_kolom = 0;

//     reset_hasil_cari(ed);
//     return 0;
// }
   
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

// int replace_teks(TextEditor *ed, const char *cari, const char *ganti) {
//     int used, i, count = 0;
//     size_t len_cari, len_ganti;
//     char *src, *pos;
//     char tmp_baris[MAX_KOLOM]; //buffer sementara di stack
//     size_t prefix;

//     // memastikan kata yang dicari tidak null
//     if (cari == NULL || strlen(cari) == 0) return -1;
//     // jika pengganti null, anggap sebagai string kosong (menghapus kata)
//     if (ganti == NULL) ganti = "";

//     // menghitung panjang string yg dicari & string pengganti
//     len_cari = strlen(cari);
//     len_ganti = strlen(ganti);

//     for (i = 0; i < ed->jumlah_baris; i++) {
//         if (ed->buffer[i][0] == '\0') continue;
//         if (strstr(ed->buffer[i], cari) == NULL) continue;

//         // bsngun baris hasil di tmp_baris
//         tmp_baris[0] = '\0';
//         used = 0;
//         src = ed->buffer[i]; // menunjuk ke posisi baca saat ini di baris asli
//         // cari dan ganti semua kemunculan berikutnya
//         while ((pos = strstr(src, cari)) != NULL) {
//             prefix = (size_t)(pos - src); // menghitungg pjg teks sblm kemunculan kata yg ingin diganti

//             // cek apa hasil masih muat di MAX_KOLOM
//            if (used + (int)prefix + (int)len_ganti >= MAX_KOLOM - 1) {
//                 // hasil akan melebihi kapasitas baris
//                 printf("Peringatan: hasil replace di baris %d melebihi %d karakter, dipotong.\n",
//                     i + 1, MAX_KOLOM - 1);
//                 break; 
//         	}
        	
//         	// salin teks sebelum kemunculan keyword
//             memcpy(tmp_baris + used, src, prefix);
//             used += (int)prefix;
 
//             // salin teks pengganti 
//             memcpy(tmp_baris + used, ganti, len_ganti);
//             used += (int)len_ganti;
 
//             src = pos + len_cari; // geser posisi baca melewati kemunculan
//             count++;
// 		}
        	
//         // menyalin sisa teks di baris tsb setelah kemunculan terakhir kata yg dicari
//         prefix = strlen(src);
//         if (used + (int)prefix < MAX_KOLOM - 1) {
//             memcpy(tmp_baris + used, src, prefix + 1); /* +1 untuk null terminator */
//             used += (int)prefix;
//         } else {
//             // potong jika tdk muat
//             memcpy(tmp_baris + used, src, MAX_KOLOM - 1 - used);
//             tmp_baris[MAX_KOLOM - 1] = '\0';
//         }
 
//         // salin hasil kembali ke buffer[i]
//         strncpy(ed->buffer[i], tmp_baris, MAX_KOLOM - 1);
//         ed->buffer[i][MAX_KOLOM - 1] = '\0';
//     }

//     if (count > 0) {
//         ed->is_modified = 1;
//         printf("Berhasil mengganti %d kemunculan \"%s\" menjadi \"%s\".\n", count, cari, ganti);
//         reset_hasil_cari(ed);
//     } else {
//         printf("Teks \"%s\" tidak ditemukan.\n", cari);
//     }

//     return count;
// }

// void word_count(const TextEditor *ed) {
//     int i, j;
//     int total_kata = 0, total_char = 0, dalam_kata = 0;
//     unsigned char c;

//     for (i = 0; i < ed->jumlah_baris; i++) {
//         if (ed->buffer[i][0] == '\0') continue;
//         j = 0;
//         // membaca karakter satu per satu hingga bertemu null terminator
//         while ((c = (unsigned char)ed->buffer[i][j]) != '\0') {
//             total_char++; // menghitung setiap karakter termasuk spasi
//             // cek apakah karakter saat ini adalah pemisah (spasi atau tab)
//             if (c == ' ' || c == '\t') {
//                 // spasi ini menandakan akhir dari satu kata
//                 if (dalam_kata) { 
//                     total_kata++; 
//                     dalam_kata = 0; // reset status karena sekarang di luar kata
//                 } 
//             } else { // jika karakter bukan spasi/tab, berarti skrg berada di dalam kata
//                 dalam_kata = 1;
//             }
//             j++;
//         }
//         // penanganan akhir baris, baris berakhir dan masih dalam status di dalam kata maka dihitung sbg 1 kata terakhir di bais tsb
//         if (dalam_kata) { 
//             total_kata++; 
//             dalam_kata = 0; 
//         }
//     }

//     printf("Statistik dokumen:\n");
//     printf("  Jumlah baris    : %d\n", ed->jumlah_baris);
//     printf("  Jumlah kata     : %d\n", total_kata);
//     printf("  Jumlah karakter : %d\n", total_char);
// }

// int go_to_line(TextEditor *ed, int nomor) {
//     int idx = nomor - 1; // konversi nomor baris dari sisi user
//     // jika kosong, tidak bisa navigasi
//     if (ed->jumlah_baris == 0) { 
//         printf("Dokumen kosong.\n"); 
//         getchar();
//         return -1; 
//     }
//     // validasi nomor baris
//     if (idx < 0 || idx >= ed->jumlah_baris) {                              
//         printf("Nomor baris %d tidak valid (1-%d).\n", nomor, ed->jumlah_baris);
//         getchar();
//         return -1;
//     }
//     // update posisi kursor
//     ed->kursor_baris = idx; // pindahkan kursor ke baris yang diminta
//     ed->kursor_kolom = 0;   // reset kolom ke awal baris 
//     return 0;      
// }