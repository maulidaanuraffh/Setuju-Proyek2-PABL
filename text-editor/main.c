#include <stdio.h>
#include "editor.h"

int main() {
    TextEditor ed;

    // test init_editor 
    init_editor(&ed); 
    printf("init OK: jumlah_baris=%d\n\n", ed.jumlah_baris);

    // test insert_baris
    printf("----TEST INSERT----\n");
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

    // test delete_baris
    printf("----TEST DELETE----\n");
    delete_baris(&ed, 0);
    printf("delete OK: jumlah_baris=%d\n", ed.jumlah_baris);
    printf("Setelah delete indeks 0:\n");
    printf("baris 0: %s\n", ed.buffer[0]); // skrg jadi "sisipan tengah"
    printf("baris 1: %s\n", ed.buffer[1]); // "struktur data" 

    // test insert_karakter
    printf("\n----TEST INSERT KARAKTER----\n");
    char input_c;
    // coba insert di baris 0 paling depan
    int target_baris = 0; 
    int target_kolom = 0; 

    printf("Ketik satu huruf untuk dimasukkan di paling awal: ");
    scanf(" %c", &input_c); 

    // panggil fungsi insert_karakter
    if (insert_karakter(&ed, target_baris, target_kolom, input_c) == 0) {
        printf("insert_karakter OK: jumlah_baris=%d\n", ed.jumlah_baris);
        printf("Hasil di baris 0 sekarang: %s\n", ed.buffer[0]);
    } else {
        printf("insert_karakter GAGAL!\n");
    }

    bebaskan_buffer(&ed);
    printf("bebaskan OK\n");
    return 0;
}