#pragma once
#include "Entidad.h"

class Bala : public Entidad {
public:
    Bala(int inicioX, int inicioY, int direccionX, int limiteAncho, int limiteAlto)
        : Entidad(inicioX, inicioY, "o", limiteAncho, limiteAlto, ConsoleColor::Magenta) {
        dx = direccionX; dy = 0; anchoRepresentacion = 1;
    }
    ~Bala() override {}
    void mover() override {
        borrar(); x += dx;
        // si sale de pantalla o choca con borde, se desactiva
        if (x <= 0 || x >= anchoLimite - 1) {
            desactivar();
        }
    }
    bool estaActiva() const { return x > 0 && x < anchoLimite - 1; } // mas preciso, debe estar dentro de los limites visibles
    void desactivar() { if (x > 0 && x < anchoLimite - 1) borrar(); x = -1; } // solo borrar si estaba visible
};