#include <stdio.h>
#include "editor.h"

int main() {
    TextEditor ed;

    // test init_editor 
    init_editor(&ed); 
    printf("init OK: jumlah_baris=%d\n", ed.jumlah_baris);

    // test insert_baris
    insert_baris(&ed, 0, "proyek 2");
    insert_baris(&ed, 1, "struktur data");
    printf("insert OK: jumlah_baris=%d\n", ed.jumlah_baris);
    printf("baris 0: %s\n", ed.buffer[0]);
    printf("baris 1: %s\n", ed.buffer[1]);

    insert_baris(&ed, 1, "sisipan tengah");
    printf("Setelah sisip di indeks 1:\n");
    printf("baris 0: %s\n", ed.buffer[0]); // bakal ttp "proyek 2"
    printf("baris 1: %s\n", ed.buffer[1]); // hrsnya jd "sisipan tengah"
    printf("baris 2: %s\n", ed.buffer[2]); // jd "struktur data" 

    bebaskan_buffer(&ed);
    printf("bebaskan OK\n");
    return 0;
}