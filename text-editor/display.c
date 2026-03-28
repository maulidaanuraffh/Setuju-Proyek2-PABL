/* ============================================================
 * display.c 
 * Proyek 2 |  Ikhwan Syahid Azizy (013)
 * ============================================================ */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "editor.h"
#include "display.h"

/*   clear_screen()   */
void clear_screen(void){
#ifdef _WIN32
    system("cls");
#else 
    system("clear");
#endif
}

/*   flush_stdin()   */
void flush_stdin(void){
    int c;
    while ((c = getchar()) != '\n' && c != EOF)
        ;
}

/*   baca_baris_aman()   */
int baca_baris_aman(char *buf, int max ){
    if (buf == NULL || max <=0 ) return -1;
    if (fgets (buf, max, stdin) == NULL){
        buf[0] = '\0';
        return -1;
    }

    buf[strcspn(buf, "\r\n")] = '\0';

    if ((int)strlen(buf) == max - 1) flush_stdin();

     return (int)strlen(buf);
}

/*   edit_baris_inline()   */
int edit_baris_inline(char *buf, int max, const char *isi_lama) {
    int len;

    if (buf == NULL || max <= 0) return -1;

    /* tampilkan isi lama di baris */
    printf("  Isi lama  : ");
    if (isi_lama && strlen(isi_lama) > 0) {
        printf("%s\n", isi_lama);
    } else {
        printf("(kosong)\n");
    }
    printf("  Isi baru  : ");
    fflush(stdout);

    if (fgets(buf, max, stdin) == NULL) {
        buf[0] = '\0';
        return -1;
    }

    buf[strcspn(buf, "\r\n")] = '\0';
    if ((int)strlen(buf) == max - 1) flush_stdin();

    len = (int)strlen(buf);

    /* Enter kosong=batalkan, pertahankan isi lama */
    if (len == 0) {
        if (isi_lama) strncpy(buf, isi_lama, max - 1);
        buf[max - 1] = '\0';
        printf("  (tidak ada perubahan)\n");
        return 0; /* 0 = tidak berubah, berbeda dari -1 = error */
    }

    return len;
}

/*   render_status_bar  ()   */
void render_status_bar(const TextEditor *ed){
    const char *nama =(strlen(ed->filename) > 0) ? ed->filename : "[Tanpa Nama]";
    const char *ubah =ed->is_modified ? "[*]" : "";
    const char *mode_str =(ed->mode == MODE_INPUT) ? " | INPUT MODE |" : "";

    printf("%-80s\n", "________________________________________________________________________________");
        printf(" %s%s  |  Baris: %d/%d  |  Kolom: %d%s\n",
           ubah, nama,
            ed->jumlah_baris > 0 ? ed->kursor_baris + 1 : 0,
            ed->jumlah_baris,
            ed->kursor_kolom,
            mode_str);

    /* tampilkan info pencarian jika ada hasil aktif */
    if (ed->jumlah_hasil > 0 && strlen(ed->keyword_terakhir) > 0) {
        printf(" Cari: \"%s\"  —  %d kemunculan  |  aktif: %d/%d "
               "(baris %d, kolom %d)\n",
                ed->keyword_terakhir,
                ed->jumlah_hasil,
                ed->index_cari + 1,
                ed->jumlah_hasil,
                ed->hasil_cari[ed->index_cari].baris + 1,
                ed->hasil_cari[ed->index_cari].kolom + 1);
    }

    printf("%-80s\n", "________________________________________________________________________________");
}

