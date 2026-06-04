/* ============================================================
   editor.h 
   Proyek 2 |  Maulida Nur Afifah  (017)
 * ============================================================ */
#ifndef EDITOR_H
#define EDITOR_H

#define MAX_FILEPATH 300
#define MAX_INPUT 4096
#define MAX_FILENAME 100
#define MAX_HASIL 2000 
#define MODE_PERINTAH 0 // nilai mode saat editor menunggu pilihan menu
#define MODE_INPUT 1    // nilai mode saat editor menerima input teks 

typedef struct RowNode {
    struct RowNode *prev_row;  // baris di atasnya           
    struct RowNode *next_row;  // baris di bawahnya          
    char *isi; // string baris, dialokasikan dengan malloc
} RowNode;

// struct posisi kemunculan keyword
typedef struct {
    RowNode *baris;  // pointer ke RowNode yang mengandung keyword 
    int nomor_baris; // nomor baris ke berapa 
    int kolom; // posisi karakter awal keyword dalam string  
} HasilCari;

// struct utama
typedef struct {
    RowNode *head; // baris pertama dokumen
    RowNode *tail; // baris terakhir dokumen
    RowNode *kursor_ptr; // pointer ke baris aktif utk akses cepat
    int kursor_baris; // untuk render baris aktif
    int kursor_kolom; // untuk indeks karakter dalam baris aktif
    char filepath[MAX_FILEPATH];
    char filename[MAX_FILENAME];
    int is_modified;
    int jumlah_baris;
    int mode;           
    int show_line_num;
    HasilCari hasil_cari[MAX_HASIL];
    int jumlah_hasil;
    int index_cari;
    char keyword_terakhir[256];
} TextEditor;
 
// helper utk RowNode
RowNode *buat_row_node(const char *teks);
void bebaskan_row_list(RowNode *head);
void bebaskan_semua_baris(TextEditor *ed);

// helper navigasi
RowNode *cari_node(const TextEditor *ed, int posisi);

// lifecycle editor
void init_editor(TextEditor *ed);

// operasi baris
int insert_baris(TextEditor *ed, int posisi, const char *isi);
int delete_baris(TextEditor *ed, int posisi);
int go_to_line(TextEditor *ed, int nomor);

// pencarian
int find_teks(TextEditor *ed, const char *keyword);
void find_next(TextEditor *ed);
int replace_teks(TextEditor *ed, const char *cari, const char *ganti);
void reset_hasil_cari(TextEditor *ed);
void word_count(const TextEditor *ed);

#endif