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
