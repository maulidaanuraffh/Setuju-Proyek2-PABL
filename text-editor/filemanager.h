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
int dialog_buka_file(char *path_output);
int dialog_simpan_file(char *path_output);

#endif 
