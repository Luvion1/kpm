#!/bin/bash
# kpm-doctor: Diagnostic tool for KPM

echo "⚡ KPM Doctor - Diagnosis"
echo "------------------------"

# Check kpm
if command -v kpm &> /dev/null; then
    echo "[OK] kpm is installed at $(which kpm)"
    kpm help | grep "Version"
else
    echo "[FAIL] kpm not found in PATH"
fi

# Check Git
if command -v git &> /dev/null; then
    echo "[OK] git is installed ($(git --version))"
else
    echo "[FAIL] git is missing (required for 'get')"
fi

# Check Cache
CACHE_DIR="$HOME/.ksharp/modules"
if [ -d "$CACHE_DIR" ]; then
    echo "[OK] Cache directory exists: $CACHE_DIR"
    echo "     Count: $(ls -R $CACHE_DIR | grep kmod.k | wc -l) modules cached."
else
    echo "[WARN] Cache directory does not exist yet."
fi

echo "------------------------"
echo "Diagnosis complete."
