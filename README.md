# ⚡ KSharp Package Manager (kpm)

[![Build Status](https://github.com/Luvion1/kpm/actions/workflows/build.yml/badge.svg)](https://github.com/Luvion1/kpm/actions)
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
[![Release](https://img.shields.io/github/v/release/Luvion1/kpm)](https://github.com/Luvion1/kpm/releases)
[![Platform](https://img.shields.io/badge/platform-linux-lightgrey)](https://github.com/Luvion1/kpm)

**kpm** is a professional-grade, decentralized package manager for the **KSharp (K#)** ecosystem. It adopts the "URL-as-Identity" philosophy, enabling developers to share and consume code directly from Git repositories without a central registry bottleneck.

---

## ✨ Features

- **Decentralized**: No central server. Packages are fetched directly from Git providers (GitHub, GitLab, etc.).
- **Go-Style Workflow**: Familiar commands like `get` and `tidy` for a seamless developer experience.
- **Header-Only Metadata**: Supports embedding metadata directly in `.k` files via `// @key value` comments.
- **Modern CLI**: Rich visual experience with progress bars, spinners, and colored status messages.
- **Recursive Resolution**: Automatically detects and installs dependencies from a package's `kmod.k`.
- **Global Cache**: Manages a domain-based local cache in `~/.ksharp/modules`.

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
Copy the completion file to your `$fpath`:
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
**Structure created:**
- `kmod.k`: Project configuration.
- `src/main.k`: Entry point.
- `test/`: Directory for tests.

### Managing Dependencies
Download and register a package:
```bash
kpm get github.com/user/repo
```
Install a specific version:
```bash
kpm get github.com/user/repo v1.0.0
```

### Maintenance
List all installed modules in your system:
```bash
kpm list
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
This script checks for `kpm` installation, `git` availability, and the state of your module cache.

---

## 🧠 Metadata Management

`kpm` supports two ways of defining package information:

### 1. `kmod.k` (Standard)
The main manifest for your project.
```ksharp
module github.com/Luvion1/my-lib
ksharp 0.0.1

require (
    github.com/ksharp-lang/stdlib v0.5.0
)
```

### 2. Header-Only Meta (Lightweight)
Perfect for single-file libraries. Just add comments at the top of your `.k` file:
```ksharp
// @name my-util
// @version 1.0.0
// @description A simple utility
// @author Luvion1

pub fn say_hello() { ... }
```

---

## ⚙️ Configuration Details

| Command | Aliases | Description |
| :--- | :--- | :--- |
| `init` | - | Create a new K# module structure |
| `get` | `install` | Fetch a package from a URL |
| `list` | `ls` | Show all installed packages |
| `remove` | `rm`, `uninstall` | Remove a package from cache |
| `tidy` | `resolve` | Auto-sync `kmod.k` with source code |
| `help` | - | Show command reference |

---

## 🤝 Contributing

We welcome contributions! 
1. **Fork** the repo.
2. **Create** a feature branch.
3. **Build** and verify (`make clean && make`).
4. **Open** a Pull Request.

Please check [CONTRIBUTING.md](CONTRIBUTING.md) for more details.

## 📄 License

kpm is released under the **MIT License**. See [LICENSE](LICENSE) for details.