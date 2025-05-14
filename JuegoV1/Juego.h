#pragma once
#include <vector>
#include <string>
#include <windows.h>
#include <conio.h>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include "Constantes.h"
#include "Entidad.h"
#include "Personaje.h"
#include "Enemigo.h"
#include "Recurso.h"
#include "Bala.h"
#include "Aliado.h"
#include "MisFunciones.h"

const std::vector<std::string> TITULO_ASCII_JUEGO = {
"  ______ ____  _    _ _____ _      _______      ________ _____   _____  ____  ",
" |  ____/ __ \\| |  | |_   _| |    |_   _\\ \\    / /  ____|  __ \\ / ____|/ __ \\ ",
" | |__ | |  | | |  | | | | | |      | |  \\ \\  / /| |__  | |__) | (___ | |  | |",
" |  __|| |  | | |  | | | | | |      | |   \\ \\/ / |  __| |  _  / \\___ \\| |  | |",
" | |___| |__| | |__| |_| |_| |____ _| |_   \\  /  | |____| | \\ \\ ____) | |__| |",
" |______\\___\\_\\\\____/|_____|______|_____|   \\/   |______|_|  \\_\\_____/ \\____/ ",
"                                                                              ",
"                                                                              "
};

struct Portal {
    int x, y;
    int mundoDestino;
    char simbolo;
    ConsoleColor color;
    int anchoRepresentacion;
private:
    std::string representacionTemporal;
public:
    Portal(int px, int py, int destino, char sym = 'P', ConsoleColor col = ConsoleColor::Magenta) :
        x(px), y(py), mundoDestino(destino), simbolo(sym), color(col) {
        representacionTemporal = sym; //para calcular el ancho
        anchoRepresentacion = 1; // los portales son de un solo caracter
    }
    void dibujar() const {
        Console::ForegroundColor = color; movexy(x, y); std::cout << simbolo; Console::ForegroundColor = ConsoleColor::White;
    }
};

enum class EstadoCuadrante { VACIO, CONSTRUIDO_TIPO_A, CONSTRUIDO_TIPO_B, CONSTRUIDO_TIPO_C };

struct RecetaConstruccion {
    TipoRecurso req1_tipo; int req1_cantidad;
    TipoRecurso req2_tipo; int req2_cantidad;
    EstadoCuadrante resultadoConstruccion;
    std::string nombreConstruccion;
};

const std::vector<RecetaConstruccion> RECETAS_MUNDO3 = {
    {TipoRecurso::EMPATIA, 1, TipoRecurso::ETICA, 1, EstadoCuadrante::CONSTRUIDO_TIPO_A, "Centro Comunal (M1+M1)"},
    {TipoRecurso::NUCLEO_IA, 1, TipoRecurso::CELULA_ENERGIA, 1, EstadoCuadrante::CONSTRUIDO_TIPO_B, "Red Energetica (M2+M2)"},
    {TipoRecurso::ADAPTABILIDAD, 1, TipoRecurso::PAQUETE_DATOS, 1, EstadoCuadrante::CONSTRUIDO_TIPO_C, "Nodo Invest. (M1+M2)"}
};

struct ZonaConstruccionM3 {
    int x, y;
    int ancho, alto;
    EstadoCuadrante estado;
    char simboloVacio;
    char simboloConstruidoBase;

    ZonaConstruccionM3(int px = 0, int py = 0, int w = 6, int h = 3, EstadoCuadrante est = EstadoCuadrante::VACIO)
        : x(px), y(py), ancho(w), alto(h), estado(est), simboloVacio('.'), simboloConstruidoBase('#') {
    }

    void dibujar() const {
        ConsoleColor colorZona = ConsoleColor::DarkGray; char simboloDibujo = simboloVacio;
        if (estado == EstadoCuadrante::VACIO) { simboloDibujo = simboloVacio; colorZona = ConsoleColor::DarkGray; }
        else if (estado == EstadoCuadrante::CONSTRUIDO_TIPO_A) { simboloDibujo = 'A'; colorZona = ConsoleColor::Green; }
        else if (estado == EstadoCuadrante::CONSTRUIDO_TIPO_B) { simboloDibujo = 'B'; colorZona = ConsoleColor::Cyan; }
        else if (estado == EstadoCuadrante::CONSTRUIDO_TIPO_C) { simboloDibujo = 'C'; colorZona = ConsoleColor::Yellow; }

        Console::ForegroundColor = colorZona;
        for (int i = 0; i < alto; ++i) {
            for (int j = 0; j < ancho; ++j) {
                if (x + j < ANCHO_JUEGO - 1 && y + i < ALTO_JUEGO - 1) {
                    movexy(x + j, y + i);
                    if (i == alto / 2 && j == ancho / 2) std::cout << simboloDibujo;
                    else if (i == 0 || i == alto - 1 || j == 0 || j == ancho - 1) std::cout << char(177); // caracter de borde
                    else std::cout << " ";
                }
            }
        }
        Console::ForegroundColor = ConsoleColor::White;
    }
    bool colisionaCon(int pX, int pY, int pAncho, int pAlto = 1) const {
        return (pX < x + ancho && pX + pAncho > x && pY < y + alto && pY + pAlto > y);
    }
};

class Juego {
private:
    int anchoAreaJuego, altoAreaJuego;
    int mundoActual;
    bool enMenuPrincipal, nivelEnCurso, juegoTerminado, victoria, salirDelPrograma;

    Personaje* jugador;
    Enemigo** enemigos; int numEnemigosActual, capacidadEnemigos;
    Recurso** recursos; int numRecursosActual, capacidadRecursos;
    Aliado** aliados; int numAliadosActual, capacidadAliados;
    Bala** balas; int numBalasActual, capacidadBalas;
    Portal** portalesActivos; int numPortalesActivos, capacidadPortales;

    int ticksRestantesJuego;
    int ticksCongelacionTiempoRestantes, ticksCongelacionEnemigosRestantes;
    std::string mensajeHUDInferior;

    ZonaConstruccionM3** zonasConstruccion;
    int numZonasConstruccion;
    int cuadrantesConstruidos;
    bool mundo3Inicializado; // para saber si el mundo 3 ya fue creado alguna vez
    int zonaSeleccionadaParaConstruir;
    bool mostrandoMenuConstruccion;
    int recetaSeleccionadaMenuConstruccion;

    void inicializarVariablesGlobales() {
        anchoAreaJuego = ANCHO_JUEGO; altoAreaJuego = ALTO_JUEGO;
        mundoActual = MUNDO_1; // por defecto
        enMenuPrincipal = true; nivelEnCurso = false; juegoTerminado = false; victoria = false; salirDelPrograma = false;
        jugador = nullptr;
        enemigos = nullptr; numEnemigosActual = 0; capacidadEnemigos = 0;
        recursos = nullptr; numRecursosActual = 0; capacidadRecursos = 0;
        aliados = nullptr; numAliadosActual = 0; capacidadAliados = 0;
        balas = nullptr; numBalasActual = 0; capacidadBalas = 0;
        portalesActivos = nullptr; numPortalesActivos = 0; capacidadPortales = 0;
        zonasConstruccion = nullptr; numZonasConstruccion = 0; // se establecera al entrar al mundo 3
        ticksRestantesJuego = TIEMPO_LIMITE_SEGUNDOS * TICKS_POR_SEGUNDO;
        ticksCongelacionTiempoRestantes = 0; ticksCongelacionEnemigosRestantes = 0;
        mensajeHUDInferior = "";
        cuadrantesConstruidos = 0; mundo3Inicializado = false;
        mostrandoMenuConstruccion = false; recetaSeleccionadaMenuConstruccion = 0;
        zonaSeleccionadaParaConstruir = -1;
        srand((unsigned int)time(NULL));
        Console::SetWindowSize(ANCHO_CONSOLA, ALTO_CONSOLA); Console::CursorVisible = false;
    }

