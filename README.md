 ---
  # Setuju — Text Editor & Landing Page
  

  ## Anggota Tim

  | NIM | Nama | Kontribusi Utama (after ETS) |
  |-----|------|-----------------|
  | 013 | Ikhwan Syahid Azizy | `filemanager.c`, `display.c`, `editor.c` (delete, replace, word count, go-to-line) |
  | 017 | Maulida Nur Afifah | `editor.h`, `editor.c` (linked list, insert, search, find-next), `display.c` (render) |

  ---

  ## Tentang Proyek

  Repositori ini terdiri dari dua bagian:

  1. **Text Editor** — aplikasi editor teks berbasis terminal yang ditulis dalam bahasa C. Menggunakan struktur data *doubly linked list* untuk menyimpan baris dokumen, mendukung berbagai operasi teks seperti
   tulis, edit, hapus, cari, ganti, dan manajemen file.

  2. **Landing Page** — halaman web statis yang menjadi antarmuka informasi tim proyek, di-*deploy* via GitHub Pages.

  ---

  ## Fitur Text Editor

  | Tombol | Fitur |
  |--------|-------|
  | `1` | Tulis baris baru (mode multi-baris, Enter kosong untuk selesai) |
  | `2` | Hapus baris yang sedang aktif |
  | `3` | Edit baris aktif secara *inline* (panah kiri/kanan, Home, End, Delete, Backspace) |
  | `4` | Cari teks (*keyword search*, tampilkan semua kemunculan) |
  | `5` | Cari & ganti teks (*search & replace* semua kemunculan) |
  | `6` | Simpan file (overwrite atau save as dengan navigasi direktori) |
  | `7` | Buka file (navigasi direktori interaktif) |
  | `8` | Buat dokumen baru |
  | `9` | Hapus file dari disk |
  | `0` | Hitung kata, baris, dan karakter dokumen |
  | `g` | Pergi ke nomor baris tertentu |
  | `l` | Toggle tampilan nomor baris |
  | `n` | Loncat ke kemunculan berikutnya dari hasil pencarian |
  | `q` | Keluar (dengan konfirmasi jika ada perubahan belum disimpan) |

  ---

  ## Struktur Data

  Dokumen disimpan sebagai **doubly linked list** dari node baris (`RowNode`):

  ```
  head → [baris 0] ⇄ [baris 1] ⇄ [baris 2] ⇄ ... ⇄ [baris n] ← tail
  ```

  Setiap node mengalokasikan teksnya secara dinamis (`malloc`). Traversal dilakukan dua arah dari `head` atau `tail` tergantung posisi target, sehingga worst-case pencarian node adalah O(n/2).

  ---

  ## Struktur Repositori

  ```
  /
  ├── landing-page/
  │   ├── index.html
  │   ├── script.js
  │   └── styles.css
  │
  ├── text-editor/
  │   ├── main.c          # Loop utama & command dispatcher
  │   ├── editor.c / .h   # Struktur data, operasi baris, pencarian
  │   ├── display.c / .h  # Rendering layar, input handler, status bar
  │   ├── filemanager.c / .h  # Buka/simpan/hapus file, navigasi direktori
  │   └── Makefile.win    # Build file untuk Dev-C++ / MinGW
  │
  ├── .gitignore
  └── README.md
  ```

  ---

  ## Cara Build & Menjalankan

  ### Persyaratan

  - Sistem operasi **Windows**
  - Compiler **GCC / MinGW** (termasuk dalam instalasi Dev-C++)

  ### Opsi 1 — Menggunakan Dev-C++ 5.11

  1. Buka Dev-C++
  2. **File → Open Project** → pilih `Makefile.win` di dalam folder `text-editor/`
  3. Tekan **F11** atau **Execute → Compile & Run**

  ### Opsi 2 — Menggunakan Command Prompt / PowerShell

  ```bash
  cd text-editor

  gcc -c display.c -o display.o
  gcc -c editor.c -o editor.o
  gcc -c filemanager.c -o filemanager.o
  gcc -c main.c -o main.o
  gcc display.o editor.o filemanager.o main.o -o Setuju1317.exe

  ./Setuju1317.exe
  ```

  ### Opsi 3 — Satu Perintah

  ```bash
  cd text-editor
  gcc display.c editor.c filemanager.c main.c -o Setuju1317.exe && ./Setuju1317.exe
  ```

  ---

  ## Arsitektur Modul

  ```
  main.c
    ├── editor.h / editor.c
    │     Struktur data (RowNode, TextEditor), operasi linked list,
    │     insert/delete baris, pencarian teks, word count
    │
    ├── display.h / display.c
    │     Rendering layar & scroll, status bar, menu,
    │     input karakter-per-karakter (edit inline), baca baris aman
    │
    └── filemanager.h / filemanager.c
          Buka/simpan/hapus file, navigasi direktori interaktif,
          deteksi ekstensi otomatis
  ```

  ---

  ## Catatan Teknis

  - Program **hanya berjalan di Windows** karena menggunakan `<conio.h>` (`_getch`), `<direct.h>` (`_getcwd`), dan perintah `cls` / `dir`.
  - Navigasi *inline* pada fitur edit baris menggunakan ANSI escape code untuk menggerakkan kursor terminal.
  - Hasil pencarian disimpan dalam array `HasilCari[2000]` di dalam struct `TextEditor`, mendukung hingga 2.000 kemunculan keyword per sesi pencarian.

  ---

  ## Landing Page

  Landing page tersedia di tab **GitHub Pages** repositori ini (lihat bagian *About* di halaman repo).

  ---

  *Proyek ini dibuat untuk memenuhi tugas mata kuliah Proyek 2: Pengembangan Aplikasi Berbasis Library.*