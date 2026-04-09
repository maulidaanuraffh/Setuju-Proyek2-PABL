/* ============================================================
 * filemanager.h 
 * Proyek 2 |  Ikhwan Syahid Azizy (013)
 * ============================================================ */

#ifndef FILEMANAGER_H
#define FILEMANAGER_H


#include "editor.h"

int   open_file(TextEditor *ed, const char *path);
int   save_file(TextEditor *ed);
int   save_as(TextEditor *ed, const char *path);
int   delete_file(TextEditor *ed);
char *ambil_nama_file(const char *path);
int   file_ada(const char *path);
void tampilkan_isi_folder(const char *path);
void potong_ke_parent(char *path);
int  navigasi_path_custom(char *hasil_path);

#endif 