    template<typename T>
    void agregarElemento(T**& arreglo, int& contador, int& capacidad, T* elemento) {
        if (contador == capacidad) {
            int nuevaCapacidad = (capacidad == 0) ? 5 : capacidad * 2;
            T** temp = new T * [nuevaCapacidad];
            for (int i = 0; i < contador; ++i) temp[i] = arreglo[i];
            delete[] arreglo; arreglo = temp; capacidad = nuevaCapacidad;
        }
        arreglo[contador] = elemento; contador++;
    }

    bool esPosicionOcupada(int posX, int posY, int anchoEnt, int altoEnt = 1, bool chequearJugador = true) {
        for (int i = 0; i < numPortalesActivos; ++i) {
            if (portalesActivos[i] && posX < portalesActivos[i]->x + portalesActivos[i]->anchoRepresentacion &&
                posX + anchoEnt > portalesActivos[i]->x && posY < portalesActivos[i]->y + 1 && posY + altoEnt > portalesActivos[i]->y) return true;
        }
        for (int i = 0; i < numRecursosActual; ++i) {
            if (recursos[i] && posX < recursos[i]->getX() + recursos[i]->getAnchoRepresentacion() &&
                posX + anchoEnt > recursos[i]->getX() && posY < recursos[i]->getY() + 1 && posY + altoEnt > recursos[i]->getY()) return true;
        }
        for (int i = 0; i < numAliadosActual; ++i) {
            if (aliados[i] && posX < aliados[i]->getX() + aliados[i]->getAnchoRepresentacion() &&
                posX + anchoEnt > aliados[i]->getX() && posY < aliados[i]->getY() + 1 && posY + altoEnt > aliados[i]->getY()) return true;
        }
        if (chequearJugador && jugador && nivelEnCurso && posX < jugador->getX() + jugador->getAnchoRepresentacion() &&
            posX + anchoEnt > jugador->getX() && posY < jugador->getY() + 1 && posY + altoEnt > jugador->getY()) return true;
        return false;
    }

    void generarPortalesAleatorios(int mundoOrigen, int mundoDestino1, char simbolo1, ConsoleColor color1,
        int mundoDestino2, char simbolo2, ConsoleColor color2) {
        int pX1, pY1, pX2, pY2; int anchoPortal = 1;
        do { pX1 = rand() % (anchoAreaJuego - (anchoPortal + 2)) + 1; pY1 = rand() % (altoAreaJuego - 2) + 1; } while (esPosicionOcupada(pX1, pY1, anchoPortal, 1, false));
        agregarElemento(portalesActivos, numPortalesActivos, capacidadPortales, new Portal(pX1, pY1, mundoDestino1, simbolo1, color1));
        do { pX2 = rand() % (anchoAreaJuego - (anchoPortal + 2)) + 1; pY2 = rand() % (altoAreaJuego - 2) + 1; } while ((pX2 == pX1 && pY2 == pY1) || esPosicionOcupada(pX2, pY2, anchoPortal, 1, false));
        agregarElemento(portalesActivos, numPortalesActivos, capacidadPortales, new Portal(pX2, pY2, mundoDestino2, simbolo2, color2));
    }

    void liberarEntidadesNoPersistentes() { // libera enemigos, recursos de m1/m2, aliados, balas
        if (enemigos != nullptr) { for (int i = 0; i < numEnemigosActual; ++i) if (enemigos[i]) delete enemigos[i]; delete[] enemigos; enemigos = nullptr; numEnemigosActual = 0; capacidadEnemigos = 0; }
        if (recursos != nullptr) { for (int i = 0; i < numRecursosActual; ++i) if (recursos[i]) delete recursos[i]; delete[] recursos; recursos = nullptr; numRecursosActual = 0; capacidadRecursos = 0; }
        if (aliados != nullptr) { for (int i = 0; i < numAliadosActual; ++i) if (aliados[i]) delete aliados[i]; delete[] aliados; aliados = nullptr; numAliadosActual = 0; capacidadAliados = 0; }
        if (balas != nullptr) { for (int i = 0; i < numBalasActual; ++i) if (balas[i]) delete balas[i]; delete[] balas; balas = nullptr; numBalasActual = 0; capacidadBalas = 0; }
        // los portales se limpian siempre al inicio de inicializarmundo
        ticksCongelacionTiempoRestantes = 0; ticksCongelacionEnemigosRestantes = 0;
        mensajeHUDInferior = "";
    }


