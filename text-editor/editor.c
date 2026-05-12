/* ============================================================
   editor.c 
   Proyek 2 |  Maulida Nur Afifah  (017)
 * ============================================================ */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "editor.h"

CharNode *buat_char_node(char c) {
    CharNode *baru = (CharNode *)malloc(sizeof(CharNode));
    if (baru == NULL) {
        fprintf(stderr, "Error: gagal alokasi CharNode.\n");
        return NULL;
    }
    baru->data = c;
    baru->prev_char = NULL;
    baru->next_char = NULL;
    return baru;
}

RowNode *buat_row_node(const char *teks) {
    RowNode *baris;
    CharNode *cn;
    int i;
    int len;

    baris = (RowNode *)malloc(sizeof(RowNode));
    if (baris == NULL) {
        fprintf(stderr, "Error: gagal alokasi RowNode.\n");
        return NULL;
    }

    baris->prev_row = NULL;
    baris->next_row = NULL;
    baris->head_row = NULL;
    baris->tail_row = NULL;
    baris->jml_char = 0;

    if (teks == NULL) teks = "";
    len = (int)strlen(teks);

    /* buat CharNode satu per satu, sambungkan ke ekor */
    for (i = 0; i < len; i++) {
        cn = buat_char_node(teks[i]);
        if (cn == NULL) {
            /* gagal di tengah — bebaskan yang sudah dibuat */
            bebaskan_char_list(baris->head_row);
            free(baris);
            return NULL;
        }

        if (baris->head_row == NULL) {
            /* karakter pertama */
            baris->head_row = cn;
            baris->tail_row   = cn;
        } else {
            /* sambung ke ekor */
            cn->prev_char = baris->tail_row;
            baris->tail_row->next_char = cn;
            baris->tail_row = cn;
        }
        baris->jml_char++;
    }

    return baris;
}