/*   render_layar()   */
void render_layar(const TextEditor *ed) {
    int mulai, akhir, i, j;
    int total_halaman, halaman_kini;

    clear_screen();
    render_status_bar(ed);

    /* hitung jendela tampilan (scroll sederhana) */
    mulai = ed->kursor_baris - (TAMPIL_BARIS / 2);
    if (mulai < 0) mulai = 0;
    akhir = mulai + TAMPIL_BARIS;
    if (akhir > ed->jumlah_baris) {
        akhir = ed->jumlah_baris;
        mulai = akhir - TAMPIL_BARIS;
        if (mulai < 0) mulai = 0;
    }

    /* indikator scroll */
    if (ed->jumlah_baris > TAMPIL_BARIS) {
        total_halaman = (ed->jumlah_baris + TAMPIL_BARIS - 1) / TAMPIL_BARIS;
        halaman_kini  = (ed->kursor_baris / TAMPIL_BARIS) + 1;
        printf(" [baris %d-%d dari %d | hal %d/%d]\n",
               mulai + 1, akhir, ed->jumlah_baris,
               halaman_kini, total_halaman);
    } else {
        printf("\n");
    }

    if (ed->jumlah_baris == 0) {
        printf("  (dokumen kosong pilih [1] untuk mulai mengetik)\n");
    } else {
        for (i = mulai; i < akhir; i++) {
            const char *penanda = (i == ed->kursor_baris) ? ">" : " ";
            const char *isi     = ed->buffer[i] ? ed->buffer[i] : "";
            int         is_hit  = 0;

            /* cek apakah baris ini mengandung hasil cari aktif */
            if (ed->jumlah_hasil > 0) {
                for (j = 0; j < ed->jumlah_hasil; j++) {
                    if (ed->hasil_cari[j].baris == i) { is_hit = 1; break; }
                }
            }

            if (ed->show_line_num) {
                printf(" %s %3d | %s%s\n",
                       penanda, i + 1, isi,
                       is_hit ? "  <<" : "");
            } else {
                printf(" %s %s%s\n",
                       penanda, isi,
                       is_hit ? "  <<" : "");
            }
        }
    }

    printf("\n");

    if (ed->mode == MODE_PERINTAH) tampilkan_menu(ed);
}

/*   tampilkan_menu()  */
void tampilkan_menu(const TextEditor *ed) {
    printf("________________________________________________________________________________\n");
    printf(" [1] Tulis baris      [2] Hapus baris aktif  [3] Edit baris aktif\n");
    printf(" [4] Cari teks        [5] Cari & ganti        [n] Kemunculan berikutnya\n");
    printf(" [6] Simpan           [7] Buka file           [8] File baru\n");
    printf(" [9] Hapus file       [0] Hitung kata         [l] Toggle nomor baris\n");
    printf(" [g] Go to line       [i] Insert karakter     [d] Delete karakter\n");

    if (ed->is_modified) {
        printf(" [q] Keluar         *** Ada perubahan belum disimpan! ***\n");
    } else {
        printf(" [q] Keluar\n");
    }

    printf("________________________________________________________________________________\n");
    printf("Pilihan: ");
    fflush(stdout);
}

/*   render_hasil_cari()   */
/* Tampilkan tabel semua posisi kemunculan keyword */
void render_hasil_cari(const TextEditor *ed) {
    int i;

    if (ed->jumlah_hasil == 0) {
        printf("Tidak ada hasil pencarian.\n");
        return;
    }

    printf("\nSemua kemunculan \"%s\" (%d total):\n",
           ed->keyword_terakhir, ed->jumlah_hasil);
    printf("  %-6s  %-6s  %s\n", "Baris", "Kolom", "Isi baris");
    printf("  %-6s  %-6s  %s\n", "------", "------",
           "----------------------------------------");

    for (i = 0; i < ed->jumlah_hasil; i++) {
        int   b   = ed->hasil_cari[i].baris;
        int   k   = ed->hasil_cari[i].kolom;
        const char *isi = (ed->buffer[b] != NULL) ? ed->buffer[b] : "";
        const char *tanda = (i == ed->index_cari) ? "<<" : "  ";

        /* tampilkan potongan baris (maks 45 karakter) */
        printf("  %-6d  %-6d  %.45s %s\n", b + 1, k + 1, isi, tanda);
    }
    printf("\n");
}