    void inicializarMundo(int numeroMundo) {
        // limpiar entidades del mundo anterior (excepto zonasconstruccion y jugador que son persistentes en la partida)
        if (mundoActual != MUNDO_3_CONSTRUCCION || numeroMundo != MUNDO_3_CONSTRUCCION) { // si salimos/entramos de/a m1/m2
            liberarEntidadesNoPersistentes();
        }
        if (portalesActivos != nullptr) { for (int i = 0; i < numPortalesActivos; ++i) if (portalesActivos[i]) delete portalesActivos[i]; delete[] portalesActivos; portalesActivos = nullptr; numPortalesActivos = 0; capacidadPortales = 0; }

        mundoActual = numeroMundo;
        nivelEnCurso = true; juegoTerminado = false; enMenuPrincipal = false; mensajeHUDInferior = "";

        int posJugadorX, posJugadorY;
        if (mundoActual == MUNDO_3_CONSTRUCCION) { posJugadorX = anchoAreaJuego / 2; posJugadorY = altoAreaJuego / 2; }
        else { posJugadorX = anchoAreaJuego / 10; posJugadorY = altoAreaJuego / 2; }

        if (jugador == nullptr) jugador = new Personaje(posJugadorX, posJugadorY, anchoAreaJuego, altoAreaJuego, VIDAS_INICIALES);
        else jugador->setPos(posJugadorX, posJugadorY);

        if (mundoActual == MUNDO_1) {
            generarPortalesAleatorios(MUNDO_1, MUNDO_2, 'P', ConsoleColor::Cyan, MUNDO_3_CONSTRUCCION, 'C', ConsoleColor::Gray);
            int numErrantes = NUM_ENEMIGOS_ERRANTES_BASE; int numRebotadores = NUM_ENEMIGOS_REBOTADORES_BASE; int numDisparadores = NUM_ENEMIGOS_DISPARADORES_BASE;
            int genX, genY, anchoEnt;
            for (int i = 0; i < numErrantes; ++i) { anchoEnt = 3; do { genX = rand() % (anchoAreaJuego - (anchoEnt + 2)) + 1; genY = rand() % (altoAreaJuego - 2) + 1; } 
            while (esPosicionOcupada(genX, genY, anchoEnt)); agregarElemento(enemigos, numEnemigosActual, capacidadEnemigos, new Enemigo(genX, genY, anchoAreaJuego, altoAreaJuego, TipoEnemigo::ERRANTE_ALEATORIO)); }
            for (int i = 0; i < numRebotadores; ++i) { anchoEnt = 3; do { genX = rand() % (anchoAreaJuego - (anchoEnt + 2)) + 1; genY = rand() % (altoAreaJuego - 2) + 1; } 
            while (esPosicionOcupada(genX, genY, anchoEnt)); agregarElemento(enemigos, numEnemigosActual, capacidadEnemigos, new Enemigo(genX, genY, anchoAreaJuego, altoAreaJuego, TipoEnemigo::REBOTADOR)); }
            for (int i = 0; i < numDisparadores; ++i) { 
                int yDisp = (i + 1) * (altoAreaJuego / (numDisparadores + 1)); 
                if (yDisp <= 0) yDisp = 1; 
                if (yDisp >= altoAreaJuego - 1) yDisp = altoAreaJuego - 2; 
                int xDisp = (i % 2 == 0) ? 1 : (anchoAreaJuego - 1 - 3); 
                agregarElemento(enemigos, numEnemigosActual, capacidadEnemigos, new Enemigo(xDisp, yDisp, anchoAreaJuego, altoAreaJuego, TipoEnemigo::DISPARADOR)); }

            for (int i = 0; i < TOTAL_RECURSOS_MUNDO_NORMAL; ++i) {
                TipoRecurso tipoR; int rVal = rand() % 3; // 0, 1, o 2
                if (rVal == 0) tipoR = TipoRecurso::EMPATIA; else if (rVal == 1) tipoR = TipoRecurso::ETICA; else tipoR = TipoRecurso::ADAPTABILIDAD;
                int indiceDiseno = 0; 
                if (tipoR == TipoRecurso::EMPATIA) indiceDiseno = 0; 
                else if (tipoR == TipoRecurso::ETICA) indiceDiseno = 1; 
                else if (tipoR == TipoRecurso::ADAPTABILIDAD) indiceDiseno = 2; 
                else if (tipoR == TipoRecurso::NUCLEO_IA) indiceDiseno = 3; 
                else if (tipoR == TipoRecurso::PAQUETE_DATOS) indiceDiseno = 4; 
                else indiceDiseno = 5;
                if (indiceDiseno >= 0 && indiceDiseno < (int)DISEÑOS_RECURSO.size()) anchoEnt = DISEÑOS_RECURSO[indiceDiseno].length(); else anchoEnt = 3;
                do { genX = rand() % (anchoAreaJuego - (anchoEnt + 2)) + 1; genY = rand() % (altoAreaJuego - 2) + 1; } while (esPosicionOcupada(genX, genY, anchoEnt));
                agregarElemento(recursos, numRecursosActual, capacidadRecursos, new Recurso(genX, genY, anchoAreaJuego, altoAreaJuego, tipoR));
            }
            int numMaxAliados = (rand() % 2) + 1;
            for (int i = 0; i < numMaxAliados; ++i) {
                TipoAliado tipoA; int rValA = rand() % 3; if (rValA == 0) tipoA = TipoAliado::SANADOR; else if (rValA == 1) tipoA = TipoAliado::CONGELADOR_TIEMPO; else tipoA = TipoAliado::CONGELADOR_ENEMIGOS;
                int indiceDisenoAliado = 0; if (tipoA == TipoAliado::SANADOR) indiceDisenoAliado = 0; else if (tipoA == TipoAliado::CONGELADOR_TIEMPO) indiceDisenoAliado = 1; else indiceDisenoAliado = 2;
                if (indiceDisenoAliado >= 0 && indiceDisenoAliado < (int)DISEÑOS_ALIADO.size()) anchoEnt = DISEÑOS_ALIADO[indiceDisenoAliado].length(); else anchoEnt = 5;
                do { genX = rand() % (anchoAreaJuego - (anchoEnt + 2)) + 1; genY = rand() % (altoAreaJuego - 2) + 1; } while (esPosicionOcupada(genX, genY, anchoEnt));
                agregarElemento(aliados, numAliadosActual, capacidadAliados, new Aliado(genX, genY, anchoAreaJuego, altoAreaJuego, tipoA));
            }
        }
        else if (mundoActual == MUNDO_2) {
            generarPortalesAleatorios(MUNDO_2, MUNDO_1, 'P', ConsoleColor::Green, MUNDO_3_CONSTRUCCION, 'C', ConsoleColor::Gray);
            int numErrantes = NUM_ENEMIGOS_ERRANTES_BASE + 1; int numRebotadores = NUM_ENEMIGOS_REBOTADORES_BASE + 1; int numDisparadores = NUM_ENEMIGOS_DISPARADORES_BASE - 1; 
            if (numDisparadores < 0) numDisparadores = 0;
            int genX, genY, anchoEnt; // similar a mundo 1 para generar enemigos y recursos
            for (int i = 0; i < numErrantes; ++i) { 
                anchoEnt = 3; 
                do { genX = rand() % (anchoAreaJuego - (anchoEnt + 2)) + 1; 
                genY = rand() % (altoAreaJuego - 2) + 1; } while (esPosicionOcupada(genX, genY, anchoEnt)); 
                agregarElemento(enemigos, numEnemigosActual, capacidadEnemigos, new Enemigo(genX, genY, anchoAreaJuego, altoAreaJuego, TipoEnemigo::ERRANTE_ALEATORIO)); 
            }
            for (int i = 0; i < numRebotadores; ++i) { 
                anchoEnt = 3; 
                do { genX = rand() % (anchoAreaJuego - (anchoEnt + 2)) + 1; 
                genY = rand() % (altoAreaJuego - 2) + 1; } while (esPosicionOcupada(genX, genY, anchoEnt)); 
                agregarElemento(enemigos, numEnemigosActual, capacidadEnemigos, new Enemigo(genX, genY, anchoAreaJuego, altoAreaJuego, TipoEnemigo::REBOTADOR)); 
            }
            for (int i = 0; i < numDisparadores; ++i) { 
                int yDisp = (i + 1) * (altoAreaJuego / (numDisparadores + 1)); 
                if (yDisp <= 0) yDisp = 1; if (yDisp >= altoAreaJuego - 1) yDisp = altoAreaJuego - 2; 
                int xDisp = (i % 2 == 0) ? 1 : (anchoAreaJuego - 1 - 3); 
                agregarElemento(enemigos, numEnemigosActual, capacidadEnemigos, new Enemigo(xDisp, yDisp, anchoAreaJuego, altoAreaJuego, TipoEnemigo::DISPARADOR)); }

            for (int i = 0; i < TOTAL_RECURSOS_MUNDO_NORMAL; ++i) {
                TipoRecurso tipoR; int rVal = rand() % 3; // 0, 1, o 2
                if (rVal == 0) tipoR = TipoRecurso::NUCLEO_IA; else if (rVal == 1) tipoR = TipoRecurso::PAQUETE_DATOS; else tipoR = TipoRecurso::CELULA_ENERGIA;
                int indiceDiseno = 0; if (tipoR == TipoRecurso::EMPATIA) indiceDiseno = 0; else if (tipoR == TipoRecurso::ETICA) indiceDiseno = 1; 
                else if (tipoR == TipoRecurso::ADAPTABILIDAD) indiceDiseno = 2; else if (tipoR == TipoRecurso::NUCLEO_IA) indiceDiseno = 3; 
                else if (tipoR == TipoRecurso::PAQUETE_DATOS) indiceDiseno = 4; else indiceDiseno = 5;
                if (indiceDiseno >= 0 && indiceDiseno < (int)DISEÑOS_RECURSO.size()) anchoEnt = DISEÑOS_RECURSO[indiceDiseno].length(); else anchoEnt = 3;
                do { genX = rand() % (anchoAreaJuego - (anchoEnt + 2)) + 1; genY = rand() % (altoAreaJuego - 2) + 1; } while (esPosicionOcupada(genX, genY, anchoEnt));
                agregarElemento(recursos, numRecursosActual, capacidadRecursos, new Recurso(genX, genY, anchoAreaJuego, altoAreaJuego, tipoR));
            }
            int numMaxAliados = (rand() % 2) + 1; // igual que mundo 1 para aliados
            for (int i = 0; i < numMaxAliados; ++i) {
                TipoAliado tipoA; int rValA = rand() % 3; if (rValA == 0) tipoA = TipoAliado::SANADOR; 
                else if (rValA == 1) tipoA = TipoAliado::CONGELADOR_TIEMPO; else tipoA = TipoAliado::CONGELADOR_ENEMIGOS;
                int indiceDisenoAliado = 0; if (tipoA == TipoAliado::SANADOR) indiceDisenoAliado = 0; else if (tipoA == TipoAliado::CONGELADOR_TIEMPO) indiceDisenoAliado = 1; 
                else indiceDisenoAliado = 2;
                if (indiceDisenoAliado >= 0 && indiceDisenoAliado < (int)DISEÑOS_ALIADO.size()) anchoEnt = DISEÑOS_ALIADO[indiceDisenoAliado].length(); else anchoEnt = 5;
                do { genX = rand() % (anchoAreaJuego - (anchoEnt + 2)) + 1; genY = rand() % (altoAreaJuego - 2) + 1; } while (esPosicionOcupada(genX, genY, anchoEnt));
                agregarElemento(aliados, numAliadosActual, capacidadAliados, new Aliado(genX, genY, anchoAreaJuego, altoAreaJuego, tipoA));
            }
        }
        else if (mundoActual == MUNDO_3_CONSTRUCCION) {
            generarPortalesAleatorios(MUNDO_3_CONSTRUCCION, MUNDO_1, 'P', ConsoleColor::Green, MUNDO_2, 'P', ConsoleColor::Cyan);
            if (!mundo3Inicializado) {
                numZonasConstruccion = DIM_MUNDO3_X * DIM_MUNDO3_Y;
                zonasConstruccion = new ZonaConstruccionM3 * [numZonasConstruccion];
                int celdaAncho = 6; int celdaAlto = 3; int idx = 0;
                int totalGridAncho = DIM_MUNDO3_X * celdaAncho + (DIM_MUNDO3_X - 1);
                int totalGridAlto = DIM_MUNDO3_Y * celdaAlto + (DIM_MUNDO3_Y - 1);
                int offsetX = (anchoAreaJuego - totalGridAncho) / 2; if (offsetX < 1) offsetX = 1;
                int offsetY = (altoAreaJuego - totalGridAlto) / 2; if (offsetY < 1) offsetY = 1;
                for (int i = 0; i < DIM_MUNDO3_Y; ++i) {
                    for (int j = 0; j < DIM_MUNDO3_X; ++j) {
                        zonasConstruccion[idx++] = new ZonaConstruccionM3(offsetX + j * (celdaAncho + 1), offsetY + i * (celdaAlto + 1), celdaAncho, celdaAlto);
                    }
                }
                mundo3Inicializado = true; // marcar que el mundo 3 ya se creo una vez
            }
            mostrandoMenuConstruccion = false; zonaSeleccionadaParaConstruir = -1;
        }
        Console::Clear(); dibujarBordesJuego(); dibujarBordeHUD();
    }

