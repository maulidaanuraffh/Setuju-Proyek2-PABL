#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "editor.h"
#include "display.h"
#include "filemanager.h"

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

void proses_perintah(TextEditor *ed, int pilihan) {
   switch (pilihan) {
        case '1': 
            cmd_tulis_baris(ed);
            break;
        case '2': 
            cmd_hapus_baris(ed);
            break;
        case 'g': 
            cmd_go_to_line_menu(ed); 
            break;
        case 'l': 
            cmd_toggle_nomor(ed); 
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