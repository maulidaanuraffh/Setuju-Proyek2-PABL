#include <stdio.h>
#include <string.h>
#include "editor.h"

int main() {
    TextEditor ed;
    char input_buffer[MAX_INPUT];
    int target_baris, target_kolom;
    char input_c;

    // test init_editor 
    init_editor(&ed); 

    // test insert_baris
    printf("----TEST INSERT 3 BARIS----\n");
    for(int i = 0; i < 3; i++){
        printf("Masukkan isi baris ke-%d: ", i+1);
        if (fgets(input_buffer, sizeof(input_buffer), stdin)) {
            input_buffer[strcspn(input_buffer, "\n")] = 0;
            insert_baris(&ed, i, input_buffer);
        }
    }
    // menyisipi baris baru
    printf("Insert baris baru di? (1-%d): ", ed.jumlah_baris+1);
    fgets(input_buffer, sizeof(input_buffer), stdin);
    sscanf(input_buffer, "%d", &target_baris); // mengambil angka posisi
    target_baris -= 1;
    printf("Isi baris baru: ");
    if (fgets(input_buffer, sizeof(input_buffer), stdin)) {
        input_buffer[strcspn(input_buffer, "\n")] = 0;
        
        // panggil fungsi insert_baris dengan posisi pilihan user
        if (insert_baris(&ed, target_baris, input_buffer) == 0) { //
            printf("\nBerhasil disisipkan! Hasilnya sekarang:\n");
            for(int i = 0; i < ed.jumlah_baris; i++) {
                printf("baris ke-%d -> %s\n", i, ed.buffer[i]);
            }
        } else {
            printf("Gagal menyisipkan baris.\n");
        }
    }

    // test delete_baris
    printf("----TEST DELETE BARIS----\n");
    printf("Hapus Baris nomor berapa? (1-%d): ", ed.jumlah_baris);
    fgets(input_buffer, sizeof(input_buffer), stdin);
    sscanf(input_buffer, "%d", &target_baris);
    target_baris -= 1;

    if (delete_baris(&ed, target_baris) == 0) {
        printf("OK! Baris %d telah dihapus.\n", target_baris+1);
        printf("Jumlah baris sekarang: %d\n", ed.jumlah_baris);
    }

    // test insert_karakter
    printf("\n----TEST INSERT KARAKTER----\n");
    printf("Ketik satu huruf yang ingin disisipkan: ");
    scanf(" %c", &input_c); 
    getchar(); 

    printf("Mau disisipkan di Baris berapa? (1-%d): ", ed.jumlah_baris);
    fgets(input_buffer, sizeof(input_buffer), stdin);
    sscanf(input_buffer, "%d", &target_baris);
    target_baris -= 1;

    printf("Mau disisipkan di Kolom berapa? (1-n): ");
    fgets(input_buffer, sizeof(input_buffer), stdin);
    sscanf(input_buffer, "%d", &target_kolom);
    target_kolom -= 1;

    if (insert_karakter(&ed, target_baris, target_kolom, input_c) == 0) {
        printf("OK! Hasil baris %d: %s\n", target_baris+1, ed.buffer[target_baris]);
    }

    // test delete_karakter
    printf("Hapus karakter di Baris berapa?(1-%d): ", ed.jumlah_baris);
    fgets(input_buffer, sizeof(input_buffer), stdin);
    sscanf(input_buffer, "%d", &target_baris);
    target_baris -= 1;

    printf("Hapus di Kolom berapa? (1-n): ");
    fgets(input_buffer, sizeof(input_buffer), stdin);
    sscanf(input_buffer, "%d", &target_kolom);
//    target_kolom -= 1;

    if (delete_karakter(&ed, target_baris, target_kolom) == 0) {
        printf("OK! Hasil baris %d: %s\n", target_baris, ed.buffer[target_baris]);
    }

    // hasil akhir
    printf("\nIsi Dokumen Akhir:\n");
    for(int i = 0; i < ed.jumlah_baris; i++) {
        printf("[%d]: %s\n", i+1, ed.buffer[i]);
    }

    // test find_teks
    printf("\n----PENCARIAN TEKS----\n");
    printf("Masukkan kata yang ingin dicari: ");
    if (fgets(input_buffer, sizeof(input_buffer), stdin)) {
        input_buffer[strcspn(input_buffer, "\n")] = 0; // hapus newline
        
        int total = find_teks(&ed, input_buffer);
        
        if (total > 0) {
            printf("Kursor otomatis pindah ke: Baris %d, Kolom %d\n", 
                ed.kursor_baris+1, ed.kursor_kolom+1);
            
            // tampilkan semua lokasi jika lebih dari satu
            printf("Detail lokasi temuan:\n");
            for(int j = 0; j < total; j++) {
                printf(" - Temuan %d: Baris %d, Kolom %d\n",
                    j + 1, ed.hasil_cari[j].baris+1, ed.hasil_cari[j].kolom+1);
            }
        }
    }

    // --------------- TEST UNTUK RESET HASIL CARI --------------
    printf("----TEST DELETE BARIS----\n");
    printf("Hapus Baris nomor berapa? (1-%d): ", ed.jumlah_baris);
    fgets(input_buffer, sizeof(input_buffer), stdin);
    sscanf(input_buffer, "%d", &target_baris);
    target_baris -= 1;

    if (delete_baris(&ed, target_baris) == 0) {
        printf("OK! Baris %d telah dihapus.\n", target_baris+1);
        printf("Jumlah baris sekarang: %d\n", ed.jumlah_baris);
    }

    // test find_teks
    printf("\n----PENCARIAN TEKS----\n");
    printf("Masukkan kata yang ingin dicari: ");
    if (fgets(input_buffer, sizeof(input_buffer), stdin)) {
        input_buffer[strcspn(input_buffer, "\n")] = 0; // hapus newline
        
        int total = find_teks(&ed, input_buffer);
        
        if (total > 0) {
            printf("Kursor otomatis pindah ke: Baris %d, Kolom %d\n", 
                ed.kursor_baris+1, ed.kursor_kolom+1);
            
            // tampilkan semua lokasi jika lebih dari satu
            printf("Detail lokasi temuan:\n");
            for(int j = 0; j < total; j++) {
                printf(" - Temuan %d: Baris %d, Kolom %d\n",
                    j + 1, ed.hasil_cari[j].baris+1, ed.hasil_cari[j].kolom+1);
            }
        }
    }
    // ----------------------------
    
    // test find_next
    printf("\n----TEST FIND NEXT----\n");
    if (ed.jumlah_hasil > 1) {
        printf("Tekan [Enter] untuk ke lokasi berikutnya, atau ketik 'q' lalu [Enter] untuk berhenti.\n");
        
        char loop_input[10];
        while (fgets(loop_input, sizeof(loop_input), stdin)) {
            if (loop_input[0] == 'q') break; // berhenti jika user ketik q
            
            find_next(&ed);
            printf("Lanjut? (Enter/q): ");
        }
    }

    // test replace_teks
    char input_cari[MAX_INPUT], input_ganti[MAX_INPUT];
    
    printf("\n----TEST REPLACE TEKS----\n");
    printf("Masukkan kata yang ingin diganti: ");
    if (fgets(input_cari, sizeof(input_cari), stdin)) {
        input_cari[strcspn(input_cari, "\n")] = 0; // hps newline
        
        printf("Masukkan kata pengganti: ");
        if (fgets(input_ganti, sizeof(input_ganti), stdin)) {
            input_ganti[strcspn(input_ganti, "\n")] = 0;
            
            // memanggil fungsi replace
            replace_teks(&ed, input_cari, input_ganti);
        }
    }
    // tampilkan hasil akhir semua baris dengan sistem +1
    printf("\nIsi Dokumen Setelah Operasi:\n");
    for(int i = 0; i < ed.jumlah_baris; i++) {
        printf("[%d]: %s\n", i + 1, ed.buffer[i]);
    }

    // test word_count
    printf("\n---- Menghitung Statistik Dokumen ----\n");
    word_count(&ed);

    bebaskan_buffer(&ed);
    printf("bebaskan OK\n");
    return 0;
}