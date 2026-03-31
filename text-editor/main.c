#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "editor.h"
#include "display.h"
#include "filemanager.h"

void proses_perintah(TextEditor *ed, int pilihan);
void cmd_exit(TextEditor *ed);

void cmd_exit(TextEditor *ed) {
    char pilihan[8];

    if (ed->is_modified) {
        ed->mode = MODE_INPUT;
        render_layar(ed);
        printf("Ada perubahan belum disimpan. Simpan dulu? (ya/tidak): ");
        fflush(stdout);

        if (baca_baris_aman(pilihan, sizeof(pilihan)) > 0) {
            if (strcmp(pilihan, "ya") == 0) {
                save_file(ed); 
            } else {
                ed->mode = MODE_PERINTAH; 
                return; 
            }
        }
    }

    bebaskan_buffer(ed);
    printf("\nPROGRAM SELESAI!\n");
    exit(0);
}

static void cmd_tulis_baris(TextEditor *ed) {
    char input[MAX_INPUT];
    int posisi;
    int n = 0;

    ed->mode = MODE_INPUT;
    render_layar(ed);

    posisi = (ed->jumlah_baris == 0) ? 0 : ed->kursor_baris + 1;

    printf("[Mode tulis] Ketik baris demi baris, Enter kosong untuk selesai.\n");
    printf("(Baris akan disisipkan mulai posisi %d)\n\n", posisi + 1);

     while (1) {
        printf("  [%d] ", posisi + 1);
        fflush(stdout);

        if (fgets(input, sizeof(input), stdin) == NULL) break;
        input[strcspn(input, "\r\n")] = '\0';

        if (strlen(input) == 0) {
            if (n == 0) printf("  (tidak ada baris yang ditambahkan)\n");
            else printf("  %d baris ditambahkan.\n", n);
            break;
        }

        if ((int)strlen(input) == sizeof(input) - 1) flush_stdin();

        if (insert_baris(ed, posisi, input) == 0) {
            posisi++;
            n++;
        }
    }

    ed->mode = MODE_PERINTAH;
}

static void cmd_toggle_nomor(TextEditor *ed) {
    ed->show_line_num = !ed->show_line_num;
    printf("Nomor baris: %s\n", ed->show_line_num ? "ON" : "OFF");
    fflush(stdout);
}

static void cmd_hapus_baris(TextEditor *ed) {
   if (ed->jumlah_baris == 0) {
        printf("Dokumen kosong. Klik Enter untuk lanjut\n");
        getchar();
        return;
    }

    printf("Hapus baris %d: \"%s\" ? (ya/tidak): ",
        ed->kursor_baris + 1,
        ed->buffer[ed->kursor_baris] ? 
        ed->buffer[ed->kursor_baris] : "");
    fflush(stdout);

    {
        char konfirmasi[8];
        ed->mode = MODE_INPUT;
        if (baca_baris_aman(konfirmasi, sizeof(konfirmasi)) > 0
            && strcmp(konfirmasi, "ya") == 0) {
            delete_baris(ed, ed->kursor_baris);
        } else {
            printf("Dibatalkan.\n");
        }
        ed->mode = MODE_PERINTAH; 
    }
}

 static void cmd_go_to_line_menu(TextEditor *ed) {
    char input[16];
    int nomor;

    if (ed->jumlah_baris == 0) {
        printf("Dokumen kosong.\n");
        getchar();
        return;
    }

    ed->mode = MODE_INPUT;
    render_layar(ed);
    printf("Pergi ke baris (1-%d): ", ed->jumlah_baris);
    fflush(stdout);

    if (baca_baris_aman(input, sizeof(input)) <= 0) {
        printf("Dibatalkan.\n");
        ed->mode = MODE_PERINTAH;
        return;
    }

    nomor = atoi(input);
    go_to_line(ed, nomor);
    ed->mode = MODE_PERINTAH;
}

