#pragma once
#include "MisFunciones.h"
#include <string>
#include <stdlib.h>
#include <time.h>

class Entidad {
protected:
    int x, y; // posicion
    int dx, dy; // direccion de movimiento
    std::string representacion; // ahora usamos string para el dibujo
    int anchoRepresentacion; // para borrar correctamente
    int anchoLimite, altoLimite; // limites del area de juego
    ConsoleColor color; // color base de la entidad

public:
    Entidad(int inicioX, int inicioY, const std::string& simbolo, int limiteAncho, 
        int limiteAlto, ConsoleColor col = ConsoleColor::White)
        : x(inicioX), y(inicioY), representacion(simbolo), anchoRepresentacion(simbolo.length()),
        anchoLimite(limiteAncho), altoLimite(limiteAlto), color(col), dx(0), dy(0) {
    }

    virtual ~Entidad() {}

    virtual void borrar() {
        movexy(x, y);
        for (int i = 0; i < anchoRepresentacion; ++i) {
            std::cout << " ";
        }
    }

    virtual void dibujar() {
        Console::ForegroundColor = color;
        movexy(x, y);
        std::cout << representacion;
        Console::ForegroundColor = ConsoleColor::White;
    }

    virtual void mover() = 0;

    int getX() const { return x; }
    int getY() const { return y; }
    void setX(int nuevoX) { x = nuevoX; }
    void setY(int nuevoY) { y = nuevoY; }
    void setPos(int nuevoX, int nuevoY) { x = nuevoX; y = nuevoY; }
    int getAnchoRepresentacion() const { return anchoRepresentacion; }
};