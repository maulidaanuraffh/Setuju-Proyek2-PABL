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
#include <windows.h>
#include <commdlg.h>

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

int dialog_buka_file(char *path_output) {
    OPENFILENAME ofn;
    char szFile[260];

    // Gunakan memset untuk membersihkan memori struktur
    memset(&ofn, 0, sizeof(ofn));
    memset(szFile, 0, sizeof(szFile));

    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = NULL;
    ofn.lpstrFile = szFile;
    ofn.nMaxFile = sizeof(szFile);
    ofn.lpstrFilter = "Text Files (*.txt)\0*.txt\0All Files (*.*)\0*.*\0";
    ofn.nFilterIndex = 1;
    ofn.lpstrFileTitle = NULL;
    ofn.nMaxFileTitle = 0;
    ofn.lpstrInitialDir = NULL;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;

    if (GetOpenFileName(&ofn) == TRUE) {
        strcpy(path_output, ofn.lpstrFile);
        return 1;
    }
    return 0;
}

/*   save_file   */

/*int save_file(TextEditor *ed) {
    char path_baru[MAX_FILEPATH];

    if (strlen(ed->filepath) == 0) {
        ed->mode = MODE_INPUT;
        render_layar(ed);
        printf("Nama file baru (misal: catatan.txt): ");
        fflush(stdout);

        if (baca_baris_aman(path_baru, sizeof(path_baru)) <= 0) {
            printf("Dibatalkan.\n");
            ed->mode = MODE_PERINTAH;
            return -1;
        }
        ed->mode = MODE_PERINTAH;
        return save_as(ed, path_baru);
    }

    return save_as(ed, ed->filepath);
}*/

/* Update fungsi save_file di filemanager.c */
int save_file(TextEditor *ed) {
    char path_baru[MAX_FILEPATH];

    // Jika file belum punya path (file baru)
    if (strlen(ed->filepath) == 0) {
        if (dialog_simpan_file(path_baru)) {
            return save_as(ed, path_baru);
        }
        return -1; // Batal
    }

    // Jika sudah punya path, langsung simpan (Save)
    return save_as(ed, ed->filepath);
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

    /* FIX: konfirmasi overwrite jika file sudah ada dan path berbeda */
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


int dialog_simpan_file(char *path_output) {
    OPENFILENAME ofn;
    char szFile[260];

    memset(&ofn, 0, sizeof(ofn));
    memset(szFile, 0, sizeof(szFile));

    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = NULL;
    ofn.lpstrFile = szFile;
    ofn.nMaxFile = sizeof(szFile);
    
    // Memberikan pilihan format .txt
    ofn.lpstrFilter = "Text Files (*.txt)\0*.txt\0All Files (*.*)\0*.*\0";
    ofn.nFilterIndex = 1;
    ofn.lpstrFileTitle = NULL;
    ofn.nMaxFileTitle = 0;
    ofn.lpstrInitialDir = NULL;
    
    // Tambahan Flag: OFN_OVERWRITEPROMPT untuk konfirmasi jika menimpa file
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT | OFN_NOCHANGEDIR;
    ofn.lpstrDefExt = "txt"; // Ekstensi otomatis jika user lupa mengetik .txt

    if (GetSaveFileName(&ofn) == TRUE) {
        strcpy(path_output, ofn.lpstrFile);
        return 1;
    }
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