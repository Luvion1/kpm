# ⚡ KSharp Package Manager (kpm)

[![Build Status](https://github.com/Luvion1/kpm/actions/workflows/build.yml/badge.svg)](https://github.com/Luvion1/kpm/actions)
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
[![Release](https://img.shields.io/github/v/release/Luvion1/kpm)](https://github.com/Luvion1/kpm/releases)
[![Platform](https://img.shields.io/badge/platform-linux-lightgrey)](https://github.com/Luvion1/kpm)

**kpm** adalah manajer paket resmi untuk ekosistem **K# (KSharp)**. Dirancang dengan filosofi "URL-as-Identity", `kpm` memungkinkan pengembang untuk berbagi dan menggunakan kode langsung dari repositori Git tanpa hambatan registry terpusat.

---

## 💎 Filosofi K#

> **"Maximum Power, Minimal Noise."**

K# hadir untuk memberikan performa setingkat C dengan estetika kode yang bersih dan manajemen memori yang cerdas. `kpm` mendukung visi ini dengan menyediakan sistem distribusi kode yang:
- **Desentralisasi**: Tidak ada server pusat. Kode adalah milik Anda, di mana pun Anda menyimpannya.
- **Transparan**: Metadata minimalis yang bisa disematkan langsung di dalam kode sumber.
- **Cepat**: Ditulis dalam C11 untuk performa maksimal dan jejak memori yang rendah.

---

## ✨ Fitur Utama

- 🚀 **Go-Style Workflow**: Perintah familiar seperti `get` dan `tidy` untuk pengalaman pengembangan yang mulus.
- 🌐 **Decentralized**: Paket diambil langsung dari penyedia Git (GitHub, GitLab, dll).
- 📦 **Header-Only Metadata**: Mendukung penyematan metadata langsung di file `.k` via komentar `// @key value`.
- 🎨 **Modern CLI**: Antarmuka kaya visual dengan progress bar, spinner, dan pesan status berwarna.
- 🔄 **Recursive Resolution**: Mendeteksi dan menginstal dependensi secara otomatis dari file `kmod.k`.
- 📂 **Global Cache**: Manajemen modul berbasis domain di `~/.ksharp/modules`.

---

## 🚀 Instalasi

### Skrip Otomatis (Rekomendasi)
Instal `kpm` secara global dengan satu perintah:
```bash
curl -fsSL https://raw.githubusercontent.com/Luvion1/kpm/main/install.sh | bash
```

### Dari Sumber (Source Code)
Membutuhkan `gcc` dan `make`.
```bash
git clone https://github.com/Luvion1/kpm.git
cd kpm
make
sudo make install
```
*Ini juga akan menginstal man pages dan bash completion.*

---

## 🐚 Shell Completions

`kpm` mendukung auto-completion untuk terminal agar lebih produktif.

### Bash
Biasanya terinstal otomatis di `/usr/local/share/bash-completion/completions/kpm`. Aktifkan manual dengan:
```bash
source completions/kpm.bash
```

### Zsh
Tambahkan ke `$fpath` Anda:
```zsh
mkdir -p ~/.zsh/completion
cp completions/kpm.zsh ~/.zsh/completion/_kpm
echo 'fpath=(~/.zsh/completion $fpath)' >> ~/.zshrc
echo 'autoload -U compinit && compinit' >> ~/.zshrc
```

---

## 🛠 Panduan Penggunaan

### Inisialisasi Proyek Baru
Buat struktur proyek standar K#:
```bash
kpm init my-awesome-app
```

### Mengelola Dependensi
Unduh dan daftarkan paket baru:
```bash
kpm get github.com/user/repo
```

Sinkronkan dependensi proyek dan hapus yang tidak digunakan:
```bash
kpm tidy
```

### Diagnostik
Jika mengalami kendala, jalankan alat diagnostik:
```bash
./scripts/kpm-doctor.sh
```

---

## ⚙️ Referensi Perintah

| Perintah | Alias | Deskripsi |
| :--- | :--- | :--- |
| `init` | - | Membuat struktur modul K# baru |
| `get` | `install` | Mengambil paket dari URL/Git |
| `list` | `ls` | Menampilkan semua paket terinstal |
| `remove` | `rm`, `uninstall` | Menghapus paket dari cache |
| `tidy` | `resolve` | Sinkronisasi `kmod.k` dengan kode sumber |
| `help` | - | Menampilkan bantuan perintah |

---

## 🤝 Kontribusi

Kontribusi selalu diterima! Silakan baca [CONTRIBUTING.md](CONTRIBUTING.md) untuk panduan lebih lanjut.

## 📄 Lisensi

kpm dirilis di bawah **MIT License**. Lihat [LICENSE](LICENSE) untuk detail selengkapnya.
