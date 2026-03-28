/* ============================================================
 * display.h  
 * Proyek 2 |  Ikhwan Syahid Azizy (013)
 * ============================================================ */

#ifndef DISPLAY_H
#define DISPLAY_H

#include "editor.h"

#define TAMPIL_BARIS   22   /* jumlah baris konten yang ditampilkan */

void  render_layar(const TextEditor *ed);
void  render_status_bar(const TextEditor *ed);
void  render_hasil_cari(const TextEditor *ed);
void  tampilkan_menu(const TextEditor *ed);
int   baca_baris_aman(char *buf, int max);
int   edit_baris_inline(char *buf, int max, const char *isi_lama);
void  flush_stdin(void);
void  clear_screen(void);

#endif /* DISPLAY_H */
