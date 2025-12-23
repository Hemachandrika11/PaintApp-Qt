# 🎨 PaintApp (Qt + CMake)

A desktop Paint application built using **C++**, **Qt 6**, and **CMake**.

## ✨ Features
- Freehand drawing & eraser
- Line, Rectangle, Ellipse tools
- Fill tool (bucket)
- Undo / Redo (Ctrl+Z / Ctrl+Y)
- New canvas (Ctrl+N)
- Save drawing (Ctrl+S)
- Color picker
- Adjustable brush size

## 🛠 Tech Stack
- C++17
- Qt 6 (Widgets)
- CMake
- MinGW (Windows)

## 🚀 Build Instructions (Windows)

```bash
cmake -S . -B build -G "MinGW Makefiles" -DCMAKE_PREFIX_PATH="C:/Qt/6.10.1/mingw_64"
cmake --build build
