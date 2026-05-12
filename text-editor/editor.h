/* ============================================================
   editor.h 
   Proyek 2 |  Maulida Nur Afifah  (017)
 * ============================================================ */
#ifndef EDITOR_H
#define EDITOR_H

#define MAX_BARIS 100
#define MAX_KOLOM 101
#define MAX_FILEPATH 300
#define MAX_INPUT 4096
#define MAX_FILENAME 100
#define MAX_HASIL 2000 
#define MODE_PERINTAH 0 // nilai mode saat editor menunggu pilihan menu
#define MODE_INPUT 1    // nilai mode saat editor menerima input teks 

typedef struct CharNode {
    char data;  // satu karakter                  
    struct CharNode *prev_char;  // pointer ke karakter sebelumnya 
    struct CharNode *next_char;  // pointer ke karakter berikutnya 
} CharNode;

typedef struct RowNode {
    struct RowNode *prev_row;  // baris di atasnya           
    struct RowNode *next_row;  // baris di bawahnya          
    CharNode *head_row; // CharNode pertama di baris  
    CharNode *tail_row; // CharNode terakhir di baris 
    int jml_char;     // jumlah karakter
} RowNode;

// struct posisi kemunculan keyword
typedef struct {
    RowNode *baris;  // RowNode yang mengandung keyword 
    CharNode *kolom_node; // CharNode awal keyword  
    int panjang_keyword;  
} HasilCari;

// struct utama
typedef struct {
    RowNode *head;
    RowNode *tail;
    RowNode *kursor_baris;
    RowNode *kursor_kolom;
    char  filepath[MAX_FILEPATH];
    char  filename[MAX_FILENAME];
    int   is_modified;
    int   mode;           
    int   show_line_num;
    HasilCari  hasil_cari[MAX_HASIL];
    int jumlah_hasil;
    int index_cari;
    char keyword_terakhir[256];
} TextEditor;

CharNode *buat_char_node(char c);
void bebaskan_char_list(CharNode *head); 
RowNode *buat_row_node(const char *teks);
void bebaskan_row_list(RowNode *head);
char *row_ke_string(const RowNode *baris);

void init_editor(TextEditor *ed);

int insert_baris(TextEditor *ed, int posisi, const char *isi);
int delete_baris(TextEditor *ed, int posisi);
int go_to_line(TextEditor *ed, int nomor);

int find_teks(TextEditor *ed, const char *keyword);
void find_next(TextEditor *ed);
int replace_teks(TextEditor *ed, const char *cari, const char *ganti);
void reset_hasil_cari(TextEditor *ed);
void word_count(const TextEditor *ed);

#endif