#pragma once
#include <windows.h>
#include <iostream>
using namespace System;

// mover cursor
void movexy(short x, short y) {
    Console::SetCursorPosition(x, y);
}

void hacerSonido(int frecuencia, int duracion) {
    Beep(frecuencia, duracion);
}

void limpiarArea(int x1, int y1, int x2, int y2) {
    for (int y = y1; y <= y2; ++y) {
        for (int x = x1; x <= x2; ++x) {
            movexy(x, y);
            std::cout << " ";
        }
    }
}