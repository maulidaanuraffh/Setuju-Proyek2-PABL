/* ============================================================
 * display.h  
 * Proyek 2 |  Ikhwan Syahid Azizy (013)
 * ============================================================ */

#ifndef DISPLAY_H
#define DISPLAY_H
#ifdef _WIN32
  #include <conio.h>
  #include <windows.h>
#endif
#include "editor.h"

#define TAMPIL_BARIS 22   /* jumlah baris konten yang ditampilkan */
#define MODE_PERINTAH      0
#define MODE_INPUT     1
#define KEY_ENTER     13
#define KEY_ESC       27
#define KEY_BACKSPACE  8
#define KEY_SPECIAL   224   /* prefix tombol panah / Del di Windows */
#define KEY_LEFT       75
#define KEY_RIGHT      77
#define KEY_DELETE     83
#define KEY_HOME       71
#define KEY_END        79

void  render_layar(const TextEditor *ed);
void  render_status_bar(const TextEditor *ed);
void  render_hasil_cari(const TextEditor *ed);
void  tampilkan_menu(const TextEditor *ed);
int   baca_baris_aman(char *buf, int max);
int   edit_baris_inline(char *buf, int max, const char *isi_lama);
void  flush_stdin(void);
void  clear_screen(void);

#endif /* DISPLAY_H */