    void procesarEntradaJuego() {
        if (_kbhit()) {
            char tecla = _getch();
            if (tecla == 27) { enMenuPrincipal = true; nivelEnCurso = false; mensajeHUDInferior = ""; return; } // tecla esc
            if (mundoActual == MUNDO_3_CONSTRUCCION && mostrandoMenuConstruccion) procesarEntradaMenuConstruccion(tecla);
            else if (jugador && jugador->estaVivo()) {
                jugador->moverConTeclado(tecla);
                if (mundoActual == MUNDO_3_CONSTRUCCION && (tolower(tecla) == 'c')) {
                    zonaSeleccionadaParaConstruir = -1;
                    for (int i = 0; i < numZonasConstruccion; ++i) {
                        if (zonasConstruccion[i] && zonasConstruccion[i]->estado == EstadoCuadrante::VACIO &&
                            zonasConstruccion[i]->colisionaCon(jugador->getX(), jugador->getY(), jugador->getAnchoRepresentacion())) {
                            zonaSeleccionadaParaConstruir = i; mostrandoMenuConstruccion = true; recetaSeleccionadaMenuConstruccion = 0;
                            mensajeHUDInferior = "Elige que construir..."; break;
                        }
                    }
                    if (zonaSeleccionadaParaConstruir == -1 && !mostrandoMenuConstruccion) mensajeHUDInferior = "Acercate a zona vacia y pulsa C.";
                }
                else { // mundos 1 y 2, o movimiento normal en mundo 3 para portales
                    for (int i = 0; i < numPortalesActivos; ++i) {
                        if (portalesActivos[i] && jugador->getX() < portalesActivos[i]->x + portalesActivos[i]->anchoRepresentacion &&
                            jugador->getX() + jugador->getAnchoRepresentacion() > portalesActivos[i]->x &&
                            jugador->getY() == portalesActivos[i]->y) {
                            inicializarMundo(portalesActivos[i]->mundoDestino); return;
                        }
                    }
                }
                if (!mostrandoMenuConstruccion) mensajeHUDInferior = "";
            }
        }
    }

