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
    printf("\nSampai jumpa!\n");
    exit(0);
}

static void cmd_tulis_baris(TextEditor *ed) {
    char input[MAX_INPUT];
    char buf_pos[16];
    int posisi;

    ed->mode = MODE_INPUT;
    render_layar(ed);

    printf("Sisipkan di posisi (1-%d, Enter=akhir): ", ed->jumlah_baris + 1);
    fflush(stdout);

    if (baca_baris_aman(buf_pos, sizeof(buf_pos)) <= 0 || strlen(buf_pos) == 0) {
        posisi = ed->jumlah_baris;
    } else {
        posisi = atoi(buf_pos) - 1;
        if (posisi < 0) posisi = 0;
        if (posisi > ed->jumlah_baris) posisi = ed->jumlah_baris;
    }

    printf("Isi baris: ");
    fflush(stdout);

    if (baca_baris_aman(input, sizeof(input)) < 0) {
        printf("Dibatalkan.\n");
        ed->mode = MODE_PERINTAH;
        return;
    }

    insert_baris(ed, posisi, input);
    ed->mode = MODE_PERINTAH;
}

static void cmd_toggle_nomor(TextEditor *ed) {
    ed->show_line_num = !ed->show_line_num;
    printf("Nomor baris: %s\n", ed->show_line_num ? "ON" : "OFF");
    fflush(stdout);

    getchar();
}

void proses_perintah(TextEditor *ed, int pilihan) {
   switch (pilihan) {
        case '1': 
            cmd_tulis_baris(ed);
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