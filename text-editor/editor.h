/* ============================================================
   editor.h 
   Proyek 2 |  Maulida Nur Afifah  (017)
 * ============================================================ */
#ifndef EDITOR_H

#define EDITOR_H
#define MAX_BARIS 500
#define MAX_PATH 300
#define MAX_INPUT 4096
#define MAX_FILENAME 100
#define MAX_HASIL 2000 
#define MODE_PERINTAH 0 // nilai mode saat editor menunggu pilihan menu
#define MODE_INPUT 1    // nilai mode saat editor menerima input teks 

// struct posisi kemunculan keyword
typedef struct {
    int baris;   
    int kolom;   
} HasilCari;

// struct utama
typedef struct {
    char *buffer[MAX_BARIS];
    int   jumlah_baris;
    char  filepath[MAX_PATH];
    char  filename[MAX_FILENAME];
    int   is_modified;
    int   kursor_baris;   
    int   kursor_kolom;
    int   mode;           
    int   show_line_num;

    HasilCari  hasil_cari[MAX_HASIL];
    int jumlah_hasil;
    int index_cari;
    char keyword_terakhir[256];
} TextEditor;

void init_editor(TextEditor *ed);
void bebaskan_buffer(TextEditor *ed);
char *alokasi_baris(const char *teks);
int insert_baris(TextEditor *ed, int posisi, const char *isi);
int delete_baris(TextEditor *ed, int posisi);

#endif