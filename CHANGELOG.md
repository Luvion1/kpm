# Changelog

All notable changes to this project will be documented in this file.

## [0.0.1] - 2026-01-06

### Added
- **Initial Public Release** of the KSharp Package Manager (`kpm`).
- **Decentralized Architecture**: Packages are identified by URLs (Go-style).
- **Git Integration**: Native support for fetching packages via `git clone`.
- **Global Module Cache**: Nested directory structure in `~/.ksharp/modules`.
- **Dependency Resolution**: Recursive installation based on `kmod.k` files.
- **Project Tooling**: Commands for `init`, `get`, `list`, `remove`, and `tidy`.
- **Automated CI/CD**: GitHub Actions for builds and automated binary releases.

### Fixed & Improved
- **Smart Protocol Detection**: Added support for URLs with explicit protocols (`https://`, `git@`, `ssh://`).
- **SSH Support**: Enabled fetching packages from private repositories via SSH.
- **Manifest Consistency**: Standardized the project manifest filename to `kmod.k` across all operations.