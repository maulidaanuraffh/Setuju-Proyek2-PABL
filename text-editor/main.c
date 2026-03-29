#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "editor.h"
#include "display.h"
#include "filemanager.h"

void proses_perintah(TextEditor *ed, int pilihan);
void cmd_exit(TextEditor *ed);
 
void proses_perintah(TextEditor *ed, int pilihan) {
   switch (pilihan) {
        case 'q':
            cmd_exit(ed);
            break;
        default:
            break;
    }
}

void cmd_exit(TextEditor *ed) {
    if (ed->is_modified) {
        char konfirmasi[8];
        printf("Ada perubahan belum disimpan. Keluar? (ya/tidak): ");
        fflush(stdout);
        if (baca_baris_aman(konfirmasi, sizeof(konfirmasi)) <= 0
            || strcmp(konfirmasi, "ya") != 0) {
            return;
        }
    }
    bebaskan_buffer(ed);
    exit(0);
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