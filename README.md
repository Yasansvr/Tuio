# TUIO - Terminal File Explorer

TUIO is a simple **Terminal User Interface (TUI) file explorer** written in **C** using the **ncurses** library.  
It allows users to navigate directories and preview file contents directly from the terminal.

The interface is split into two windows:

- **Left window** → Directory navigation
- **Right window** → File content preview

This project is a minimal example of building a **terminal-based file manager** with keyboard navigation.

---

![Interface Preview](sample.png)

---

## Features

- Navigate directories using arrow keys
- Open folders and move to parent directory
- Preview readable text files
- Detect binary files and prevent unreadable output
- Scroll vertically and horizontally through file content using VIM key bindings
- Simple ncurses-based UI with two panels

---
## Requirements

- Linux / Unix-like OS
- GCC compiler
- `ncurses` library

---

## Installing ncurses 

```bash
sudo apt install libncurses5-dev libncursesw5-dev
```

---

## compilation and running 

```
gcc tuio.c -o tuio -lncurses && ./tuio
```

