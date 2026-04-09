/* ============================================================
 * filemanager.c 
 * Proyek 2 |  Ikhwan Syahid Azizy (013)
 * ============================================================ */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "editor.h"
#include "filemanager.h"
#include "display.h"

/*   ambil_nama_file   */

char *ambil_nama_file(const char *path){
    char *s1, *s2, *terakhir;

    if(path == NULL) return NULL;

    s1=strrchr(path, '/');
    s2=strrchr(path, '\\');

    
    if (s1 == NULL && s2 == NULL) return (char *)path;
    if (s1 == NULL)  terakhir = s2;
    else if (s2 == NULL) terakhir = s1;
    else terakhir = (s1 > s2) ? s1 : s2;

    return terakhir + 1;
}

/*   file_ada()   */

int file_ada(const char *path) {
    FILE *fp;
    if (path == NULL || strlen(path) == 0) return 0;
    fp = fopen(path, "r");
    if (fp == NULL) return 0;
    fclose(fp);
    return 1;
}

/*   open_file()   */

int open_file(TextEditor *ed, const char *path) {
    FILE *fp;
    char  baris[MAX_INPUT];
    char *ptr;
    int   n = 0;

    if (path == NULL || strlen(path) == 0) {
        printf("Path tidak boleh kosong.\n");
        return -1;
    }

    fp = fopen(path, "r");
    if (fp == NULL) {
        printf("Error: tidak bisa membuka \"%s\".\n", path);
        return -1;
    }

    bebaskan_buffer(ed);

    while (fgets(baris, sizeof(baris), fp) != NULL && n < MAX_BARIS) {
        /* FIX: strip \r\n (Windows) maupun \n (Unix) */
        baris[strcspn(baris, "\r\n")] = '\0';

        ed->buffer[n] = alokasi_baris(baris);
        if (ed->buffer[n] == NULL) {
            fclose(fp);
            ed->jumlah_baris = n;
            bebaskan_buffer(ed);
            printf("Error: kehabisan memori saat membuka file.\n");
            return -1;
        }
        n++;
    }

    fclose(fp);
    ed->jumlah_baris = n;
    ed->is_modified  = 0;
    ed->kursor_baris = 0;
    ed->kursor_kolom = 0;

    strncpy(ed->filepath, path, MAX_FILEPATH - 1);
    ed->filepath[MAX_FILEPATH - 1] = '\0';

    ptr = ambil_nama_file(path);
    strncpy(ed->filename, ptr ? ptr : path, MAX_FILENAME - 1);
    ed->filename[MAX_FILENAME - 1] = '\0';

    printf("File \"%s\" dibuka (%d baris).\n", ed->filename, n);
    return 0;
}

/* filemanager.c */

int save_file(TextEditor *ed) {
    char path_baru[MAX_FILEPATH];
    char nama_file[MAX_FILENAME];
    char folder_tujuan[MAX_FILEPATH];
    char pilihan[4];

    // Jika file belum pernah disimpan sama sekali (pengaman tambahan)
    if (strlen(ed->filepath) == 0) {
        if (navigasi_path_custom(folder_tujuan)) {
            printf("Masukkan nama file baru: ");
            fflush(stdout);
            if (baca_baris_aman(nama_file, sizeof(nama_file)) > 0) {
                snprintf(path_baru, sizeof(path_baru), "%s/%s", folder_tujuan, nama_file);
                return save_as(ed, path_baru);
            }
        }
        return -1;
    }

    // MENU PILIHAN UNTUK FILE YANG SUDAH ADA
    ed->mode = MODE_INPUT;
    render_layar(ed);
    printf("____________________________________________\n");
    printf("File aktif: \"%s\"\n", ed->filename);
    printf("____________________________________________\n");
    printf("[1] Save    - Timpa file lama (Langsung simpan)\n"); // Fokus perbaikan di sini
    printf("[2] Save As - Simpan di lokasi/nama baru\n");
    printf("____________________________________________\n");
    printf("Pilih (1/2): ");
    fflush(stdout);

    if (baca_baris_aman(pilihan, sizeof(pilihan)) <= 0) {
        ed->mode = MODE_PERINTAH;
        return -1;
    }

    // opsi 1: Menimpa file
    if (strcmp(pilihan, "1") == 0) {
        ed->mode = MODE_PERINTAH;
        // Langsung panggil save_as menggunakan path yang sudah tersimpan di struct
        return save_as(ed, ed->filepath); 
    }

    // opsi 2: Menyimpan sebagai file baru (menggunakan navigator)
    if (strcmp(pilihan, "2") == 0) {
        if (navigasi_path_custom(folder_tujuan)) {
            printf("Masukkan nama file baru: ");
            fflush(stdout);
            if (baca_baris_aman(nama_file, sizeof(nama_file)) > 0) {
                snprintf(path_baru, sizeof(path_baru), "%s/%s", folder_tujuan, nama_file);
                ed->mode = MODE_PERINTAH;
                return save_as(ed, path_baru);
            }
        }
        ed->mode = MODE_PERINTAH;
        return -1;
    }

    printf("Pilihan tidak valid.\n");
    ed->mode = MODE_PERINTAH;
    return -1;
}

/*    save_as()   */

