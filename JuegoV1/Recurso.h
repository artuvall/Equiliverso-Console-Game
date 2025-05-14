#pragma once
#include "Entidad.h"
#include "Constantes.h"
#include <string>
#include <vector>

enum class TipoRecurso {
    EMPATIA, ETICA, ADAPTABILIDAD,
    NUCLEO_IA, PAQUETE_DATOS, CELULA_ENERGIA
};

const std::vector<std::string> DISEÑOS_RECURSO = {
    "<(E)>", "{ETI}", "<AD>", "[IA]", "{DB}", "(⚡)"
};

inline std::string nombreTipoRecurso(TipoRecurso tipo) {
    switch (tipo) {
    case TipoRecurso::EMPATIA: return "Empatia";
    case TipoRecurso::ETICA: return "Etica";
    case TipoRecurso::ADAPTABILIDAD: return "Adaptab.";
    case TipoRecurso::NUCLEO_IA: return "Nucleo IA";
    case TipoRecurso::PAQUETE_DATOS: return "Paq. Datos";
    case TipoRecurso::CELULA_ENERGIA: return "Energia";
    default: return "Desconocido";
    }
}

class Recurso : public Entidad {
private:
    TipoRecurso tipo;
    bool recolectado;
    int contadorParpadeo;
    bool estadoParpadeo;

public:
    Recurso(int inicioX, int inicioY, int limiteAncho, int limiteAlto, TipoRecurso t)
        : Entidad(inicioX, inicioY, "", limiteAncho, limiteAlto),
        tipo(t), recolectado(false), contadorParpadeo(rand() % 10), estadoParpadeo(true) {

        if (t == TipoRecurso::EMPATIA) representacion = DISEÑOS_RECURSO[0];
        else if (t == TipoRecurso::ETICA) representacion = DISEÑOS_RECURSO[1];
        else if (t == TipoRecurso::ADAPTABILIDAD) representacion = DISEÑOS_RECURSO[2];
        else if (t == TipoRecurso::NUCLEO_IA) representacion = DISEÑOS_RECURSO[3];
        else if (t == TipoRecurso::PAQUETE_DATOS) representacion = DISEÑOS_RECURSO[4];
        else if (t == TipoRecurso::CELULA_ENERGIA) representacion = DISEÑOS_RECURSO[5];
        else representacion = "???";

        anchoRepresentacion = representacion.length();

        int tipoInt = 0;
        if (t == TipoRecurso::EMPATIA) tipoInt = 0;
        else if (t == TipoRecurso::ETICA) tipoInt = 1;
        else if (t == TipoRecurso::ADAPTABILIDAD) tipoInt = 2;
        else if (t == TipoRecurso::NUCLEO_IA) tipoInt = 3;
        else if (t == TipoRecurso::PAQUETE_DATOS) tipoInt = 4;
        else if (t == TipoRecurso::CELULA_ENERGIA) tipoInt = 5;

        if (tipoInt <= 2) color = ConsoleColor::Green; // color del mundo 1
        else color = ConsoleColor::Cyan;              // color del mundo 2
    }

    ~Recurso() override {}
    void mover() override {}
    void actualizarParpadeo() {
        if (!recolectado) {
            contadorParpadeo++;
            if (contadorParpadeo > 5) {
                contadorParpadeo = 0;
                estadoParpadeo = !estadoParpadeo;
            }
        }
    }
    void dibujar() override {
        if (!recolectado) {
            ConsoleColor colorActualParpadeo = color;
            if (estadoParpadeo) {
                if (color == ConsoleColor::Green) colorActualParpadeo = ConsoleColor::DarkGreen;
                else if (color == ConsoleColor::Cyan) colorActualParpadeo = ConsoleColor::DarkCyan;
            }
            Console::ForegroundColor = colorActualParpadeo;
            movexy(x, y);
            std::cout << representacion;
            Console::ForegroundColor = ConsoleColor::White;
        }
    }
    void borrar() override {
        if (!recolectado) {
            movexy(x, y);
            for (int i = 0; i < anchoRepresentacion; ++i) std::cout << " ";
        }
    }
    bool fueRecolectado() const { return recolectado; }
    void recolectar() {
        if (!recolectado) { borrar(); recolectado = true; }
    }
    TipoRecurso getTipo() const { return tipo; }
};