    void procesarEntradaMenuConstruccion(char tecla) {
        if (tolower(tecla) == 'w') recetaSeleccionadaMenuConstruccion = (recetaSeleccionadaMenuConstruccion - 1 + RECETAS_MUNDO3.size()) % RECETAS_MUNDO3.size();
        else if (tolower(tecla) == 's') recetaSeleccionadaMenuConstruccion = (recetaSeleccionadaMenuConstruccion + 1) % RECETAS_MUNDO3.size();
        else if (tecla == 13) { // enter
            if (zonaSeleccionadaParaConstruir != -1 && zonaSeleccionadaParaConstruir < numZonasConstruccion && zonasConstruccion[zonaSeleccionadaParaConstruir]) {
                const RecetaConstruccion& receta = RECETAS_MUNDO3[recetaSeleccionadaMenuConstruccion];
                if (jugador && jugador->consumirRecursos(receta.req1_tipo, receta.req1_cantidad, receta.req2_tipo, receta.req2_cantidad)) {
                    zonasConstruccion[zonaSeleccionadaParaConstruir]->estado = receta.resultadoConstruccion;
                    cuadrantesConstruidos++; mensajeHUDInferior = receta.nombreConstruccion + " construido!"; mostrandoMenuConstruccion = false;
                    Console::Clear(); dibujarBordesJuego(); dibujarBordeHUD(); // forzar redibujado
                    if (numZonasConstruccion > 0 && cuadrantesConstruidos * 100.0 / numZonasConstruccion >= OBJETIVO_CONSTRUCCION_PORCENTAJE) {
                        juegoTerminado = true; victoria = true; nivelEnCurso = false;
                    }
                }
                else mensajeHUDInferior = "Recursos insuficientes!";
            }
        }
        else if (tecla == 27 || tolower(tecla) == 'c') { // esc o 'c' de nuevo para cerrar
            mostrandoMenuConstruccion = false; mensajeHUDInferior = "";
            Console::Clear(); dibujarBordesJuego(); dibujarBordeHUD(); // forzar redibujado
        }
    }

    void actualizarEstadoJuego() {
        if (!jugador || !jugador->estaVivo()) { juegoTerminado = true; victoria = false; nivelEnCurso = false; return; }
        if (ticksCongelacionTiempoRestantes > 0) ticksCongelacionTiempoRestantes--;
        else if (nivelEnCurso && !mostrandoMenuConstruccion && mundoActual != MUNDO_3_CONSTRUCCION) ticksRestantesJuego--;
        if (ticksRestantesJuego <= 0 && nivelEnCurso && (mundoActual == MUNDO_1 || mundoActual == MUNDO_2)) { juegoTerminado = true; victoria = false; nivelEnCurso = false; return; }

        if (mundoActual == MUNDO_1 || mundoActual == MUNDO_2) {
            if (ticksCongelacionEnemigosRestantes > 0) ticksCongelacionEnemigosRestantes--;
            else {
                for (int i = 0; i < numEnemigosActual; ++i) if (enemigos[i]) enemigos[i]->mover();
                for (int i = 0; i < numEnemigosActual; ++i) if (enemigos[i]) enemigos[i]->actualizarDisparador(balas, numBalasActual, capacidadBalas);
            }
            for (int i = numBalasActual - 1; i >= 0; --i) {
                if (!balas[i] || !balas[i]->estaActiva()) { if (balas[i]) delete balas[i]; if (numBalasActual > 0 && i < numBalasActual - 1) balas[i] = balas[numBalasActual - 1]; if (numBalasActual > 0) numBalasActual--; continue; }
                balas[i]->mover();
                if (jugador && balas[i] && jugador->getX() < balas[i]->getX() + balas[i]->getAnchoRepresentacion() && jugador->getX() + jugador->getAnchoRepresentacion() > balas[i]->getX() && jugador->getY() == balas[i]->getY()) {
                    jugador->decrementarVida(); balas[i]->desactivar(); delete balas[i]; if (numBalasActual > 0 && i < numBalasActual - 1) balas[i] = balas[numBalasActual - 1]; if (numBalasActual > 0) numBalasActual--;
                    if (!jugador->estaVivo()) { juegoTerminado = true; victoria = false; nivelEnCurso = false; return; } continue;
                }
                if (balas[i] && !balas[i]->estaActiva()) { delete balas[i]; if (numBalasActual > 0 && i < numBalasActual - 1) balas[i] = balas[numBalasActual - 1]; if (numBalasActual > 0) numBalasActual--; }
            }
            int jX = jugador->getX(), jY = jugador->getY(), jAncho = jugador->getAnchoRepresentacion();
            for (int i = 0; i < numEnemigosActual; ++i) {
                if (!enemigos[i]) continue; int eX = enemigos[i]->getX(), eY = enemigos[i]->getY(), eAncho = enemigos[i]->getAnchoRepresentacion();
                if (jX < eX + eAncho && jX + jAncho > eX && jY == eY) {
                    jugador->decrementarVida(); enemigos[i]->borrar(); enemigos[i]->setPos(rand() % (anchoAreaJuego - eAncho - 2) + 1, rand() % (altoAreaJuego - 2) + 1);
                    if (!jugador->estaVivo()) { juegoTerminado = true; victoria = false; nivelEnCurso = false; return; } break;
                }
            }
            for (int i = 0; i < numRecursosActual; ++i) {
                if (recursos[i] && !recursos[i]->fueRecolectado()) {
                    int rX = recursos[i]->getX(), rY = recursos[i]->getY(), rAncho = recursos[i]->getAnchoRepresentacion();
                    if (jX < rX + rAncho && jX + jAncho > rX && jY == rY) {
                        if (jugador && jugador->puedeAgregarRecurso()) { jugador->agregarRecurso(recursos[i]->getTipo()); recursos[i]->recolectar(); mensajeHUDInferior = "Recurso Obtenido!"; }
                        else mensajeHUDInferior = "Inventario Lleno!";
                    }
                }
            }
            for (int i = 0; i < numRecursosActual; ++i) if (recursos[i] && !recursos[i]->fueRecolectado()) recursos[i]->actualizarParpadeo();
            for (int i = numAliadosActual - 1; i >= 0; --i) {
                if (aliados[i] && !aliados[i]->fueUsado() && jugador && jugador->getX() < aliados[i]->getX() + aliados[i]->getAnchoRepresentacion() && jugador->getX() + jugador->getAnchoRepresentacion() > aliados[i]->getX() && jugador->getY() == aliados[i]->getY()) {
                    int duracion = aliados[i]->activar(jugador); TipoAliado tipoAl = aliados[i]->getTipo();
                    if (tipoAl == TipoAliado::CONGELADOR_TIEMPO) { ticksCongelacionTiempoRestantes = duracion; mensajeHUDInferior = "Tiempo Congelado!"; }
                    else if (tipoAl == TipoAliado::CONGELADOR_ENEMIGOS) { ticksCongelacionEnemigosRestantes = duracion; mensajeHUDInferior = "Enemigos Congelados!"; }
                    else if (tipoAl == TipoAliado::SANADOR) mensajeHUDInferior = "Vida recuperada!";
                }
            }
        }
    }

    void renderizarJuego() {
        dibujarHUD();
        if (mundoActual == MUNDO_1 || mundoActual == MUNDO_2) {
            if (jugador) jugador->dibujar();
            for (int i = 0; i < numEnemigosActual; ++i) if (enemigos[i]) enemigos[i]->dibujar();
            for (int i = 0; i < numRecursosActual; ++i) if (recursos[i]) recursos[i]->dibujar();
            for (int i = 0; i < numAliadosActual; ++i) if (aliados[i] && !aliados[i]->fueUsado()) aliados[i]->dibujar();
            for (int i = 0; i < numBalasActual; ++i) if (balas[i] && balas[i]->estaActiva()) balas[i]->dibujar();
        }
        else if (mundoActual == MUNDO_3_CONSTRUCCION) {
            for (int i = 0; i < numZonasConstruccion; ++i) if (zonasConstruccion[i]) zonasConstruccion[i]->dibujar();
            if (jugador) jugador->dibujar();
            if (mostrandoMenuConstruccion) dibujarMenuConstruccion();
        }
        for (int i = 0; i < numPortalesActivos; ++i) if (portalesActivos[i]) portalesActivos[i]->dibujar();
    }

