#!/bin/bash
# macOS Run Script for RPC File Operations Project
# Usage: ./run_macos.sh [server|client|both]

PROJECT_DIR="$(cd "$(dirname "$0")" && pwd)"
SERVER_BIN="$PROJECT_DIR/server"
CLIENT_BIN="$PROJECT_DIR/client"

check_binaries() {
    if [ ! -f "$SERVER_BIN" ] || [ ! -f "$CLIENT_BIN" ]; then
        echo "❌ Binaries not found. Building..."
        make -C "$PROJECT_DIR"
    fi
}

start_rpcbind() {
    if pgrep -x rpcbind > /dev/null 2>&1; then
        echo "✅ rpcbind is already running."
    else
        echo "🔧 Starting rpcbind (requires sudo)..."
        sudo rpcbind -w
        sleep 1
        if pgrep -x rpcbind > /dev/null 2>&1; then
            echo "✅ rpcbind started successfully."
        else
            echo "❌ Failed to start rpcbind. Try: sudo rpcbind -w"
            exit 1
        fi
    fi
}

case "${1:-both}" in
    server)
        check_binaries
        start_rpcbind
        echo ""
        echo "🚀 Starting RPC File Server..."
        echo "   (Leave this terminal open; open another for the client)"
        echo ""
        cd "$PROJECT_DIR"
        exec "$SERVER_BIN"
        ;;
    client)
        check_binaries
        echo ""
        echo "🖥️  Starting RPC File Client (connecting to localhost)..."
        echo ""
        cd "$PROJECT_DIR"
        exec "$CLIENT_BIN" localhost
        ;;
    both)
        check_binaries
        start_rpcbind
        echo ""
        echo "======================================================"
        echo "  RPC File Operations Demo"
        echo "======================================================"
        echo ""
        echo "Step 1: Start the server in a NEW terminal:"
        echo "   cd \"$PROJECT_DIR\""
        echo "   ./server"
        echo ""
        echo "Step 2: In another NEW terminal, run the client:"
        echo "   cd \"$PROJECT_DIR\""
        echo "   ./client localhost"
        echo ""
        echo "Alternatively, run automatically:"
        echo "   ./run_macos.sh server   (Terminal 1)"
        echo "   ./run_macos.sh client   (Terminal 2)"
        echo "======================================================"
        ;;
    *)
        echo "Usage: $0 [server|client|both]"
        exit 1
        ;;
esac
