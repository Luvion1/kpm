# Changelog

All notable changes to this project will be documented in this file.

## [0.0.2] - 2026-01-06

### Fixed
- **Smart Protocol Detection**: Fixed a bug where `kpm get` would fail if the URL already included a protocol (e.g., `https://` or `git@`).
- **SSH Support**: Added support for SSH-based Git URLs (e.g., `git@github.com:user/repo`).
- **Manifest Consistency**: Standardized the project manifest filename to `kmod.k` across all commands (`init`, `tidy`).

## [0.0.1] - 2026-01-05

### Added
- **Initial Public Release** of the KSharp Package Manager (`kpm`).
- **Decentralized Architecture**: Packages are identified by URLs (Go-style).
- **Git Integration**: Native support for fetching packages via `git clone`.
- **Global Module Cache**: Nested directory structure in `~/.ksharp/modules`.
- **Dependency Resolution**: Recursive installation based on `kmod.k` files.
- **Project Tooling**: Commands for `init`, `get`, `list`, `remove`, and `tidy`.
- **Automated CI/CD**: GitHub Actions for builds and automated binary releases.