    void dibujarMenuConstruccion() {
        int menuAncho = 65; int numItemsMenu = RECETAS_MUNDO3.size(); int menuAlto = numItemsMenu + 5;
        int menuX = (anchoAreaJuego - menuAncho) / 2; if (menuX < 1) menuX = 1;
        int menuY = (altoAreaJuego - menuAlto) / 2; if (menuY < 1) menuY = 1;
        Console::BackgroundColor = ConsoleColor::DarkBlue; Console::ForegroundColor = ConsoleColor::White;
        for (int y_menu = 0; y_menu < menuAlto; ++y_menu) { movexy(menuX, menuY + y_menu); for (int x_menu = 0; x_menu < menuAncho; ++x_menu) std::cout << " "; }
        std::string tituloMenuStr;
        if (zonaSeleccionadaParaConstruir != -1 && zonaSeleccionadaParaConstruir < numZonasConstruccion && zonasConstruccion[zonaSeleccionadaParaConstruir])
            tituloMenuStr = "Construir Zona (" + std::to_string(zonasConstruccion[zonaSeleccionadaParaConstruir]->x) + "," + std::to_string(zonasConstruccion[zonaSeleccionadaParaConstruir]->y) + "):";
        else tituloMenuStr = "MENU CONSTRUCCION";
        movexy(menuX + 2, menuY + 1); std::cout << tituloMenuStr.substr(0, menuAncho - 4);
        movexy(menuX + 2, menuY + 2); std::cout << std::string(menuAncho - 4, '-');
        for (int i = 0; i < (int)RECETAS_MUNDO3.size(); ++i) { // typecast para comparacion con recetaSeleccionadaMenuConstruccion
            movexy(menuX + 2, menuY + 4 + i);
            Console::BackgroundColor = (i == recetaSeleccionadaMenuConstruccion) ? ConsoleColor::Cyan : ConsoleColor::DarkBlue;
            Console::ForegroundColor = (i == recetaSeleccionadaMenuConstruccion) ? ConsoleColor::Black : ConsoleColor::White;
            std::string textoOpcion = RECETAS_MUNDO3[i].nombreConstruccion + " (Req: " + std::to_string(RECETAS_MUNDO3[i].req1_cantidad) + " " + nombreTipoRecurso(RECETAS_MUNDO3[i].req1_tipo) + ", " + std::to_string(RECETAS_MUNDO3[i].req2_cantidad) + " " + nombreTipoRecurso(RECETAS_MUNDO3[i].req2_tipo) + ")";
            std::cout << textoOpcion.substr(0, menuAncho - 3);
            for (int k_clean = textoOpcion.length(); k_clean < menuAncho - 3; ++k_clean) std::cout << " ";
        }
        Console::BackgroundColor = ConsoleColor::Black; Console::ForegroundColor = ConsoleColor::White;
    }

    void mostrarMenuPrincipal() {
        int opcionSeleccionada = 0; const int numOpciones = 4;
        while (enMenuPrincipal && !salirDelPrograma) {
            Console::Clear(); dibujarBordesMenu(); dibujarTituloASCII_JUEGO(); dibujarOpcionesMenu(opcionSeleccionada);
            char entrada = _getch();
            if (tolower(entrada) == 'w') { opcionSeleccionada = (opcionSeleccionada - 1 + numOpciones) % numOpciones; hacerSonido(600, 50); }
            else if (tolower(entrada) == 's') { opcionSeleccionada = (opcionSeleccionada + 1) % numOpciones; hacerSonido(600, 50); }
            else if (entrada == 13) { Console::Clear(); ejecutarOpcionMenu(opcionSeleccionada); hacerSonido(800, 100); }
            else if (entrada == 27) salirDelPrograma = true;
        }
    }

    void dibujarBordesMenu() {
        Console::ForegroundColor = ConsoleColor::Cyan; int ancho = ANCHO_CONSOLA - 1, alto = ALTO_CONSOLA - 1;
        for (int i = 0; i <= ancho; ++i) { movexy(i, 0); std::cout << "="; movexy(i, alto); std::cout << "="; }
        for (int i = 1; i < alto; ++i) { movexy(0, i); std::cout << "|"; movexy(ancho, i); std::cout << "|"; }
        movexy(0, 0); std::cout << "+"; movexy(ancho, 0); std::cout << "+"; movexy(0, alto); std::cout << "+"; movexy(ancho, alto); std::cout << "+";
        Console::ForegroundColor = ConsoleColor::White;
    }

    void dibujarTituloASCII_JUEGO() {
        Console::ForegroundColor = ConsoleColor::Yellow; int inicioY = 3;
        int tituloAncho = (TITULO_ASCII_JUEGO.empty() ? 0 : TITULO_ASCII_JUEGO[0].length());
        int inicioX = (ANCHO_CONSOLA - tituloAncho) / 2; if (inicioX < 0) inicioX = 0;
        for (int i = 0; i < (int)TITULO_ASCII_JUEGO.size(); ++i) { movexy(inicioX, inicioY + i); std::cout << TITULO_ASCII_JUEGO[i]; }
        Console::ForegroundColor = ConsoleColor::White;
    }

    void dibujarOpcionesMenu(int seleccionada) {
        const char* opciones[] = { "Iniciar Juego", "Instrucciones", "Creditos", "Salir" };
        int inicioY = 12; int inicioX = ANCHO_CONSOLA / 2 - 8;
        for (int i = 0; i < 4; ++i) {
            movexy(inicioX - 2, inicioY + i * 2); std::cout << ((i == seleccionada) ? ">" : " ");
            movexy(inicioX, inicioY + i * 2); Console::ForegroundColor = (i == seleccionada) ? ConsoleColor::Green : ConsoleColor::Gray; std::cout << opciones[i];
        }
        Console::ForegroundColor = ConsoleColor::White;
    }

    void ejecutarOpcionMenu(int opcion) {
        switch (opcion) {
        case 0: // iniciar juego
            juegoTerminado = false; victoria = false;
            ticksRestantesJuego = TIEMPO_LIMITE_SEGUNDOS * TICKS_POR_SEGUNDO;
            if (jugador) { delete jugador; jugador = nullptr; } // liberar jugador anterior
            // liberar todas las entidades persistentes y no persistentes para una nueva partida
            liberarEntidadesNoPersistentes(); // limpia enemigos, recursos de m1/m2, aliados, balas
            if (portalesActivos != nullptr) { for (int i = 0; i < numPortalesActivos; ++i) if (portalesActivos[i]) delete portalesActivos[i]; delete[] portalesActivos; portalesActivos = nullptr; numPortalesActivos = 0; capacidadPortales = 0; }
            if (zonasConstruccion != nullptr) { for (int i = 0; i < numZonasConstruccion; ++i) if (zonasConstruccion[i]) delete zonasConstruccion[i]; delete[] zonasConstruccion; zonasConstruccion = nullptr; numZonasConstruccion = 0; }
            cuadrantesConstruidos = 0; mundo3Inicializado = false; // resetear estado de mundo 3
            inicializarMundo(MUNDO_1); // empezar en mundo 1
            break;
        case 1: mostrarInstrucciones(); break;
        case 2: mostrarCreditos(); break;
        case 3: salirDelPrograma = true; break;
        }
    }

