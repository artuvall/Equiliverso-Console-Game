#pragma once
#include "Entidad.h"
#include "Recurso.h"
#include "Constantes.h"
#include <conio.h>
#include <string>

struct ItemInventario {
    TipoRecurso tipo;
    int cantidad;
    ItemInventario() : tipo(TipoRecurso::EMPATIA), cantidad(0) {}
    ItemInventario(TipoRecurso t, int cant = 0) : tipo(t), cantidad(cant) {}
};

class Personaje : public Entidad {
private:
    int vidas;
    ItemInventario inventarioItems[NUM_TIPOS_RECURSOS];
    int totalRecursosEnInventario;
    char ultimaDireccion;

    void actualizarRepresentacion(char direccion) {
        switch (tolower(direccion)) {
        case 'd': representacion = ">[o.o]>"; break;
        case 'a': representacion = "<[o.o]<"; break;
        case 'w': representacion = "/[o.o]\\"; break;
        case 's': representacion = "\\[o.o]/"; break;
        default: break;
        }
        anchoRepresentacion = representacion.length();
        ultimaDireccion = direccion;
    }

    int obtenerIndiceParaTipo(TipoRecurso tipo) const {
        if (tipo == TipoRecurso::EMPATIA) return 0;
        else if (tipo == TipoRecurso::ETICA) return 1;
        else if (tipo == TipoRecurso::ADAPTABILIDAD) return 2;
        else if (tipo == TipoRecurso::NUCLEO_IA) return 3;
        else if (tipo == TipoRecurso::PAQUETE_DATOS) return 4;
        else if (tipo == TipoRecurso::CELULA_ENERGIA) return 5;
        return -1; // Tipo no reconocido
    }

public:
    Personaje(int inicioX, int inicioY, int limiteAncho, int limiteAlto, int vidasIni)
        : Entidad(inicioX, inicioY, ">[o.o]>", limiteAncho, limiteAlto, ConsoleColor::Green),
        vidas(vidasIni), totalRecursosEnInventario(0), ultimaDireccion('d') {

        inventarioItems[0] = ItemInventario(TipoRecurso::EMPATIA, 0);
        inventarioItems[1] = ItemInventario(TipoRecurso::ETICA, 0);
        inventarioItems[2] = ItemInventario(TipoRecurso::ADAPTABILIDAD, 0);
        inventarioItems[3] = ItemInventario(TipoRecurso::NUCLEO_IA, 0);
        inventarioItems[4] = ItemInventario(TipoRecurso::PAQUETE_DATOS, 0);
        inventarioItems[5] = ItemInventario(TipoRecurso::CELULA_ENERGIA, 0);
    }

    ~Personaje() override {}

    void moverConTeclado(char tecla) {
        borrar();
        int sigX = x, sigY = y;
        char nuevaDireccion = ultimaDireccion;

        switch (tolower(tecla)) {
        case 'w': sigY--; nuevaDireccion = 'w'; break;
        case 's': sigY++; nuevaDireccion = 's'; break;
        case 'a': sigX--; nuevaDireccion = 'a'; break;
        case 'd': sigX++; nuevaDireccion = 'd'; break;
        }

        if (sigX >= 1 && (sigX + anchoRepresentacion - 1) < anchoLimite - 1 && sigY >= 1 && sigY < altoLimite - 1) {
            x = sigX; y = sigY;
            if (nuevaDireccion != ultimaDireccion) actualizarRepresentacion(nuevaDireccion);
        }
    }
    void mover() override {}

    void dibujar() override {
        Console::ForegroundColor = color;
        movexy(x, y);
        std::cout << representacion;
        Console::ForegroundColor = ConsoleColor::White;
    }

    void decrementarVida() {
        if (vidas > 0) { vidas--; hacerSonido(200, 300); }
    }

    void incrementarVida() {
        if (vidas < VIDAS_INICIALES) {
            vidas++;
        }
    }

    bool puedeAgregarRecurso() const {
        return totalRecursosEnInventario < CAPACIDAD_INVENTARIO;
    }

    void agregarRecurso(TipoRecurso tipo) {
        if (puedeAgregarRecurso()) {
            int indice = obtenerIndiceParaTipo(tipo);
            if (indice != -1) {
                inventarioItems[indice].cantidad++;
                totalRecursosEnInventario++;
                hacerSonido(1000, 100);
            }
        }
    }

    int getCantidadRecurso(TipoRecurso tipo) const {
        int indice = obtenerIndiceParaTipo(tipo);
        if (indice != -1) {
            return inventarioItems[indice].cantidad;
        }
        return 0;
    }

    bool consumirRecursos(TipoRecurso tipo1, int cant1, TipoRecurso tipo2, int cant2) {
        int indice1 = obtenerIndiceParaTipo(tipo1);
        int indice2 = obtenerIndiceParaTipo(tipo2);

        if (indice1 != -1 && indice2 != -1 &&
            inventarioItems[indice1].cantidad >= cant1 &&
            inventarioItems[indice2].cantidad >= cant2) {

            inventarioItems[indice1].cantidad -= cant1;
            inventarioItems[indice2].cantidad -= cant2;
            totalRecursosEnInventario -= (cant1 + cant2);
            return true;
        }
        return false;
    }
    bool consumirRecursos(TipoRecurso tipo1, int cant1) {
        int indice1 = obtenerIndiceParaTipo(tipo1);
        if (indice1 != -1 && inventarioItems[indice1].cantidad >= cant1) {
            inventarioItems[indice1].cantidad -= cant1;
            totalRecursosEnInventario -= cant1;
            return true;
        }
        return false;
    }

    int getVidas() const { return vidas; }
    int getTotalRecursosInventario() const { return totalRecursosEnInventario; }
    bool estaVivo() const { return vidas > 0; }
};