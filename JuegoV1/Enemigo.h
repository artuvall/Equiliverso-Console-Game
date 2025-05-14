#pragma once
#include "Entidad.h"
#include "Bala.h"
#include "Constantes.h"
#include <vector>


enum class TipoEnemigo {
    ERRANTE_ALEATORIO, // eee amarillo
    REBOTADOR,         // <O> naranja
    DISPARADOR         // |+| rojo
};

class Enemigo : public Entidad {
private:
    TipoEnemigo tipo;
    int contadorDisparo;
    int intervaloDisparo;
    int direccionDisparo;

    void inicializarEspecifico() {
        switch (tipo) {
        case TipoEnemigo::ERRANTE_ALEATORIO:
            representacion = "eee"; color = ConsoleColor::Yellow;
            dx = rand() % 3 - 1; dy = rand() % 3 - 1; if (dx == 0 && dy == 0) dx = 1;
            break;
        case TipoEnemigo::REBOTADOR:
            representacion = "<O>"; color = ConsoleColor::DarkYellow;
            dx = (rand() % 2) * 2 - 1; dy = (rand() % 2) * 2 - 1;
            break;
        case TipoEnemigo::DISPARADOR:
            representacion = "|+|"; color = ConsoleColor::Red;
            contadorDisparo = rand() % (2 * TICKS_POR_SEGUNDO);
            intervaloDisparo = 2 * TICKS_POR_SEGUNDO; // dispara cada 2 segundos
            direccionDisparo = (x < anchoLimite / 2) ? 1 : -1;
            dx = dy = 0;
            break;
        }
        anchoRepresentacion = representacion.length();
    }

public:
    Enemigo(int inicioX, int inicioY, int limiteAncho, int limiteAlto, TipoEnemigo t)
        : Entidad(inicioX, inicioY, "", limiteAncho, limiteAlto), tipo(t) {
        inicializarEspecifico();
    }

    ~Enemigo() override {}

    void mover() override {
        borrar();
        int sigX, sigY;

        switch (tipo) {
        case TipoEnemigo::ERRANTE_ALEATORIO:
            if (rand() % 10 < 1) { dx = rand() % 3 - 1; dy = rand() % 3 - 1; if (dx == 0 && dy == 0) dx = (rand() % 2) * 2 - 1; }
            sigX = x + dx; sigY = y + dy;
            if (sigX <= 0 || (sigX + anchoRepresentacion - 1) >= anchoLimite - 1) { dx *= -1; sigX = x; }
            if (sigY <= 0 || sigY >= altoLimite - 1) { dy *= -1; sigY = y; }
            x = sigX; y = sigY;
            if (x <= 0) x = 1; if (x + anchoRepresentacion - 1 >= anchoLimite - 1) x = anchoLimite - anchoRepresentacion - 2; // -2 para ser mas seguro
            if (y <= 0) y = 1; if (y >= altoLimite - 1) y = altoLimite - 2;
            break;

        case TipoEnemigo::REBOTADOR:
            sigX = x + dx; sigY = y + dy;
            if (sigX <= 0 || (sigX + anchoRepresentacion - 1) >= anchoLimite - 1) dx *= -1;
            if (sigY <= 0 || sigY >= altoLimite - 1) dy *= -1;
            x += dx; y += dy;
            if (x <= 0) { x = 1; dx = abs(dx); } if (x + anchoRepresentacion - 1 >= anchoLimite - 1) { x = anchoLimite - anchoRepresentacion - 2; dx = -abs(dx); }
            if (y <= 0) { y = 1; dy = abs(dy); } if (y >= altoLimite - 1) { y = altoLimite - 2; dy = -abs(dy); }
            break;

        case TipoEnemigo::DISPARADOR: /* no se mueve */ break;
        }
    }

    void actualizarDisparador(Bala**& listaBalas, int& numBalasActual, int& capacidadBalas) {
        if (tipo == TipoEnemigo::DISPARADOR) {
            contadorDisparo--;
            if (contadorDisparo <= 0) {
                contadorDisparo = intervaloDisparo;
                int inicioXBala = (direccionDisparo == 1) ? x + anchoRepresentacion : x - 1;
                int inicioYBala = y;

                Bala* nuevaBala = new Bala(inicioXBala, inicioYBala, direccionDisparo, anchoLimite, altoLimite);
                if (numBalasActual == capacidadBalas) {
                    int nuevaCapacidad = (capacidadBalas == 0) ? 5 : capacidadBalas * 2;
                    Bala** temp = new Bala * [nuevaCapacidad];
                    for (int i = 0; i < numBalasActual; ++i) temp[i] = listaBalas[i];
                    delete[] listaBalas; listaBalas = temp; capacidadBalas = nuevaCapacidad;
                }
                listaBalas[numBalasActual] = nuevaBala; numBalasActual++;
            }
        }
    }

    void dibujar() override {
        Console::ForegroundColor = color;
        movexy(x, y); std::cout << representacion;
        Console::ForegroundColor = ConsoleColor::White;
    }
    TipoEnemigo getTipo() const { return tipo; }
};