void init_editor(TextEditor *ed) {
    ed->head = NULL;
    ed->tail = NULL;
    ed->kursor_baris = NULL;
    ed->kursor_kolom = NULL;
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

void reset_buffer(TextEditor *ed) {
    int i;
    for (i = 0; i < ed->jumlah_baris; i++) ed->buffer[i][0] = '\0';
    ed->jumlah_baris = 0;
    ed->is_modified = 0;
    ed->kursor_baris = 0;
    ed->kursor_kolom = 0;
    ed->jumlah_hasil = 0;
    ed->index_cari = 0;
    ed->keyword_terakhir[0] = '\0';
    for (i = 0; i < MAX_BARIS; i++) ed->is_wrapped[i] = 0;
}

void reset_hasil_cari(TextEditor *ed) { // dipanggil setiap buffer diubah agar hasil cari tdk stale 
    ed->jumlah_hasil = 0;
    ed->index_cari = 0;
    ed->keyword_terakhir[0] = '\0';
}

int insert_baris(TextEditor *ed, int posisi, const char *isi) {
    int   i;

    if (ed->jumlah_baris >= MAX_BARIS) {
        printf("Dokumen penuh (%d baris maksimum).\n", MAX_BARIS);
        return -1;
    }
    if (posisi < 0 || posisi > ed->jumlah_baris) {
        printf("Posisi tidak valid: %d\n", posisi);
        return -1;
    }

   // geser dari bawah ke atas 
    for (i = ed->jumlah_baris; i > posisi; i--)
    memcpy(ed->buffer[i], ed->buffer[i - 1], MAX_KOLOM);
 
    //isi baris baru di posisi yang sudah dikosongkan 
    if (isi != NULL) {
        strncpy(ed->buffer[posisi], isi, MAX_KOLOM - 1);
        ed->buffer[posisi][MAX_KOLOM - 1] = '\0'; 
    } else {
        ed->buffer[posisi][0] = '\0'; 
    }

    ed->jumlah_baris++;
    ed->is_modified  = 1;
    ed->kursor_baris = posisi;
    ed->kursor_kolom = 0;

    reset_hasil_cari(ed);
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

    for (i = posisi; i < ed->jumlah_baris - 1; i++)
    memcpy(ed->buffer[i], ed->buffer[i + 1], MAX_KOLOM);
    
    ed->buffer[ed->jumlah_baris - 1][0] = '\0';
    ed->jumlah_baris--;
    ed->is_modified = 1;

    if (ed->kursor_baris >= ed->jumlah_baris && ed->kursor_baris > 0)
    ed->kursor_baris = ed->jumlah_baris - 1;
    ed->kursor_kolom = 0;

    reset_hasil_cari(ed);
    return 0;
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
        if (baris[0] == '\0') continue; // skip baris kosong

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
    int used, i, count = 0;
    size_t len_cari, len_ganti;
    char *src, *pos;
    char tmp_baris[MAX_KOLOM]; //buffer sementara di stack
    size_t prefix;

    // memastikan kata yang dicari tidak null
    if (cari == NULL || strlen(cari) == 0) return -1;
    // jika pengganti null, anggap sebagai string kosong (menghapus kata)
    if (ganti == NULL) ganti = "";

    // menghitung panjang string yg dicari & string pengganti
    len_cari = strlen(cari);
    len_ganti = strlen(ganti);

    for (i = 0; i < ed->jumlah_baris; i++) {
        if (ed->buffer[i][0] == '\0') continue;
        if (strstr(ed->buffer[i], cari) == NULL) continue;

        // bsngun baris hasil di tmp_baris
        tmp_baris[0] = '\0';
        used = 0;
        src = ed->buffer[i]; // menunjuk ke posisi baca saat ini di baris asli
        // cari dan ganti semua kemunculan berikutnya
        while ((pos = strstr(src, cari)) != NULL) {
            prefix = (size_t)(pos - src); // menghitungg pjg teks sblm kemunculan kata yg ingin diganti

            // cek apa hasil masih muat di MAX_KOLOM
           if (used + (int)prefix + (int)len_ganti >= MAX_KOLOM - 1) {
                // hasil akan melebihi kapasitas baris
                printf("Peringatan: hasil replace di baris %d melebihi %d karakter, dipotong.\n",
                    i + 1, MAX_KOLOM - 1);
                break; 
        	}
        	
        	// salin teks sebelum kemunculan keyword
            memcpy(tmp_baris + used, src, prefix);
            used += (int)prefix;
 
            // salin teks pengganti 
            memcpy(tmp_baris + used, ganti, len_ganti);
            used += (int)len_ganti;
 
            src = pos + len_cari; // geser posisi baca melewati kemunculan
            count++;
		}
        	
        // menyalin sisa teks di baris tsb setelah kemunculan terakhir kata yg dicari
        prefix = strlen(src);
        if (used + (int)prefix < MAX_KOLOM - 1) {
            memcpy(tmp_baris + used, src, prefix + 1); /* +1 untuk null terminator */
            used += (int)prefix;
        } else {
            // potong jika tdk muat
            memcpy(tmp_baris + used, src, MAX_KOLOM - 1 - used);
            tmp_baris[MAX_KOLOM - 1] = '\0';
        }
 
        // salin hasil kembali ke buffer[i]
        strncpy(ed->buffer[i], tmp_baris, MAX_KOLOM - 1);
        ed->buffer[i][MAX_KOLOM - 1] = '\0';
    }

    if (count > 0) {
        ed->is_modified = 1;
        printf("Berhasil mengganti %d kemunculan \"%s\" menjadi \"%s\".\n", count, cari, ganti);
        reset_hasil_cari(ed);
    } else {
        printf("Teks \"%s\" tidak ditemukan.\n", cari);
    }

    return count;
}

void word_count(const TextEditor *ed) {
    int i, j;
    int total_kata = 0, total_char = 0, dalam_kata = 0;
    unsigned char c;

    for (i = 0; i < ed->jumlah_baris; i++) {
        if (ed->buffer[i][0] == '\0') continue;
        j = 0;
        // membaca karakter satu per satu hingga bertemu null terminator
        while ((c = (unsigned char)ed->buffer[i][j]) != '\0') {
            total_char++; // menghitung setiap karakter termasuk spasi
            // cek apakah karakter saat ini adalah pemisah (spasi atau tab)
            if (c == ' ' || c == '\t') {
                // spasi ini menandakan akhir dari satu kata
                if (dalam_kata) { 
                    total_kata++; 
                    dalam_kata = 0; // reset status karena sekarang di luar kata
                } 
            } else { // jika karakter bukan spasi/tab, berarti skrg berada di dalam kata
                dalam_kata = 1;
            }
            j++;
        }
        // penanganan akhir baris, baris berakhir dan masih dalam status di dalam kata maka dihitung sbg 1 kata terakhir di bais tsb
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