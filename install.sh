#!/bin/bash

# Install ncurses libraries across macOS and various Linux distributions.

set -e

echo "Detecting Operating System..."
OS="$(uname -s)"

if [ "$OS" = "Linux" ]; then
    if [ -f /etc/os-release ]; then
        # Load the OS information
        . /etc/os-release
        
        # ID or ID_LIKE can be used to match the distro family
        case "$ID" in
            ubuntu|debian|pop|linuxmint|kali|deepin)
                echo "Detected Debian/Ubuntu-based Linux ($ID)."
                sudo apt-get update
                sudo apt-get install -y libncurses5-dev libncursesw5-dev
                ;;
            fedora)
                echo "Detected Fedora Linux."
                sudo dnf install -y ncurses-devel
                ;;
            centos|rhel|almalinux|rocky)
                echo "Detected RHEL/CentOS-based Linux ($ID)."
                sudo yum install -y ncurses-devel
                ;;
            arch|manjaro|endeavouros)
                echo "Detected Arch-based Linux ($ID)."
                sudo pacman -Sy --noconfirm ncurses
                ;;
            opensuse*|suse)
                echo "Detected OpenSUSE/SUSE ($ID)."
                sudo zypper install -y ncurses-devel
                ;;
            *)
                # Fallback to checking ID_LIKE
                if echo "$ID_LIKE" | grep -q "debian"; then
                    echo "Detected Debian-like Linux ($ID_LIKE)."
                    sudo apt-get update
                    sudo apt-get install -y libncurses5-dev libncursesw5-dev
                elif echo "$ID_LIKE" | grep -q "arch"; then
                    echo "Detected Arch-like Linux ($ID_LIKE)."
                    sudo pacman -Sy --noconfirm ncurses
                elif echo "$ID_LIKE" | grep -q 'fedora\|rhel\|centos'; then
                     echo "Detected RHEL/Fedora-like Linux ($ID_LIKE)."
                     if command -v dnf >/dev/null 2>&1; then
                         sudo dnf install -y ncurses-devel
                     else
                         sudo yum install -y ncurses-devel
                     fi
                else
                    echo "Unsupported Linux distribution: $NAME"
                    echo "Please install the ncurses development libraries manually."
                    exit 1
                fi
                ;;
        esac
    else
        echo "Could not find /etc/os-release. Cannot detect Linux distribution."
        echo "Please install the ncurses development libraries manually."
        exit 1
    fi
elif [ "$OS" = "Darwin" ]; then
    echo "Detected macOS."
    
    # macOS actually ships with ncurses by default, but it's often old.
    # We will install via Homebrew if the user needs the latest/development libraries.
    if command -v brew >/dev/null 2>&1; then
        echo "Installing ncurses via Homebrew..."
        brew install ncurses
        
        echo ""
        echo "Note for macOS compilation:"
        echo "If you need to link against the brew-installed ncurses, you may need to add:"
        echo '  LDFLAGS="-L$(brew --prefix ncurses)/lib"'
        echo '  CPPFLAGS="-I$(brew --prefix ncurses)/include"'
        echo "to your compiler flags."
    else
        echo "Homebrew is not installed. macOS comes with an older ncurses built-in."
        echo "To get the latest version, please install Homebrew (https://brew.sh) first."
        exit 1
    fi
else
    echo "Unsupported Operating System: $OS"
    exit 1
fi

echo ""
echo "Installation process has completed successfully!"