    void mostrarPantallaTexto(const char* titulo, const std::vector<std::string>& texto) {
        dibujarBordesMenu(); int tituloLen = strlen(titulo);
        int inicioXTitulo = (ANCHO_CONSOLA / 2) - (tituloLen / 2); if (inicioXTitulo < 1) inicioXTitulo = 1;
        movexy(inicioXTitulo, 3); Console::ForegroundColor = ConsoleColor::Yellow; std::cout << titulo;
        int inicioY = 6; Console::ForegroundColor = ConsoleColor::White;
        if (!texto.empty()) {
            for (int i = 0; i < (int)texto.size(); ++i) { // typecast para evitar warning con (int)i
                if (inicioY + i < ALTO_CONSOLA - 3) { movexy(4, inicioY + i); std::cout << texto[i]; }
            }
        }
        movexy(4, ALTO_CONSOLA - 3); std::cout << "Presiona Enter para volver al menu...";
        while (_getch() != 13);
    }

    void mostrarInstrucciones() {
        mostrarPantallaTexto("INSTRUCCIONES", {
            "Bienvenido Vigilante del Equilibrio. Muevete con W,A,S,D.",
            "Recolecta recursos en Mundos 1 y 2 para construir el Mundo 3.",
            "Usa 'C' sobre zonas vacias en Mundo 3 para construir.",
            "Esquiva enemigos y sus disparos ('o').",
            "Aliados: Sanador (+H+), Congela-Tiempo (&T&), Congela-Enemigos (*E*).",
            "Pierdes si te quedas sin vidas o si se acaba el tiempo en M1/M2.",
            "Completa el " + std::to_string(OBJETIVO_CONSTRUCCION_PORCENTAJE) + "% del Mundo 3 para ganar.",
            "Presiona ESC durante el juego para volver al menu."
            });
    }
    void mostrarCreditos() {
        mostrarPantallaTexto("CREDITOS", { "Juego desarrollado por:", "- github.com/artuvall", "Curso: Algoritmos", "UPC", "2025-1" });
    }

    void dibujarBordesJuego() {
        char bv = '|', bh = '-'; ConsoleColor cb = ConsoleColor::DarkGray;
        if (mundoActual == MUNDO_1) { bv = 179; bh = 196; cb = ConsoleColor::DarkGreen; }
        else if (mundoActual == MUNDO_2) { bv = 186; bh = 205; cb = ConsoleColor::DarkCyan; }
        else if (mundoActual == MUNDO_3_CONSTRUCCION) { bv = '#'; bh = '#'; cb = ConsoleColor::Gray; }
        Console::ForegroundColor = cb;
        for (int i = 0; i < anchoAreaJuego; ++i) { movexy(i, 0); std::cout << bh; movexy(i, altoAreaJuego - 1); std::cout << bh; }
        for (int i = 1; i < altoAreaJuego - 1; ++i) { movexy(0, i); std::cout << bv; movexy(anchoAreaJuego - 1, i); std::cout << bv; }
        movexy(0, 0); std::cout << "+"; movexy(anchoAreaJuego - 1, 0); std::cout << "+"; movexy(0, altoAreaJuego - 1); std::cout << "+"; movexy(anchoAreaJuego - 1, altoAreaJuego - 1); std::cout << "+";
        Console::ForegroundColor = ConsoleColor::White;
    }

    void dibujarBordeHUD() {
        Console::ForegroundColor = ConsoleColor::DarkCyan;
        int x1 = DESPLAZAMIENTO_X_HUD - 1, y1 = POSICION_Y_HUD - 1; int x2 = ANCHO_CONSOLA - 1, y2 = ALTO_CONSOLA - 1;
        for (int x = x1; x <= x2; ++x) { movexy(x, y1); std::cout << "-"; movexy(x, y2); std::cout << "-"; }
        for (int y = y1 + 1; y < y2; ++y) { movexy(x1, y); std::cout << "|"; movexy(x2, y); std::cout << "|"; }
        movexy(x1, y1); std::cout << "+"; movexy(x2, y1); std::cout << "+"; movexy(x1, y2); std::cout << "+"; movexy(x2, y2); std::cout << "+";
        Console::ForegroundColor = ConsoleColor::White;
    }

    void imprimirConAjusteYLimpieza(int& linea_y_actual, const std::string& texto, int anchoMax) { // "linea_y_actual" en lugar de "currenty"
        if (linea_y_actual >= ALTO_CONSOLA - 1) return;
        size_t pos = 0;
        while (pos < texto.length()) {
            if (linea_y_actual >= ALTO_CONSOLA - 1) break;
            std::string sub = texto.substr(pos, anchoMax);
            movexy(DESPLAZAMIENTO_X_HUD, linea_y_actual); std::cout << sub;
            for (int k = sub.length(); k < anchoMax; ++k) std::cout << " "; // limpiado con int para k
            pos += anchoMax; linea_y_actual++;
        }
    }