int save_as(TextEditor *ed, const char *path) {
    FILE *fp;
    int   i;
    char *ptr;
    char  konfirmasi[8];

    if (path == NULL || strlen(path) == 0) {
        printf("Path tidak boleh kosong.\n");
        return -1;
    }

    if (file_ada(path) && strcmp(path, ed->filepath) != 0) {
        printf("File \"%s\" sudah ada. Timpa? (ya/tidak): ", path);
        fflush(stdout);
        if (baca_baris_aman(konfirmasi, sizeof(konfirmasi)) <= 0
            || strcmp(konfirmasi, "ya") != 0) {
            printf("Dibatalkan.\n");
            return -1;
        }
    }

    fp = fopen(path, "w");
    if (fp == NULL) {
        printf("Error: tidak bisa menulis ke \"%s\".\n", path);
        return -1;
    }

    for (i = 0; i < ed->jumlah_baris; i++) {
        /* FIX: hanya tulis \n jika buffer[i] valid */
        if (ed->buffer[i] != NULL) {
            fputs(ed->buffer[i], fp);
            fputc('\n', fp);
        }
    }

    fclose(fp);

    strncpy(ed->filepath, path, MAX_FILEPATH - 1);
    ed->filepath[MAX_FILEPATH - 1] = '\0';

    ptr = ambil_nama_file(path);
    strncpy(ed->filename, ptr ? ptr : path, MAX_FILENAME - 1);
    ed->filename[MAX_FILENAME - 1] = '\0';

    ed->is_modified = 0;
    printf("Disimpan ke \"%s\" (%d baris).\n", ed->filename, ed->jumlah_baris);
    return 0;
}

/*   delete_file   */

int delete_file(TextEditor *ed) {
    char konfirmasi[8];

    if (strlen(ed->filepath) == 0) {
        printf("Tidak ada file yang terbuka.\n");
        return -1;
    }

    ed->mode = MODE_INPUT;
    render_layar(ed);
    printf("Hapus \"%s\" dari disk? (ketik 'ya' untuk konfirmasi): ",
           ed->filename);
    fflush(stdout);

    if (baca_baris_aman(konfirmasi, sizeof(konfirmasi)) <= 0
        || strcmp(konfirmasi, "ya") != 0) {
        printf("Dibatalkan.\n");
        ed->mode = MODE_PERINTAH;
        return -1;
    }
    ed->mode = MODE_PERINTAH;

    if (remove(ed->filepath) != 0) {
        printf("Error: gagal menghapus \"%s\".\n", ed->filepath);
        return -1;
    }

    printf("File \"%s\" berhasil dihapus.\n", ed->filename);
    bebaskan_buffer(ed);
    ed->filepath[0] = '\0';
    ed->filename[0] = '\0';
    return 0;
}

void tampilkan_isi_folder(const char *path) {
    char perintah[MAX_FILEPATH + 20];
    printf("\n--- Isi Direktori saat ini ---\n");
    
    #ifdef _WIN32
        snprintf(perintah, sizeof(perintah), "dir /b \"%s\"", path);
    #else
        snprintf(perintah, sizeof(perintah), "ls \"%s\"", path);
    #endif
    
    system(perintah);
    printf("------------------------------------------\n");
}

void potong_ke_parent(char *path) {
    int i;
    int len = (int)strlen(path);

    // 1. Buang slash di paling akhir jika ada
    if (len > 0 && (path[len-1] == '/' || path[len-1] == '\\')) {
        path[len-1] = '\0';
        len--;
    }

    // 2. Cari slash terakhir dari belakang ke depan
    for (i = len - 1; i >= 0; i--) {
        if (path[i] == '/' || path[i] == '\\') {
            // Jika ini root (misal "C:\"), jangan hapus slash-nya
            if (i == 0 || (i == 2 && path[1] == ':')) {
                 path[i+1] = '\0';
            } else {
                 path[i] = '\0';
            }
            return;
        }
    }
}

#ifdef _WIN32
  #include <direct.h> // Untuk _getcwd
#else
  #include <unistd.h> // Untuk getcwd
#endif

int navigasi_path_custom(char *hasil_path) {
    char path_aktif[MAX_FILEPATH];
    char input[MAX_FILENAME];
    
    // AMBIL PATH ABSOLUT SAAT INI
    #ifdef _WIN32
        if (_getcwd(path_aktif, sizeof(path_aktif)) == NULL) strcpy(path_aktif, ".");
    #else
        if (getcwd(path_aktif, sizeof(path_aktif)) == NULL) strcpy(path_aktif, ".");
    #endif

    while (1) {
        clear_screen();
        printf("=== NAVIGATOR DIREKTORI ===\n");
        printf("Lokasi: %s\n", path_aktif);
        
        tampilkan_isi_folder(path_aktif);

        printf("\nNavigasi:\n");
        printf(" [..] Naik ke Parent\n");
        printf(" [nama_folder] Masuk ke folder tersebut\n");
        printf(" [y] PILIH LOKASI INI\n");
        printf(" [g] Batal\n");
        printf("\nInput Perintah/Folder: ");
        fflush(stdout);
        
        if (baca_baris_aman(input, sizeof(input)) <= 0) continue;

        if (strcmp(input, "y") == 0) {
            strcpy(hasil_path, path_aktif);
            return 1;
        } 
        else if (strcmp(input, "g") == 0) {
            return 0;
        } 
        else if (strcmp(input, "..") == 0) {
            potong_ke_parent(path_aktif);
        } 
        else {
            // Gabungkan folder
            #ifdef _WIN32
                strcat(path_aktif, "\\");
            #else
                strcat(path_aktif, "/");
            #endif
            strcat(path_aktif, input);
        }
    }
}