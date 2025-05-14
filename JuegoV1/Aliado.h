#pragma once
#include "Entidad.h"
#include "Personaje.h"
#include "Constantes.h"

enum class TipoAliado {
    SANADOR,
    CONGELADOR_TIEMPO,
    CONGELADOR_ENEMIGOS
};

const std::vector<std::string> DISEÑOS_ALIADO = {
    "(+H+)", // Ssanador
    "(&T&)", // congelador de tiempo
    "(*E*)"  // congelador de enemigos
};

class Aliado : public Entidad {
private:
    TipoAliado tipo;
    bool usado;
    int duracionEfectoTicks;

public:
    Aliado(int inicioX, int inicioY, int limiteAncho, int limiteAlto, TipoAliado t)
        : Entidad(inicioX, inicioY, DISEÑOS_ALIADO[static_cast<int>(t)], limiteAncho, 
            limiteAlto, ConsoleColor::White),
        tipo(t), usado(false) {

        if (t == TipoAliado::SANADOR) representacion = DISEÑOS_ALIADO[0];
        else if (t == TipoAliado::CONGELADOR_TIEMPO) representacion = DISEÑOS_ALIADO[1];
        else if (t == TipoAliado::CONGELADOR_ENEMIGOS) representacion = DISEÑOS_ALIADO[2];
        else representacion = "ALI"; //por si daba error el asccii art

        anchoRepresentacion = representacion.length();

        if (tipo == TipoAliado::CONGELADOR_TIEMPO || tipo == TipoAliado::CONGELADOR_ENEMIGOS) {
            duracionEfectoTicks = 5 * TICKS_POR_SEGUNDO;
        }
        else {
            duracionEfectoTicks = 0;
        }
    }

    ~Aliado() override {}
    void mover() override { /* estaticos */ }
    bool fueUsado() const { return usado; }

    int activar(Personaje* jugador) {
        if (!usado) {
            usado = true;
            borrar();
            hacerSonido(1200, 150);

            if (tipo == TipoAliado::SANADOR) {
                if (jugador) jugador->incrementarVida();
            }
            return duracionEfectoTicks;
        }
        return 0;
    }
    TipoAliado getTipo() const { return tipo; }
};