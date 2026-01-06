#!/bin/bash

# KSharp Package Manager (kpm) Installer
# --------------------------------------
# Version: 0.0.1
# Features: Auto-arch detection, Binary fetching, Build-from-source fallback

set -e

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
BOLD='\033[1m'
NC='\033[0m' # No Color

info() { echo -e "${BLUE}[INFO]${NC} $1"; }
success() { echo -e "${GREEN}[SUCCESS]${NC} $1"; }
warn() { echo -e "${YELLOW}[WARN]${NC} $1"; }
error() { echo -e "${RED}[ERROR]${NC} $1"; exit 1; }

echo -e "${BLUE}${BOLD}"
echo "  ⚡ KSharp Package Manager Installer"
echo "  ----------------------------------"
echo -e "${NC}"

REPO="Luvion1/kpm"
INSTALL_DIR="/usr/local/bin"
BINARY_NAME="kpm"

# 1. Determine installation mode
if [ -f "Makefile" ] && [ -d "src" ]; then
    MODE="BUILD"
    info "Local source detected. Switching to Build Mode."
else
    MODE="DOWNLOAD"
    info "No source found. Switching to Binary Download Mode."
fi

# 2. Prerequisite Checks
info "Checking environment..."
check_cmd() { command -v $1 &> /dev/null || error "$1 is required. Please install it."; }

if [ "$MODE" == "BUILD" ]; then
    check_cmd "gcc"
    check_cmd "make"
    check_cmd "git"
else
    check_cmd "curl"
    check_cmd "grep"
fi

# 3. Execution
if [ "$MODE" == "BUILD" ]; then
    info "Building kpm from local source..."
    make clean > /dev/null 2>&1
    make
    LOCAL_BIN="./kpm"
else
    # Architecture Detection
    ARCH=$(uname -m)
    case "$ARCH" in
        x86_64)  ASSET_NAME="kpm-x86_64" ;; 
        aarch64|arm64) ASSET_NAME="kpm-aarch64" ;; 
        *) error "Unsupported architecture: $ARCH. Please build from source." ;; 
    esac

    info "Detected architecture: $ARCH"
    
    # Fetch Latest Release Version
    LATEST_TAG=$(curl -s "https://api.github.com/repos/$REPO/releases/latest" | grep '"tag_name":' | sed -E 's/.*"([^"]+)".*/\1/')
    
    if [ -z "$LATEST_TAG" ]; then
        # Fallback if no release exists yet
        error "No stable release found on GitHub. Please build from source."
    fi

    DOWNLOAD_URL="https://github.com/$REPO/releases/download/$LATEST_TAG/$ASSET_NAME"
    
    info "Downloading $ASSET_NAME ($LATEST_TAG)..."
    curl -L "$DOWNLOAD_URL" -o "$BINARY_NAME"
    chmod +x "$BINARY_NAME"
    LOCAL_BIN="./$BINARY_NAME"
fi

# 4. Installation
info "Installing to $INSTALL_DIR..."

install_binary() {
    if [ -w "$INSTALL_DIR" ]; then
        cp "$LOCAL_BIN" "$INSTALL_DIR/$BINARY_NAME"
    else
        warn "Need sudo permissions to write to $INSTALL_DIR"
        sudo cp "$LOCAL_BIN" "$INSTALL_DIR/$BINARY_NAME"
    fi
}

install_binary

# 5. Support Assets (Completions & Man)
# Only if we are in BUILD mode (files exist)
if [ "$MODE" == "BUILD" ]; then
    info "Installing manual pages and completions..."
    if [ -w "$INSTALL_DIR" ]; then
        make install > /dev/null 2>&1
    else
        sudo make install > /dev/null 2>&1
    fi
fi

# 6. Verification
if command -v kpm &> /dev/null; then
    VER=$(kpm help | grep "Version" | awk '{print $NF}')
    success "kpm $VER is now available at $(which kpm)"
    echo -e "\nTry running: ${YELLOW}kpm help${NC}"
else
    error "Installation failed. Binary not found in PATH."
fi

# Cleanup
[ "$MODE" == "DOWNLOAD" ] && rm -f "$BINARY_NAME"

echo -e "\n${GREEN}${BOLD}Installation Complete!${NC}"
