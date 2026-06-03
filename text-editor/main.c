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
        printf("Ada perubahan belum disimpan.\n");
        printf("  [ya]    = simpan lalu keluar\n");
        printf("  [tidak] = keluar TANPA menyimpan\n");
        printf("  [batal] = kembali ke editor\n");
        printf("Pilihan: ");
        fflush(stdout);

        if (baca_baris_aman(pilihan, sizeof(pilihan)) <= 0) {
            // Gagal baca input, aman: kembali ke editor
            ed->mode = MODE_PERINTAH;
            return;
        }

        if (strcmp(pilihan, "ya") == 0) {
            save_file(ed);
            // Lanjut ke exit di bawah
        } else if (strcmp(pilihan, "tidak") == 0) {
            // Buang perubahan, langsung keluar
        } else {
            // "batal" atau input apapun selain ya/tidak
            printf("Dibatalkan, kembali ke editor.\n");
            ed->mode = MODE_PERINTAH;
            return;
        }
    }

    bebaskan_semua_baris(ed);
    printf("\nPROGRAM SELESAI!\n");
    exit(0);
}

static void cmd_tulis_baris(TextEditor *ed) {
    char input[MAX_INPUT];
    int posisi;
    int n = 0;

    ed->mode = MODE_INPUT;
    render_layar(ed);

    if (ed->jumlah_baris == 0) {
        posisi = 0;
    } else {
        posisi = ed->kursor_baris + 1;
    } 

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

        if (insert_baris(ed, posisi, input) == 0){
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

    printf("Hapus baris %d: \"%s\" ? (ya/tidak): ",ed->kursor_baris + 1, ed->kursor_ptr->isi);
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
    char *input;
    int len_lama, ret;
    char *hasil_baru;

    if (ed->jumlah_baris == 0) {
        printf("Dokumen kosong.\n");
        return;
    }
    if (ed->kursor_ptr == NULL) return;

    ed->mode = MODE_INPUT;
    render_layar(ed);
    printf("Edit baris %d: ", ed->kursor_baris + 1);
    fflush(stdout);

    len_lama = (int)strlen(ed->kursor_ptr->isi);
    input = (char *)malloc(len_lama + MAX_INPUT);
	if (input == NULL){
        printf("Error: gagal alokasi buffer edit.\n");
        ed->mode = MODE_PERINTAH;
        return;
    }

    strncpy(input, ed->kursor_ptr->isi, len_lama + MAX_INPUT-1);
    input[len_lama + MAX_INPUT - 1] = '\0';
	
    ret = edit_baris_inline(input, len_lama + MAX_INPUT, input);
 
    if (ret == 0) {
        printf("Dibatalkan.\n");
        free(input);
        ed->mode = MODE_PERINTAH;
        return;
    }
 
    // ganti isi baris dengan hasil edit 
    hasil_baru = (char *)malloc(strlen(input) + 1);
    if (hasil_baru != NULL) {
        strcpy(hasil_baru, input);
        free(ed->kursor_ptr->isi);
        ed->kursor_ptr->isi = hasil_baru;
    }

    free(input);
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

 static void cmd_hitung_kata(TextEditor *ed) {
    word_count(ed);
    printf("Tekan Enter...");
    fflush(stdout);
    flush_stdin();
}

static void cmd_simpan(TextEditor *ed) {
    char pilihan[8];
    char folder_tujuan[MAX_FILEPATH];
    char nama_file[MAX_FILENAME];
    char path_final[MAX_FILEPATH];

    // Jika file SUDAH punya nama/path
    if (strlen(ed->filepath) > 0) {
        ed->mode = MODE_INPUT;
        render_layar(ed);
        printf("File sudah memiliki nama: %s\n", ed->filename);
        printf("  [1] Simpan (Overwrite)\n");
        printf("  [2] Simpan Sebagai... (Save As)\n");
        printf("  [b] Batal\n");
        printf("Pilihan: ");
        fflush(stdout);

        if (baca_baris_aman(pilihan, sizeof(pilihan)) <= 0) {
            ed->mode = MODE_PERINTAH;
            return;
        }

        if (strcmp(pilihan, "1") == 0) {
            ed->mode = MODE_PERINTAH;
            save_file(ed); // Langsung simpan ke path yang ada
            return;
        } 
        else if (strcmp(pilihan, "2") == 0) {
            // Lanjut ke proses Save As di bawah
            printf("\n--- Proses Save As ---\n");
        } 
        else {
            printf("Dibatalkan.\n");
            ed->mode = MODE_PERINTAH;
            return;
        }
    }

    // Bagian ini dijalankan jika file BARU (pilihan 2 tadi juga akan lari ke sini)
    printf("\nPilih lokasi penyimpanan:\n");
    if (navigasi_path_custom(folder_tujuan)) {
        ed->mode = MODE_INPUT;
        render_layar(ed);
        printf("\nLokasi terpilih: %s\n", folder_tujuan);
        printf("Masukkan nama file baru (misal: tugas.txt): ");
        fflush(stdout);

        if (baca_baris_aman(nama_file, sizeof(nama_file)) > 0) {
            // Gabungkan folder + nama file
            #ifdef _WIN32
                snprintf(path_final, sizeof(path_final), "%s\\%s", folder_tujuan, nama_file);
            #else
                snprintf(path_final, sizeof(path_final), "%s/%s", folder_tujuan, nama_file);
            #endif
            save_as(ed, path_final);
        }
        ed->mode = MODE_PERINTAH;
    } else {
    	ed->mode = MODE_PERINTAH;
	}
}

static void cmd_buka_file(TextEditor *ed) {
    char folder_tujuan[MAX_FILEPATH];
    char nama_file[MAX_FILENAME];
    char path_final[MAX_FILEPATH];

    if (navigasi_path_custom(folder_tujuan)) {
        ed->mode = MODE_INPUT;
        render_layar(ed);
        printf("\nLokasi terpilih: %s\n", folder_tujuan);
        printf("Masukkan nama file yang ingin dibuka: ");
        fflush(stdout);

        if (baca_baris_aman(nama_file, sizeof(nama_file)) > 0) {
            snprintf(path_final, sizeof(path_final), "%s/%s", folder_tujuan, nama_file);
            open_file(ed, path_final);
        }
        ed->mode = MODE_PERINTAH;
    } else {
    	ed->mode = MODE_PERINTAH;
	}
}

static void cmd_file_baru(TextEditor *ed) {
    char konfirmasi[8];
    
    if (ed->is_modified) {
        ed->mode = MODE_INPUT;
        render_layar(ed);
        printf("Ada perubahan belum disimpan. Buat file baru? (ya/tidak): ");
        fflush(stdout);
        if (baca_baris_aman(konfirmasi, sizeof(konfirmasi)) <= 0 || strcmp(konfirmasi, "ya") != 0) {
            printf("Dibatalkan.\n");
            ed->mode = MODE_PERINTAH;
            return;
        }
        ed->mode = MODE_PERINTAH;
    }
    
    bebaskan_semua_baris(ed);
    ed->filepath[0] = '\0';
    ed->filename[0] = '\0';
    printf("Dokumen baru siap.\n");
}

static void cmd_hapus_file(TextEditor *ed) {
    delete_file(ed);
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
        case '6': 
            cmd_simpan(ed); 
            break;
        case '7': 
            cmd_buka_file(ed); 
            break;
        case '8': 
            cmd_file_baru(ed); 
            break;
        case '9': 
            cmd_hapus_file(ed); 
            break;
        case '0':
            cmd_hitung_kata(ed);
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