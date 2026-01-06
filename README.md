# ⚡ KSharp Package Manager (kpm)

[![Build Status](https://github.com/Luvion1/kpm/actions/workflows/build.yml/badge.svg)](https://github.com/Luvion1/kpm/actions)
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
[![Release](https://img.shields.io/github/release/Luvion1/kpm)](https://github.com/Luvion1/kpm/releases/v0.0.1)
[![Platform](https://img.shields.io/badge/platform-linux-lightgrey)](https://github.com/Luvion1/kpm)

**kpm** is the official package manager for the **K# (KSharp)** ecosystem. Designed with the "URL-as-Identity" philosophy, `kpm` enables developers to share and consume code directly from Git repositories without the friction of a centralized registry.

---

## 💎 K# Philosophy

> **"Maximum Power, Minimal Noise."**

K# is built to deliver C-level performance with clean code aesthetics and intelligent memory management. `kpm` supports this vision by providing a code distribution system that is:
- **Decentralized**: No central server. Your code belongs to you, wherever you choose to host it.
- **Transparent**: Minimalist metadata that can be embedded directly within the source code.
- **Fast**: Written in C11 for maximum performance and a low memory footprint.

---

## ✨ Key Features

- 🚀 **Go-Style Workflow**: Familiar commands like `get` and `tidy` for a seamless developer experience.
- 🌐 **Decentralized**: Packages are fetched directly from Git providers (GitHub, GitLab, etc.).
- 📦 **Header-Only Metadata**: Supports embedding metadata directly in `.k` files via `// @key value` comments.
- 🎨 **Modern CLI**: Rich visual experience with progress bars, spinners, and colored status messages.
- 🔄 **Recursive Resolution**: Automatically detects and installs dependencies from `kmod.k` files.
- 📂 **Global Cache**: Domain-based module management in `~/.ksharp/modules`.

---

## 🚀 Installation

### Automated Script (Recommended)
Install `kpm` globally with a single command:
```bash
curl -fsSL https://raw.githubusercontent.com/Luvion1/kpm/main/install.sh | bash
```

### From Source
Requires `gcc` and `make`.
```bash
git clone https://github.com/Luvion1/kpm.git
cd kpm
make
sudo make install
```
*This will also install man pages and bash completion.*

---

## 🐚 Shell Completions

`kpm` supports command auto-completion for a smoother terminal experience.

### Bash
Completions are usually installed automatically to `/usr/local/share/bash-completion/completions/kpm`. To enable it manually:
```bash
source completions/kpm.bash
```

### Zsh
Add it to your `$fpath`:
```zsh
mkdir -p ~/.zsh/completion
cp completions/kpm.zsh ~/.zsh/completion/_kpm
echo 'fpath=(~/.zsh/completion $fpath)' >> ~/.zshrc
echo 'autoload -U compinit && compinit' >> ~/.zshrc
```

---

## 🛠 Usage Guide

### Initialize a New Project
Create a standard K# project structure:
```bash
kpm init my-awesome-app
```

### Managing Dependencies
Download and register a new package:
```bash
kpm get github.com/user/repo
```

Sync project dependencies and remove unused ones:
```bash
kpm tidy
```

### Diagnostics
If you encounter issues, run the diagnostic tool:
```bash
./scripts/kpm-doctor.sh
```

---

## ⚙️ Command Reference

| Command | Aliases | Description |
| :--- | :--- | :--- |
| `init` | - | Create a new K# module structure |
| `get` | `install` | Fetch a package from a URL/Git |
| `list` | `ls` | Show all installed packages |
| `remove` | `rm`, `uninstall` | Remove a package from cache |
| `tidy` | `resolve` | Auto-sync `kmod.k` with source code |
| `help` | - | Show command reference |

---

## 🤝 Contributing

Contributions are always welcome! Please check [CONTRIBUTING.md](CONTRIBUTING.md) for guidelines.

## 📄 License

kpm is released under the **MIT License**. See [LICENSE](LICENSE) for details.