static void cmd_edit_baris(TextEditor *ed) {
    char input[MAX_INPUT];
    char *baris_baru;
    int ret;

    if (ed->jumlah_baris == 0) {
        printf("Dokumen kosong.\n");
        return;
    }

    ed->mode = MODE_INPUT;
    render_layar(ed);
    printf("Edit baris %d: ", ed->kursor_baris + 1);
    fflush(stdout);

    strncpy(input,
            ed->buffer[ed->kursor_baris] ? 
            ed->buffer[ed->kursor_baris] : "",
            sizeof(input) - 1);
    input[sizeof(input) - 1] = '\0';

    ret = edit_baris_inline(input, sizeof(input), input);

    if (ret == 0) {
        printf("Dibatalkan.\n");
        ed->mode = MODE_PERINTAH;
        return;
    }

    baris_baru = alokasi_baris(input);
    if (baris_baru == NULL) {
        ed->mode = MODE_PERINTAH;
        return;
    }

    free(ed->buffer[ed->kursor_baris]);
    ed->buffer[ed->kursor_baris] = baris_baru;
    ed->is_modified = 1;
    ed->mode = MODE_PERINTAH;
    reset_hasil_cari(ed);
}

static void cmd_cari(TextEditor *ed) {
    char keyword[256]; // buffer untuk menyimpan kata kunci pencarian

    // masuk ke mode input agar tampilan render_layar menyesuaikan
    ed->mode = MODE_INPUT;
    render_layar(ed);
    printf("Cari teks: ");
    fflush(stdout);

    // mengambil input kata kunci dari user dengan aman
    if (baca_baris_aman(keyword, sizeof(keyword)) <= 0) {
        printf("Dibatalkan.\n");
        ed->mode = MODE_PERINTAH;
        return;
    }

    find_teks(ed, keyword);

    // cek apakah ada kecocokan yang ditemukan
    if (ed->jumlah_hasil > 0) {
        // menampilkan hasil pencarian
        render_hasil_cari(ed);
        printf("Gunakan [n] untuk ke kemunculan berikutnya.\n");
    }

    printf("\nTekan Enter untuk melanjutkan...");
    fflush(stdout);
    flush_stdin();

    ed->mode = MODE_PERINTAH; // kembali ke mode perintah
}

static void cmd_cari_ganti(TextEditor *ed) {
    char cari[256], ganti[MAX_INPUT];

    ed->mode = MODE_INPUT;
    render_layar(ed);

    printf("Cari   : ");
    fflush(stdout);
    if (baca_baris_aman(cari, sizeof(cari)) <= 0) {
        printf("Dibatalkan.\n");
        ed->mode = MODE_PERINTAH;
        return;
    }

    printf("Ganti  : ");
    fflush(stdout);
    if (baca_baris_aman(ganti, sizeof(ganti)) < 0) {
        printf("Dibatalkan.\n");
        ed->mode = MODE_PERINTAH;
        return;
    }

    replace_teks(ed, cari, ganti);
    ed->mode = MODE_PERINTAH;
}

void proses_perintah(TextEditor *ed, int pilihan) {
   switch (pilihan) {
        case '1': 
            cmd_tulis_baris(ed);
            break;
        case '2': 
            cmd_hapus_baris(ed);
            break;
        case '3': 
            cmd_edit_baris(ed);
            break;
        case '4':
            cmd_cari(ed);
            break;
        case '5':
            cmd_cari_ganti(ed);
            break;
        case 'g': 
            cmd_go_to_line_menu(ed); 
            break;
        case 'l': 
            cmd_toggle_nomor(ed); 
            break;
        case 'n':
            find_next(ed);
            break;
        case 'q':
            cmd_exit(ed);
            break;
        default:
            break;
    }
}

int main(void) {
    TextEditor ed;
    int pilihan;
 
    init_editor(&ed);
 
    printf("Text Editor — Proyek 2\n");
    printf("Gunakan nomor/huruf menu lalu tekan Enter.\n\n");
 
    while (1) {
        render_layar(&ed);
        pilihan = getchar();
        if (pilihan == EOF) { 
            cmd_exit(&ed); 
        }
        if (pilihan == '\n' || pilihan == '\r') continue;
        flush_stdin();
        proses_perintah(&ed, pilihan);
    }
 
    return 0;
}