    void dibujarHUD() {
        Console::ForegroundColor = ConsoleColor::Yellow;
        int lineaYActualHUD = POSICION_Y_HUD; // reemplazo de currenty
        int hudAnchoInterior = ANCHO_CONSOLA - DESPLAZAMIENTO_X_HUD - 1;

        imprimirConAjusteYLimpieza(lineaYActualHUD, "--- ESTADO ---", hudAnchoInterior);
        imprimirConAjusteYLimpieza(lineaYActualHUD, "Mundo: " + std::to_string(mundoActual), hudAnchoInterior);
        if (jugador) {
            imprimirConAjusteYLimpieza(lineaYActualHUD, "Vidas: " + std::to_string(jugador->getVidas()), hudAnchoInterior);
            imprimirConAjusteYLimpieza(lineaYActualHUD, "Inventario: " + std::to_string(jugador->getTotalRecursosInventario()) + "/" + std::to_string(CAPACIDAD_INVENTARIO), hudAnchoInterior);
            if (mundoActual == MUNDO_3_CONSTRUCCION) { int porcentaje = (numZonasConstruccion > 0) ? (cuadrantesConstruidos * 100 / numZonasConstruccion) : 0; imprimirConAjusteYLimpieza(lineaYActualHUD, "Construido: " + std::to_string(porcentaje) + "%", hudAnchoInterior); }
        }
        else {
            imprimirConAjusteYLimpieza(lineaYActualHUD, "Vidas: ---", hudAnchoInterior); imprimirConAjusteYLimpieza(lineaYActualHUD, "Inventario: ---/" + std::to_string(CAPACIDAD_INVENTARIO), hudAnchoInterior);
            if (mundoActual == MUNDO_3_CONSTRUCCION) imprimirConAjusteYLimpieza(lineaYActualHUD, "Construido: 0%", hudAnchoInterior);
        }
        int segundosRestantes = (ticksRestantesJuego >= 0) ? (ticksRestantesJuego / TICKS_POR_SEGUNDO) : 0;
        if (mundoActual != MUNDO_3_CONSTRUCCION) { if (ticksCongelacionTiempoRestantes > 0) imprimirConAjusteYLimpieza(lineaYActualHUD, "Tiempo: CONGELADO", hudAnchoInterior); else imprimirConAjusteYLimpieza(lineaYActualHUD, "Tiempo: " + std::to_string(segundosRestantes) + "s", hudAnchoInterior); }
        else imprimirConAjusteYLimpieza(lineaYActualHUD, "Tiempo: N/A ", hudAnchoInterior);

        lineaYActualHUD++; // espacio
        imprimirConAjusteYLimpieza(lineaYActualHUD, "-- Inventario --", hudAnchoInterior);
        if (jugador && jugador->getTotalRecursosInventario() > 0) {
            bool hayRecursosMostrados = false;
            for (int i = 0; i < NUM_TIPOS_RECURSOS; ++i) { // num_tipos_recursos debe estar definido
                TipoRecurso tipoActualEnum;
                if (i == 0) tipoActualEnum = TipoRecurso::EMPATIA; else if (i == 1) tipoActualEnum = TipoRecurso::ETICA; else if (i == 2) tipoActualEnum = TipoRecurso::ADAPTABILIDAD;
                else if (i == 3) tipoActualEnum = TipoRecurso::NUCLEO_IA; else if (i == 4) tipoActualEnum = TipoRecurso::PAQUETE_DATOS; else if (i == 5) tipoActualEnum = TipoRecurso::CELULA_ENERGIA;
                else continue;
                int cantidad = jugador->getCantidadRecurso(tipoActualEnum);
                if (cantidad > 0) { if (lineaYActualHUD >= ALTO_CONSOLA - 5) break; imprimirConAjusteYLimpieza(lineaYActualHUD, nombreTipoRecurso(tipoActualEnum) + ": " + std::to_string(cantidad), hudAnchoInterior); hayRecursosMostrados = true; }
            }
            if (!hayRecursosMostrados && lineaYActualHUD < ALTO_CONSOLA - 5) imprimirConAjusteYLimpieza(lineaYActualHUD, " (Vacio)", hudAnchoInterior);
        }
        else if (lineaYActualHUD < ALTO_CONSOLA - 5) imprimirConAjusteYLimpieza(lineaYActualHUD, " (Vacio)", hudAnchoInterior);

        int y_para_feedback;
        if (mundoActual == MUNDO_3_CONSTRUCCION && !mostrandoMenuConstruccion) y_para_feedback = ALTO_CONSOLA - 5; else y_para_feedback = ALTO_CONSOLA - 4;
        int y_inicio_limpieza_feedback = lineaYActualHUD + 1; if (y_inicio_limpieza_feedback > y_para_feedback) y_inicio_limpieza_feedback = y_para_feedback;
        for (int y_clear = y_inicio_limpieza_feedback; y_clear <= y_para_feedback; ++y_clear) { if (y_clear >= POSICION_Y_HUD && y_clear < ALTO_CONSOLA - 1) { movexy(DESPLAZAMIENTO_X_HUD, y_clear); std::cout << std::string(hudAnchoInterior, ' '); } }
        if (!mensajeHUDInferior.empty()) {
            int tempY_feedback = y_para_feedback;
            if (mensajeHUDInferior.length() > (size_t)hudAnchoInterior && y_para_feedback > y_inicio_limpieza_feedback) tempY_feedback = y_para_feedback - 1;
            if (tempY_feedback < y_inicio_limpieza_feedback) tempY_feedback = y_inicio_limpieza_feedback;
            imprimirConAjusteYLimpieza(tempY_feedback, mensajeHUDInferior, hudAnchoInterior);
        }
        int posY_cMsg = ALTO_CONSOLA - 4;
        if (posY_cMsg >= POSICION_Y_HUD + 1) { movexy(DESPLAZAMIENTO_X_HUD, posY_cMsg); std::cout << std::string(hudAnchoInterior, ' '); if (mundoActual == MUNDO_3_CONSTRUCCION && !mostrandoMenuConstruccion) { std::string textoConstruir = "C: Construir"; movexy(DESPLAZAMIENTO_X_HUD, posY_cMsg); std::cout << textoConstruir.substr(0, hudAnchoInterior); } }
        int posY_escMenu = ALTO_CONSOLA - 3;
        if (posY_escMenu >= POSICION_Y_HUD + 1) { movexy(DESPLAZAMIENTO_X_HUD, posY_escMenu); std::cout << std::string(hudAnchoInterior, ' '); std::string textoMenu = "ESC: Menu"; movexy(DESPLAZAMIENTO_X_HUD, posY_escMenu); std::cout << textoMenu.substr(0, hudAnchoInterior); }
        Console::ForegroundColor = ConsoleColor::White;
    }

    void mostrarPantallaFinJuego() {
        Console::Clear(); std::string msgP;
        if (victoria) { msgP = "¡¡¡ GANASTE !!!"; Console::ForegroundColor = ConsoleColor::Green; }
        else { msgP = "--- PERDISTE ---"; Console::ForegroundColor = ConsoleColor::Red; }
        int posYP = (ALTO_CONSOLA / 2) - 6; int posXMsgP = (ANCHO_CONSOLA - msgP.length()) / 2; if (posXMsgP < 0) posXMsgP = 0;
        movexy(posXMsgP, posYP); std::cout << msgP;
        std::string msgV = "Presiona Enter para volver al menu..."; int posXMsgV = (ANCHO_CONSOLA - msgV.length()) / 2; if (posXMsgV < 0) posXMsgV = 0;
        movexy(posXMsgV, posYP + 2); Console::ForegroundColor = ConsoleColor::White; std::cout << msgV;
        while (_getch() != 13);
        enMenuPrincipal = true; nivelEnCurso = false;
    }

public:
    Juego() { inicializarVariablesGlobales(); }
    ~Juego() {
        // liberar todas las entidades al final
        liberarEntidadesNoPersistentes();
        if (jugador) { delete jugador; jugador = nullptr; }
        if (portalesActivos != nullptr) { for (int i = 0; i < numPortalesActivos; ++i) if (portalesActivos[i]) delete portalesActivos[i]; delete[] portalesActivos; portalesActivos = nullptr; }
        if (zonasConstruccion != nullptr) { for (int i = 0; i < numZonasConstruccion; ++i) if (zonasConstruccion[i]) delete zonasConstruccion[i]; delete[] zonasConstruccion; zonasConstruccion = nullptr; }
    }
    void cicloPrincipal() {
        while (!salirDelPrograma) {
            if (enMenuPrincipal) mostrarMenuPrincipal();
            else if (nivelEnCurso) {
                procesarEntradaJuego();
                if (nivelEnCurso && !mostrandoMenuConstruccion) actualizarEstadoJuego();
                if (nivelEnCurso) renderizarJuego();
                Sleep(100); // controla velocidad
                if (juegoTerminado) mostrarPantallaFinJuego();
            }
            else salirDelPrograma = true; // estado inesperado
        }
        Console::Clear(); movexy(10, 10); std::cout << "Saliendo del programa..." << std::endl; Sleep(1000);
    }
};

