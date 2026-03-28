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

    strncpy(ed->filepath, path, MAX_PATH - 1);
    ed->filepath[MAX_PATH - 1] = '\0';

    ptr = ambil_nama_file(path);
    strncpy(ed->filename, ptr ? ptr : path, MAX_FILENAME - 1);
    ed->filename[MAX_FILENAME - 1] = '\0';

    printf("File \"%s\" dibuka (%d baris).\n", ed->filename, n);
    return 0;
}
