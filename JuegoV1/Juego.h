#pragma once
#include <vector>
#include <string>
#include <windows.h> //sleep, beep
#include <conio.h>   // _kbhit, _getch
#include <cstdlib>   //rand, srand
#include <ctime>     //time en srand
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
        representacionTemporal = sym;
        anchoRepresentacion = 1;
    }
    void dibujar() const { Console::ForegroundColor = color; movexy(x, y); std::cout << simbolo; Console::ForegroundColor = ConsoleColor::White; }
};

enum class EstadoCuadrante { VACIO, CONSTRUIDO_TIPO_A, CONSTRUIDO_TIPO_B, CONSTRUIDO_TIPO_C };
enum class Idioma { ESPANOL, INGLES };


struct RecetaConstruccion {
    TipoRecurso req1_tipo; int req1_cantidad;
    TipoRecurso req2_tipo; int req2_cantidad;
    EstadoCuadrante resultadoConstruccion;
    std::string nombreConstruccion_ES; //espanol
    std::string nombreConstruccion_EN; //ingles
};

const std::vector<RecetaConstruccion> RECETAS_MUNDO3 = {
    {TipoRecurso::EMPATIA, 1, TipoRecurso::ETICA, 1, EstadoCuadrante::CONSTRUIDO_TIPO_A, "Centro Comunal (M1+M1)", "Community Center (W1+W1)"},
    {TipoRecurso::NUCLEO_IA, 1, TipoRecurso::CELULA_ENERGIA, 1, EstadoCuadrante::CONSTRUIDO_TIPO_B, "Red Energetica (M2+M2)", "Power Grid (W2+W2)"},
    {TipoRecurso::ADAPTABILIDAD, 1, TipoRecurso::PAQUETE_DATOS, 1, EstadoCuadrante::CONSTRUIDO_TIPO_C, "Nodo Invest. (M1+M2)", "Research Node (W1+W2)"}
};


struct ZonaConstruccionM3 {
    int x, y; int ancho, alto; EstadoCuadrante estado; char simboloVacio; char simboloConstruidoBase;
    ZonaConstruccionM3(int px = 0, int py = 0, int w = 6, int h = 3, EstadoCuadrante est = EstadoCuadrante::VACIO) : x(px), y(py), ancho(w), alto(h), estado(est), simboloVacio('.'), simboloConstruidoBase('#') {}
    void dibujar() const {
        ConsoleColor colorZona = ConsoleColor::DarkGray; char simboloDibujo = simboloVacio;
        if (estado == EstadoCuadrante::VACIO) { simboloDibujo = simboloVacio; colorZona = ConsoleColor::DarkGray; }
        else if (estado == EstadoCuadrante::CONSTRUIDO_TIPO_A) { simboloDibujo = 'A'; colorZona = ConsoleColor::Green; }
        else if (estado == EstadoCuadrante::CONSTRUIDO_TIPO_B) { simboloDibujo = 'B'; colorZona = ConsoleColor::Cyan; }
        else if (estado == EstadoCuadrante::CONSTRUIDO_TIPO_C) { simboloDibujo = 'C'; colorZona = ConsoleColor::Yellow; }
        Console::ForegroundColor = colorZona;
        for (int i = 0; i < alto; ++i) for (int j = 0; j < ancho; ++j) if (x + j < ANCHO_JUEGO - 1 && y + i < ALTO_JUEGO - 1) {
            movexy(x + j, y + i);
            if (i == alto / 2 && j == ancho / 2) std::cout << simboloDibujo;
            else if (i == 0 || i == alto - 1 || j == 0 || j == ancho - 1) std::cout << char(177); else std::cout << " ";
        }
        Console::ForegroundColor = ConsoleColor::White;
    }
    bool colisionaCon(int pX, int pY, int pAncho, int pAlto = 1) const { return (pX < x + ancho && pX + pAncho > x && pY < y + alto && pY + pAlto > y); }
};

class Juego {
private:
    int anchoAreaJuego, altoAreaJuego; int mundoActual;
    bool enMenuPrincipal, nivelEnCurso, juegoTerminado, victoria, salirDelPrograma;
    Personaje* jugador;
    Enemigo** enemigos; int numEnemigosActual, capacidadEnemigos;
    Recurso** recursos; int numRecursosActual, capacidadRecursos;
    Aliado** aliados; int numAliadosActual, capacidadAliados;
    Bala** balas; int numBalasActual, capacidadBalas;
    Portal** portalesActivos; int numPortalesActivos, capacidadPortales;
    int ticksRestantesJuego; int ticksCongelacionTiempoRestantes, ticksCongelacionEnemigosRestantes;
    std::string mensajeHUDInferior;
    ZonaConstruccionM3** zonasConstruccion; int numZonasConstruccion; int cuadrantesConstruidos;
    bool mundo3Inicializado; int zonaSeleccionadaParaConstruir;
    bool mostrandoMenuConstruccion; int recetaSeleccionadaMenuConstruccion;
    Idioma idiomaActual; // para el idioma

    void inicializarVariablesGlobales() {
        anchoAreaJuego = ANCHO_JUEGO; altoAreaJuego = ALTO_JUEGO; mundoActual = MUNDO_1;
        enMenuPrincipal = true; nivelEnCurso = false; juegoTerminado = false; victoria = false; salirDelPrograma = false;
        jugador = nullptr; enemigos = nullptr; numEnemigosActual = 0; capacidadEnemigos = 0;
        recursos = nullptr; numRecursosActual = 0; capacidadRecursos = 0; aliados = nullptr; numAliadosActual = 0; capacidadAliados = 0;
        balas = nullptr; numBalasActual = 0; capacidadBalas = 0; portalesActivos = nullptr; numPortalesActivos = 0; capacidadPortales = 0;
        zonasConstruccion = nullptr; numZonasConstruccion = 0;
        ticksRestantesJuego = TIEMPO_LIMITE_SEGUNDOS * TICKS_POR_SEGUNDO;
        ticksCongelacionTiempoRestantes = 0; ticksCongelacionEnemigosRestantes = 0;
        mensajeHUDInferior = ""; cuadrantesConstruidos = 0; mundo3Inicializado = false;
        mostrandoMenuConstruccion = false; recetaSeleccionadaMenuConstruccion = 0; zonaSeleccionadaParaConstruir = -1;
        idiomaActual = Idioma::ESPANOL; // idioma por defecto
        srand((unsigned int)time(NULL));
        Console::SetWindowSize(ANCHO_CONSOLA, ALTO_CONSOLA); Console::CursorVisible = false;
    }

    template<typename T>
    void agregarElemento(T**& arreglo, int& contador, int& capacidad, T* elemento) {
        if (contador == capacidad) { int nuevaCapacidad = (capacidad == 0) ? 5 : capacidad * 2; T** temp = new T * [nuevaCapacidad]; for (int i = 0;i < contador;++i)temp[i] = arreglo[i]; delete[] arreglo; arreglo = temp; capacidad = nuevaCapacidad; }
        arreglo[contador] = elemento; contador++;
    }

    bool esPosicionOcupada(int posX, int posY, int anchoEnt, int altoEnt = 1, bool chequearJugador = true) {
        for (int i = 0;i < numPortalesActivos;++i) if (portalesActivos[i] && posX<portalesActivos[i]->x + portalesActivos[i]->anchoRepresentacion && posX + anchoEnt>portalesActivos[i]->x && posY<portalesActivos[i]->y + 1 && posY + altoEnt>portalesActivos[i]->y) return true;
        for (int i = 0;i < numRecursosActual;++i) if (recursos[i] && posX<recursos[i]->getX() + recursos[i]->getAnchoRepresentacion() && posX + anchoEnt>recursos[i]->getX() && posY<recursos[i]->getY() + 1 && posY + altoEnt>recursos[i]->getY()) return true;
        for (int i = 0;i < numAliadosActual;++i) if (aliados[i] && posX<aliados[i]->getX() + aliados[i]->getAnchoRepresentacion() && posX + anchoEnt>aliados[i]->getX() && posY<aliados[i]->getY() + 1 && posY + altoEnt>aliados[i]->getY()) return true;
        if (chequearJugador && jugador && nivelEnCurso && posX<jugador->getX() + jugador->getAnchoRepresentacion() && posX + anchoEnt>jugador->getX() && posY<jugador->getY() + 1 && posY + altoEnt>jugador->getY()) return true;
        return false;
    }

    void generarPortalesAleatorios(int /*mundoOrigen*/, int mundoDestino1, char s1, ConsoleColor c1, int mundoDestino2, char s2, ConsoleColor c2) {
        int pX1, pY1, pX2, pY2; int anchoPortal = 1;
        do { pX1 = rand() % (anchoAreaJuego - (anchoPortal + 2)) + 1; pY1 = rand() % (altoAreaJuego - 2) + 1; } while (esPosicionOcupada(pX1, pY1, anchoPortal, 1, false));
        agregarElemento(portalesActivos, numPortalesActivos, capacidadPortales, new Portal(pX1, pY1, mundoDestino1, s1, c1));
        do { pX2 = rand() % (anchoAreaJuego - (anchoPortal + 2)) + 1;pY2 = rand() % (altoAreaJuego - 2) + 1; } while ((pX2 == pX1 && pY2 == pY1) || esPosicionOcupada(pX2, pY2, anchoPortal, 1, false));
        agregarElemento(portalesActivos, numPortalesActivos, capacidadPortales, new Portal(pX2, pY2, mundoDestino2, s2, c2));
    }

    void liberarEntidadesNoPersistentes() {
        if (enemigos != nullptr) { for (int i = 0;i < numEnemigosActual;++i)if (enemigos[i])delete enemigos[i];delete[] enemigos;enemigos = nullptr;numEnemigosActual = 0;capacidadEnemigos = 0; }
        if (recursos != nullptr) { for (int i = 0;i < numRecursosActual;++i)if (recursos[i])delete recursos[i];delete[] recursos;recursos = nullptr;numRecursosActual = 0;capacidadRecursos = 0; }
        if (aliados != nullptr) { for (int i = 0;i < numAliadosActual;++i)if (aliados[i])delete aliados[i];delete[] aliados;aliados = nullptr;numAliadosActual = 0;capacidadAliados = 0; }
        if (balas != nullptr) { for (int i = 0;i < numBalasActual;++i)if (balas[i])delete balas[i];delete[] balas;balas = nullptr;numBalasActual = 0;capacidadBalas = 0; }
        ticksCongelacionTiempoRestantes = 0;ticksCongelacionEnemigosRestantes = 0;mensajeHUDInferior = "";
    }

    void inicializarMundo(int numeroMundo) {
        if (mundoActual != MUNDO_3_CONSTRUCCION || numeroMundo != MUNDO_3_CONSTRUCCION) liberarEntidadesNoPersistentes();
        if (portalesActivos != nullptr) { for (int i = 0;i < numPortalesActivos;++i)if (portalesActivos[i])delete portalesActivos[i];delete[] portalesActivos;portalesActivos = nullptr;numPortalesActivos = 0;capacidadPortales = 0; }
        mundoActual = numeroMundo; nivelEnCurso = true; juegoTerminado = false; enMenuPrincipal = false; mensajeHUDInferior = "";
        int posJugadorX, posJugadorY; if (mundoActual == MUNDO_3_CONSTRUCCION) { posJugadorX = anchoAreaJuego / 2;posJugadorY = altoAreaJuego / 2; }
        else { posJugadorX = anchoAreaJuego / 10;posJugadorY = altoAreaJuego / 2; }
        if (jugador == nullptr)jugador = new Personaje(posJugadorX, posJugadorY, anchoAreaJuego, altoAreaJuego, VIDAS_INICIALES); else jugador->setPos(posJugadorX, posJugadorY);

        if (mundoActual == MUNDO_1) {
            generarPortalesAleatorios(MUNDO_1, MUNDO_2, 'P', ConsoleColor::Cyan, MUNDO_3_CONSTRUCCION, 'C', ConsoleColor::Gray);
            int nErr = NUM_ENEMIGOS_ERRANTES_BASE, nReb = NUM_ENEMIGOS_REBOTADORES_BASE, nDisp = NUM_ENEMIGOS_DISPARADORES_BASE; int gX, gY, aE;
            for (int i = 0;i < nErr;++i) { aE = 3;do { gX = rand() % (anchoAreaJuego - (aE + 2)) + 1;gY = rand() % (altoAreaJuego - 2) + 1; } while (esPosicionOcupada(gX, gY, aE));agregarElemento(enemigos, numEnemigosActual, capacidadEnemigos, new Enemigo(gX, gY, anchoAreaJuego, altoAreaJuego, TipoEnemigo::ERRANTE_ALEATORIO)); }
            for (int i = 0;i < nReb;++i) { aE = 3;do { gX = rand() % (anchoAreaJuego - (aE + 2)) + 1;gY = rand() % (altoAreaJuego - 2) + 1; } while (esPosicionOcupada(gX, gY, aE));agregarElemento(enemigos, numEnemigosActual, capacidadEnemigos, new Enemigo(gX, gY, anchoAreaJuego, altoAreaJuego, TipoEnemigo::REBOTADOR)); }
            for (int i = 0;i < nDisp;++i) { int yD = (i + 1) * (altoAreaJuego / (nDisp + 1));if (yD <= 0)yD = 1;if (yD >= altoAreaJuego - 1)yD = altoAreaJuego - 2;int xD = (i % 2 == 0) ? 1 : (anchoAreaJuego - 1 - 3);agregarElemento(enemigos, numEnemigosActual, capacidadEnemigos, new Enemigo(xD, yD, anchoAreaJuego, altoAreaJuego, TipoEnemigo::DISPARADOR)); }
            for (int i = 0;i < TOTAL_RECURSOS_MUNDO_NORMAL;++i) {
                TipoRecurso tR;int rV = rand() % 3;if (rV == 0)tR = TipoRecurso::EMPATIA;else if (rV == 1)tR = TipoRecurso::ETICA;else tR = TipoRecurso::ADAPTABILIDAD;
                int idxDis = 0;if (tR == TipoRecurso::EMPATIA)idxDis = 0;else if (tR == TipoRecurso::ETICA)idxDis = 1;else if (tR == TipoRecurso::ADAPTABILIDAD)idxDis = 2;else if (tR == TipoRecurso::NUCLEO_IA)idxDis = 3;else if (tR == TipoRecurso::PAQUETE_DATOS)idxDis = 4;else idxDis = 5;
                if (idxDis >= 0 && idxDis < (int)DISEÑOS_RECURSO.size())aE = DISEÑOS_RECURSO[idxDis].length();else aE = 3;
                do { gX = rand() % (anchoAreaJuego - (aE + 2)) + 1;gY = rand() % (altoAreaJuego - 2) + 1; } while (esPosicionOcupada(gX, gY, aE));
                agregarElemento(recursos, numRecursosActual, capacidadRecursos, new Recurso(gX, gY, anchoAreaJuego, altoAreaJuego, tR));
            }
            int nMaxAl = (rand() % 2) + 1;
            for (int i = 0;i < nMaxAl;++i) {
                TipoAliado tA;int rVA = rand() % 3;if (rVA == 0)tA = TipoAliado::SANADOR;else if (rVA == 1)tA = TipoAliado::CONGELADOR_TIEMPO;else tA = TipoAliado::CONGELADOR_ENEMIGOS;
                int idxDisAl = 0;if (tA == TipoAliado::SANADOR)idxDisAl = 0;else if (tA == TipoAliado::CONGELADOR_TIEMPO)idxDisAl = 1;else idxDisAl = 2;
                if (idxDisAl >= 0 && idxDisAl < (int)DISEÑOS_ALIADO.size())aE = DISEÑOS_ALIADO[idxDisAl].length();else aE = 5;
                do { gX = rand() % (anchoAreaJuego - (aE + 2)) + 1;gY = rand() % (altoAreaJuego - 2) + 1; } while (esPosicionOcupada(gX, gY, aE));
                agregarElemento(aliados, numAliadosActual, capacidadAliados, new Aliado(gX, gY, anchoAreaJuego, altoAreaJuego, tA));
            }
        }
        else if (mundoActual == MUNDO_2) {
            generarPortalesAleatorios(MUNDO_2, MUNDO_1, 'P', ConsoleColor::Green, MUNDO_3_CONSTRUCCION, 'C', ConsoleColor::Gray);
            int nErr = NUM_ENEMIGOS_ERRANTES_BASE + 1, nReb = NUM_ENEMIGOS_REBOTADORES_BASE + 1, nDisp = NUM_ENEMIGOS_DISPARADORES_BASE - 1;if (nDisp < 0)nDisp = 0; int gX, gY, aE;
            for (int i = 0;i < nErr;++i) { aE = 3;do { gX = rand() % (anchoAreaJuego - (aE + 2)) + 1;gY = rand() % (altoAreaJuego - 2) + 1; } while (esPosicionOcupada(gX, gY, aE));agregarElemento(enemigos, numEnemigosActual, capacidadEnemigos, new Enemigo(gX, gY, anchoAreaJuego, altoAreaJuego, TipoEnemigo::ERRANTE_ALEATORIO)); }
            for (int i = 0;i < nReb;++i) { aE = 3;do { gX = rand() % (anchoAreaJuego - (aE + 2)) + 1;gY = rand() % (altoAreaJuego - 2) + 1; } while (esPosicionOcupada(gX, gY, aE));agregarElemento(enemigos, numEnemigosActual, capacidadEnemigos, new Enemigo(gX, gY, anchoAreaJuego, altoAreaJuego, TipoEnemigo::REBOTADOR)); }
            for (int i = 0;i < nDisp;++i) { int yD = (i + 1) * (altoAreaJuego / (nDisp + 1));if (yD <= 0)yD = 1;if (yD >= altoAreaJuego - 1)yD = altoAreaJuego - 2;int xD = (i % 2 == 0) ? 1 : (anchoAreaJuego - 1 - 3);agregarElemento(enemigos, numEnemigosActual, capacidadEnemigos, new Enemigo(xD, yD, anchoAreaJuego, altoAreaJuego, TipoEnemigo::DISPARADOR)); }
            for (int i = 0;i < TOTAL_RECURSOS_MUNDO_NORMAL;++i) {
                TipoRecurso tR;int rV = rand() % 3;if (rV == 0)tR = TipoRecurso::NUCLEO_IA;else if (rV == 1)tR = TipoRecurso::PAQUETE_DATOS;else tR = TipoRecurso::CELULA_ENERGIA;
                int idxDis = 0;if (tR == TipoRecurso::EMPATIA)idxDis = 0;else if (tR == TipoRecurso::ETICA)idxDis = 1;else if (tR == TipoRecurso::ADAPTABILIDAD)idxDis = 2;else if (tR == TipoRecurso::NUCLEO_IA)idxDis = 3;else if (tR == TipoRecurso::PAQUETE_DATOS)idxDis = 4;else idxDis = 5;
                if (idxDis >= 0 && idxDis < (int)DISEÑOS_RECURSO.size())aE = DISEÑOS_RECURSO[idxDis].length();else aE = 3;
                do { gX = rand() % (anchoAreaJuego - (aE + 2)) + 1;gY = rand() % (altoAreaJuego - 2) + 1; } while (esPosicionOcupada(gX, gY, aE));
                agregarElemento(recursos, numRecursosActual, capacidadRecursos, new Recurso(gX, gY, anchoAreaJuego, altoAreaJuego, tR));
            }
            int nMaxAl = (rand() % 2) + 1;
            for (int i = 0;i < nMaxAl;++i) {
                TipoAliado tA;int rVA = rand() % 3;if (rVA == 0)tA = TipoAliado::SANADOR;else if (rVA == 1)tA = TipoAliado::CONGELADOR_TIEMPO;else tA = TipoAliado::CONGELADOR_ENEMIGOS;
                int idxDisAl = 0;if (tA == TipoAliado::SANADOR)idxDisAl = 0;else if (tA == TipoAliado::CONGELADOR_TIEMPO)idxDisAl = 1;else idxDisAl = 2;
                if (idxDisAl >= 0 && idxDisAl < (int)DISEÑOS_ALIADO.size())aE = DISEÑOS_ALIADO[idxDisAl].length();else aE = 5;
                do { gX = rand() % (anchoAreaJuego - (aE + 2)) + 1;gY = rand() % (altoAreaJuego - 2) + 1; } while (esPosicionOcupada(gX, gY, aE));
                agregarElemento(aliados, numAliadosActual, capacidadAliados, new Aliado(gX, gY, anchoAreaJuego, altoAreaJuego, tA));
            }
        }
        else if (mundoActual == MUNDO_3_CONSTRUCCION) {
            generarPortalesAleatorios(MUNDO_3_CONSTRUCCION, MUNDO_1, 'P', ConsoleColor::Green, MUNDO_2, 'P', ConsoleColor::Cyan);
            if (!mundo3Inicializado) {
                numZonasConstruccion = DIM_MUNDO3_X * DIM_MUNDO3_Y; zonasConstruccion = new ZonaConstruccionM3 * [numZonasConstruccion];
                int cA = 6, cAl = 3, idx = 0; int totGA = DIM_MUNDO3_X * cA + (DIM_MUNDO3_X - 1); int totGAl = DIM_MUNDO3_Y * cAl + (DIM_MUNDO3_Y - 1);
                int oX = (anchoAreaJuego - totGA) / 2;if (oX < 1)oX = 1; int oY = (altoAreaJuego - totGAl) / 2;if (oY < 1)oY = 1;
                for (int i = 0;i < DIM_MUNDO3_Y;++i)for (int j = 0;j < DIM_MUNDO3_X;++j)zonasConstruccion[idx++] = new ZonaConstruccionM3(oX + j * (cA + 1), oY + i * (cAl + 1), cA, cAl);
                mundo3Inicializado = true;
            }
            mostrandoMenuConstruccion = false;zonaSeleccionadaParaConstruir = -1;
        }
        Console::Clear();dibujarBordesJuego();dibujarBordeHUD();
    }

    void procesarEntradaJuego() {
        if (_kbhit()) {
            char tecla = _getch();
            if (tecla == 27) { enMenuPrincipal = true;nivelEnCurso = false;mensajeHUDInferior = "";return; }
            if (mundoActual == MUNDO_3_CONSTRUCCION && mostrandoMenuConstruccion)procesarEntradaMenuConstruccion(tecla);
            else if (jugador && jugador->estaVivo()) {
                jugador->moverConTeclado(tecla);
                if (mundoActual == MUNDO_3_CONSTRUCCION && (tolower(tecla) == 'c')) {
                    zonaSeleccionadaParaConstruir = -1;
                    for (int i = 0;i < numZonasConstruccion;++i)if (zonasConstruccion[i] && zonasConstruccion[i]->estado == EstadoCuadrante::VACIO && zonasConstruccion[i]->colisionaCon(jugador->getX(), jugador->getY(), jugador->getAnchoRepresentacion())) { zonaSeleccionadaParaConstruir = i;mostrandoMenuConstruccion = true;recetaSeleccionadaMenuConstruccion = 0;mensajeHUDInferior = (idiomaActual == Idioma::ESPANOL ? "Elige que construir..." : "Choose what to build...");break; }
                    if (zonaSeleccionadaParaConstruir == -1 && !mostrandoMenuConstruccion)mensajeHUDInferior = (idiomaActual == Idioma::ESPANOL ? "Acercate a zona vacia y pulsa C." : "Move to an empty zone and press C.");
                }
                else {
                    for (int i = 0;i < numPortalesActivos;++i)if (portalesActivos[i] && jugador->getX() < portalesActivos[i]->x + portalesActivos[i]->anchoRepresentacion && jugador->getX() + jugador->getAnchoRepresentacion() > portalesActivos[i]->x && jugador->getY() == portalesActivos[i]->y) { inicializarMundo(portalesActivos[i]->mundoDestino);return; }
                }
                if (!mostrandoMenuConstruccion)mensajeHUDInferior = "";
            }
        }
    }

    void procesarEntradaMenuConstruccion(char tecla) {
        if (tolower(tecla) == 'w')recetaSeleccionadaMenuConstruccion = (recetaSeleccionadaMenuConstruccion - 1 + RECETAS_MUNDO3.size()) % RECETAS_MUNDO3.size();
        else if (tolower(tecla) == 's')recetaSeleccionadaMenuConstruccion = (recetaSeleccionadaMenuConstruccion + 1) % RECETAS_MUNDO3.size();
        else if (tecla == 13) {
            if (zonaSeleccionadaParaConstruir != -1 && zonaSeleccionadaParaConstruir < numZonasConstruccion && zonasConstruccion[zonaSeleccionadaParaConstruir]) {
                const RecetaConstruccion& receta = RECETAS_MUNDO3[recetaSeleccionadaMenuConstruccion];
                if (jugador && jugador->consumirRecursos(receta.req1_tipo, receta.req1_cantidad, receta.req2_tipo, receta.req2_cantidad)) {
                    zonasConstruccion[zonaSeleccionadaParaConstruir]->estado = receta.resultadoConstruccion; cuadrantesConstruidos++;
                    mensajeHUDInferior = (idiomaActual == Idioma::ESPANOL ? receta.nombreConstruccion_ES : receta.nombreConstruccion_EN) + (idiomaActual == Idioma::ESPANOL ? " construido!" : " built!");
                    mostrandoMenuConstruccion = false; Console::Clear();dibujarBordesJuego();dibujarBordeHUD();
                    if (numZonasConstruccion > 0 && cuadrantesConstruidos * 100.0 / numZonasConstruccion >= OBJETIVO_CONSTRUCCION_PORCENTAJE) { juegoTerminado = true;victoria = true;nivelEnCurso = false; }
                }
                else mensajeHUDInferior = (idiomaActual == Idioma::ESPANOL ? "Recursos insuficientes!" : "Not enough resources!");
            }
        }
        else if (tecla == 27 || tolower(tecla) == 'c') { mostrandoMenuConstruccion = false;mensajeHUDInferior = "";Console::Clear();dibujarBordesJuego();dibujarBordeHUD(); }
    }

    void actualizarEstadoJuego() {
        if (!jugador || !jugador->estaVivo()) { juegoTerminado = true;victoria = false;nivelEnCurso = false;return; }
        if (ticksCongelacionTiempoRestantes > 0)ticksCongelacionTiempoRestantes--;
        else if (nivelEnCurso && !mostrandoMenuConstruccion && mundoActual != MUNDO_3_CONSTRUCCION)ticksRestantesJuego--;
        if (ticksRestantesJuego <= 0 && nivelEnCurso && (mundoActual == MUNDO_1 || mundoActual == MUNDO_2)) { juegoTerminado = true;victoria = false;nivelEnCurso = false;return; }
        if (mundoActual == MUNDO_1 || mundoActual == MUNDO_2) {
            if (ticksCongelacionEnemigosRestantes > 0)ticksCongelacionEnemigosRestantes--;
            else { for (int i = 0;i < numEnemigosActual;++i)if (enemigos[i])enemigos[i]->mover();for (int i = 0;i < numEnemigosActual;++i)if (enemigos[i])enemigos[i]->actualizarDisparador(balas, numBalasActual, capacidadBalas); }
            for (int i = numBalasActual - 1;i >= 0;--i) {
                if (!balas[i] || !balas[i]->estaActiva()) { if (balas[i])delete balas[i];if (numBalasActual > 0 && i < numBalasActual - 1)balas[i] = balas[numBalasActual - 1];if (numBalasActual > 0)numBalasActual--;continue; }
                balas[i]->mover();
                if (jugador && balas[i] && jugador->getX() < balas[i]->getX() + balas[i]->getAnchoRepresentacion() && jugador->getX() + jugador->getAnchoRepresentacion() > balas[i]->getX() && jugador->getY() == balas[i]->getY()) {
                    jugador->decrementarVida();balas[i]->desactivar();delete balas[i];if (numBalasActual > 0 && i < numBalasActual - 1)balas[i] = balas[numBalasActual - 1];if (numBalasActual > 0)numBalasActual--;
                    if (!jugador->estaVivo()) { juegoTerminado = true;victoria = false;nivelEnCurso = false;return; }continue;
                }
                if (balas[i] && !balas[i]->estaActiva()) { delete balas[i];if (numBalasActual > 0 && i < numBalasActual - 1)balas[i] = balas[numBalasActual - 1];if (numBalasActual > 0)numBalasActual--; }
            }
            int jX = jugador->getX(), jY = jugador->getY(), jAncho = jugador->getAnchoRepresentacion();
            for (int i = 0;i < numEnemigosActual;++i) {
                if (!enemigos[i])continue;int eX = enemigos[i]->getX(), eY = enemigos[i]->getY(), eAncho = enemigos[i]->getAnchoRepresentacion();
                if (jX<eX + eAncho && jX + jAncho>eX && jY == eY) { jugador->decrementarVida();enemigos[i]->borrar();enemigos[i]->setPos(rand() % (anchoAreaJuego - eAncho - 2) + 1, rand() % (altoAreaJuego - 2) + 1);if (!jugador->estaVivo()) { juegoTerminado = true;victoria = false;nivelEnCurso = false;return; }break; }
            }
            for (int i = 0;i < numRecursosActual;++i)if (recursos[i] && !recursos[i]->fueRecolectado()) { int rX = recursos[i]->getX(), rY = recursos[i]->getY(), rAncho = recursos[i]->getAnchoRepresentacion();if (jX<rX + rAncho && jX + jAncho>rX && jY == rY) { if (jugador && jugador->puedeAgregarRecurso()) { jugador->agregarRecurso(recursos[i]->getTipo());recursos[i]->recolectar();mensajeHUDInferior = (idiomaActual == Idioma::ESPANOL ? "Recurso Obtenido!" : "Resource Obtained!"); } else mensajeHUDInferior = (idiomaActual == Idioma::ESPANOL ? "Inventario Lleno!" : "Inventory Full!"); } }
            for (int i = 0;i < numRecursosActual;++i)if (recursos[i] && !recursos[i]->fueRecolectado())recursos[i]->actualizarParpadeo();
            for (int i = numAliadosActual - 1;i >= 0;--i)if (aliados[i] && !aliados[i]->fueUsado() && jugador && jugador->getX() < aliados[i]->getX() + aliados[i]->getAnchoRepresentacion() && jugador->getX() + jugador->getAnchoRepresentacion() > aliados[i]->getX() && jugador->getY() == aliados[i]->getY()) { int dur = aliados[i]->activar(jugador);TipoAliado tAl = aliados[i]->getTipo();if (tAl == TipoAliado::CONGELADOR_TIEMPO) { ticksCongelacionTiempoRestantes = dur;mensajeHUDInferior = (idiomaActual == Idioma::ESPANOL ? "Tiempo Congelado!" : "Time Frozen!"); } else if (tAl == TipoAliado::CONGELADOR_ENEMIGOS) { ticksCongelacionEnemigosRestantes = dur;mensajeHUDInferior = (idiomaActual == Idioma::ESPANOL ? "Enemigos Congelados!" : "Enemies Frozen!"); } else if (tAl == TipoAliado::SANADOR)mensajeHUDInferior = (idiomaActual == Idioma::ESPANOL ? "Vida recuperada!" : "Life recovered!"); }
        }
    }

    void renderizarJuego() {
        dibujarHUD();
        if (mundoActual == MUNDO_1 || mundoActual == MUNDO_2) {
            if (jugador)jugador->dibujar();
            for (int i = 0;i < numEnemigosActual;++i)if (enemigos[i])enemigos[i]->dibujar();
            for (int i = 0;i < numRecursosActual;++i)if (recursos[i])recursos[i]->dibujar();
            for (int i = 0;i < numAliadosActual;++i)if (aliados[i] && !aliados[i]->fueUsado())aliados[i]->dibujar();
            for (int i = 0;i < numBalasActual;++i)if (balas[i] && balas[i]->estaActiva())balas[i]->dibujar();
        }
        else if (mundoActual == MUNDO_3_CONSTRUCCION) {
            for (int i = 0;i < numZonasConstruccion;++i)if (zonasConstruccion[i])zonasConstruccion[i]->dibujar();
            if (jugador)jugador->dibujar(); if (mostrandoMenuConstruccion)dibujarMenuConstruccion();
        }
        for (int i = 0;i < numPortalesActivos;++i)if (portalesActivos[i])portalesActivos[i]->dibujar();
    }

    void dibujarMenuConstruccion() {
        int mAncho = 65, nItems = RECETAS_MUNDO3.size(), mAlto = nItems + 5; int mX = (anchoAreaJuego - mAncho) / 2;if (mX < 1)mX = 1; int mY = (altoAreaJuego - mAlto) / 2;if (mY < 1)mY = 1;
        Console::BackgroundColor = ConsoleColor::DarkBlue;Console::ForegroundColor = ConsoleColor::White;
        for (int y_m = 0;y_m < mAlto;++y_m) { movexy(mX, mY + y_m);for (int x_m = 0;x_m < mAncho;++x_m)std::cout << " "; }
        std::string titMenuStr;
        if (zonaSeleccionadaParaConstruir != -1 && zonaSeleccionadaParaConstruir < numZonasConstruccion && zonasConstruccion[zonaSeleccionadaParaConstruir])
            titMenuStr = (idiomaActual == Idioma::ESPANOL ? "Construir Zona (" : "Build Zone (") + std::to_string(zonasConstruccion[zonaSeleccionadaParaConstruir]->x) + "," + std::to_string(zonasConstruccion[zonaSeleccionadaParaConstruir]->y) + "):";
        else titMenuStr = (idiomaActual == Idioma::ESPANOL ? "MENU CONSTRUCCION" : "CONSTRUCTION MENU");
        movexy(mX + 2, mY + 1);std::cout << titMenuStr.substr(0, mAncho - 4); movexy(mX + 2, mY + 2);std::cout << std::string(mAncho - 4, '-');
        for (int i = 0;i < (int)RECETAS_MUNDO3.size();++i) {
            movexy(mX + 2, mY + 4 + i); Console::BackgroundColor = (i == recetaSeleccionadaMenuConstruccion) ? ConsoleColor::Cyan : ConsoleColor::DarkBlue; Console::ForegroundColor = (i == recetaSeleccionadaMenuConstruccion) ? ConsoleColor::Black : ConsoleColor::White;
            std::string nombreReceta = (idiomaActual == Idioma::ESPANOL ? RECETAS_MUNDO3[i].nombreConstruccion_ES : RECETAS_MUNDO3[i].nombreConstruccion_EN);
            std::string textoOpcion = nombreReceta + (idiomaActual == Idioma::ESPANOL ? " (Req: " : " (Req: ") + std::to_string(RECETAS_MUNDO3[i].req1_cantidad) + " " + nombreTipoRecurso(RECETAS_MUNDO3[i].req1_tipo) + ", " + std::to_string(RECETAS_MUNDO3[i].req2_cantidad) + " " + nombreTipoRecurso(RECETAS_MUNDO3[i].req2_tipo) + ")";
            std::cout << textoOpcion.substr(0, mAncho - 3); for (int k_c = textoOpcion.length();k_c < mAncho - 3;++k_c)std::cout << " ";
        }
        Console::BackgroundColor = ConsoleColor::Black;Console::ForegroundColor = ConsoleColor::White;
    }

    void mostrarMenuPrincipal() {
        int opSel = 0; const int numOpciones = 5; // Aumentado a 5 por "Idioma"
        while (enMenuPrincipal && !salirDelPrograma) {
            Console::Clear();dibujarBordesMenu();dibujarTituloASCII_JUEGO();dibujarOpcionesMenu(opSel);
            char entrada = _getch();
            if (tolower(entrada) == 'w') { opSel = (opSel - 1 + numOpciones) % numOpciones;hacerSonido(600, 50); }
            else if (tolower(entrada) == 's') { opSel = (opSel + 1) % numOpciones;hacerSonido(600, 50); }
            else if (entrada == 13) { Console::Clear();ejecutarOpcionMenu(opSel);if (opSel != 3)hacerSonido(800, 100); } // No sonar si es cambiar idioma, ya que no cambia de pantalla
            else if (entrada == 27)salirDelPrograma = true;
        }
    }

    void dibujarBordesMenu() {
        Console::ForegroundColor = ConsoleColor::Cyan;int ancho = ANCHO_CONSOLA - 1, alto = ALTO_CONSOLA - 1;
        for (int i = 0;i <= ancho;++i) { movexy(i, 0);std::cout << "=";movexy(i, alto);std::cout << "="; }
        for (int i = 1;i < alto;++i) { movexy(0, i);std::cout << "|";movexy(ancho, i);std::cout << "|"; }
        movexy(0, 0);std::cout << "+";movexy(ancho, 0);std::cout << "+";movexy(0, alto);std::cout << "+";movexy(ancho, alto);std::cout << "+";
        Console::ForegroundColor = ConsoleColor::White;
    }

    void dibujarTituloASCII_JUEGO() {
        Console::ForegroundColor = ConsoleColor::Yellow;int inicioY = 2; // Un poco mas arriba
        int titAncho = (TITULO_ASCII_JUEGO.empty() ? 0 : TITULO_ASCII_JUEGO[0].length());
        int inicioX = (ANCHO_CONSOLA - titAncho) / 2;if (inicioX < 0)inicioX = 0;
        for (int i = 0;i < (int)TITULO_ASCII_JUEGO.size();++i) { movexy(inicioX, inicioY + i);std::cout << TITULO_ASCII_JUEGO[i]; }
        Console::ForegroundColor = ConsoleColor::White;
    }

    void dibujarOpcionesMenu(int seleccionada) {
        std::string optIniciar = (idiomaActual == Idioma::ESPANOL ? "Iniciar Juego" : "Start Game");
        std::string optInstrucc = (idiomaActual == Idioma::ESPANOL ? "Instrucciones" : "Instructions");
        std::string optCreditos = (idiomaActual == Idioma::ESPANOL ? "Creditos" : "Credits");
        std::string optIdioma = (idiomaActual == Idioma::ESPANOL ? "Idioma: Espanol" : "Language: English");
        std::string optSalir = (idiomaActual == Idioma::ESPANOL ? "Salir" : "Exit");
        const char* opciones[] = { optIniciar.c_str(), optInstrucc.c_str(), optCreditos.c_str(), optIdioma.c_str(), optSalir.c_str() };
        int inicioY = 10; // Ajustado para el titulo y mas opciones
        int inicioX = ANCHO_CONSOLA / 2 - 10; // Ajustar para centrar textos mas largos
        for (int i = 0;i < 5;++i) {
            movexy(inicioX - 2, inicioY + i * 2);std::cout << ((i == seleccionada) ? ">" : " ");
            movexy(inicioX, inicioY + i * 2);Console::ForegroundColor = (i == seleccionada) ? ConsoleColor::Green : ConsoleColor::Gray;std::cout << opciones[i];
        }
        Console::ForegroundColor = ConsoleColor::White;
    }

    void ejecutarOpcionMenu(int opcion) {
        switch (opcion) {
        case 0: // iniciar juego
            juegoTerminado = false;victoria = false;ticksRestantesJuego = TIEMPO_LIMITE_SEGUNDOS * TICKS_POR_SEGUNDO;
            if (jugador) { delete jugador;jugador = nullptr; }
            liberarEntidadesNoPersistentes();
            if (portalesActivos != nullptr) { for (int i = 0;i < numPortalesActivos;++i)if (portalesActivos[i])delete portalesActivos[i];delete[] portalesActivos;portalesActivos = nullptr;numPortalesActivos = 0;capacidadPortales = 0; }
            if (zonasConstruccion != nullptr) { for (int i = 0;i < numZonasConstruccion;++i)if (zonasConstruccion[i])delete zonasConstruccion[i];delete[] zonasConstruccion;zonasConstruccion = nullptr;numZonasConstruccion = 0; }
            cuadrantesConstruidos = 0;mundo3Inicializado = false;
            inicializarMundo(MUNDO_1);
            break;
        case 1: mostrarInstrucciones();break;
        case 2: mostrarCreditos();break;
        case 3: // cambiar idioma
            idiomaActual = (idiomaActual == Idioma::ESPANOL) ? Idioma::INGLES : Idioma::ESPANOL;
            // no salimos del menu, se redibujara con el nuevo idioma
            break;
        case 4: salirDelPrograma = true;break;
        }
    }

    void mostrarPantallaTexto(const std::string& titulo, const std::vector<std::string>& texto) {
        dibujarBordesMenu(); int titLen = titulo.length();
        int iniXTit = (ANCHO_CONSOLA / 2) - (titLen / 2);if (iniXTit < 1)iniXTit = 1;
        movexy(iniXTit, 3);Console::ForegroundColor = ConsoleColor::Yellow;std::cout << titulo;
        int iniY = 6;Console::ForegroundColor = ConsoleColor::White;
        if (!texto.empty()) { for (int i = 0;i < (int)texto.size();++i)if (iniY + i < ALTO_CONSOLA - 3) { movexy(4, iniY + i);std::cout << texto[i]; } }
        std::string msgVolver = (idiomaActual == Idioma::ESPANOL ? "Presiona Enter para volver al menu..." : "Press Enter to return to menu...");
        movexy(4, ALTO_CONSOLA - 3);std::cout << msgVolver;
        while (_getch() != 13);
    }

    void mostrarInstrucciones() {
        std::string tit = (idiomaActual == Idioma::ESPANOL ? "INSTRUCCIONES" : "INSTRUCTIONS");
        std::vector<std::string> texto;
        if (idiomaActual == Idioma::ESPANOL) {
            texto = { "Bienvenido Vigilante del Equilibrio. Muevete con W,A,S,D.", "Recolecta recursos en Mundos 1 y 2 para construir el Mundo 3.", "Usa 'C' sobre zonas vacias en Mundo 3 para construir.", "Esquiva enemigos y sus disparos ('o').", "Aliados: Sanador (+H+), Congela-Tiempo (&T&), Congela-Enemigos (*E*).", "Pierdes si te quedas sin vidas o si se acaba el tiempo en M1/M2.", "Completa el " + std::to_string(OBJETIVO_CONSTRUCCION_PORCENTAJE) + "% del Mundo 3 para ganar.", "Presiona ESC durante el juego para volver al menu." };
        }
        else {
            texto = { "Welcome Guardian of Balance. Move with W,A,S,D.", "Collect resources in Worlds 1 & 2 to build World 3.", "Use 'C' on empty zones in World 3 to build.", "Dodge enemies and their projectiles ('o').", "Allies: Healer (+H+), Time-Freeze (&T&), Enemy-Freeze (*E*).", "You lose if you run out of lives or time in W1/W2.", "Complete " + std::to_string(OBJETIVO_CONSTRUCCION_PORCENTAJE) + "% of World 3 to win.", "Press ESC during game to return to menu." };
        }
        mostrarPantallaTexto(tit, texto);
    }
    void mostrarCreditos() {
        std::string tit = (idiomaActual == Idioma::ESPANOL ? "CREDITOS" : "CREDITS");
        std::vector<std::string> texto;
        if (idiomaActual == Idioma::ESPANOL) {
            texto = { "Juego desarrollado por:","- github.com/artuvall","","Curso: Algoritmos","UPC","2025-1" };
        }
        else {
            texto = { "Game developed by:","- github.com/artuvall","","Course: Algorithms","UPC","2025-1" };
        }
        mostrarPantallaTexto(tit, texto);
    }

    void dibujarBordesJuego() {
        char bv = '|', bh = '-';ConsoleColor cb = ConsoleColor::DarkGray;
        if (mundoActual == MUNDO_1) { bv = 179;bh = 196;cb = ConsoleColor::DarkGreen; }
        else if (mundoActual == MUNDO_2) { bv = 186;bh = 205;cb = ConsoleColor::DarkCyan; }
        else if (mundoActual == MUNDO_3_CONSTRUCCION) { bv = '#';bh = '#';cb = ConsoleColor::Gray; }
        Console::ForegroundColor = cb;
        for (int i = 0;i < anchoAreaJuego;++i) { movexy(i, 0);std::cout << bh;movexy(i, altoAreaJuego - 1);std::cout << bh; }
        for (int i = 1;i < altoAreaJuego - 1;++i) { movexy(0, i);std::cout << bv;movexy(anchoAreaJuego - 1, i);std::cout << bv; }
        movexy(0, 0);std::cout << "+";movexy(anchoAreaJuego - 1, 0);std::cout << "+";movexy(0, altoAreaJuego - 1);std::cout << "+";movexy(anchoAreaJuego - 1, altoAreaJuego - 1);std::cout << "+";
        Console::ForegroundColor = ConsoleColor::White;
    }

    void dibujarBordeHUD() {
        Console::ForegroundColor = ConsoleColor::DarkCyan;
        int x1 = DESPLAZAMIENTO_X_HUD - 1, y1 = POSICION_Y_HUD - 1;int x2 = ANCHO_CONSOLA - 1, y2 = ALTO_CONSOLA - 1;
        for (int x = x1;x <= x2;++x) { movexy(x, y1);std::cout << "-";movexy(x, y2);std::cout << "-"; }
        for (int y = y1 + 1;y < y2;++y) { movexy(x1, y);std::cout << "|";movexy(x2, y);std::cout << "|"; }
        movexy(x1, y1);std::cout << "+";movexy(x2, y1);std::cout << "+";movexy(x1, y2);std::cout << "+";movexy(x2, y2);std::cout << "+";
        Console::ForegroundColor = ConsoleColor::White;
    }

    void imprimirConAjusteYLimpieza(int& lineaYActual, const std::string& texto, int anchoMax) {
        if (lineaYActual >= ALTO_CONSOLA - 1)return; size_t pos = 0;
        while (pos < texto.length()) {
            if (lineaYActual >= ALTO_CONSOLA - 1)break; std::string sub = texto.substr(pos, anchoMax);
            movexy(DESPLAZAMIENTO_X_HUD, lineaYActual);std::cout << sub;
            for (int k = sub.length();k < anchoMax;++k)std::cout << " ";
            pos += anchoMax;lineaYActual++;
        }
    }

    void dibujarHUD() {
        Console::ForegroundColor = ConsoleColor::Yellow; int lineaYActualHUD = POSICION_Y_HUD;
        int hudAnchoInt = ANCHO_CONSOLA - DESPLAZAMIENTO_X_HUD - 1;
        std::string strEstado = (idiomaActual == Idioma::ESPANOL ? "--- ESTADO ---" : "--- STATUS ---");
        std::string strMundo = (idiomaActual == Idioma::ESPANOL ? "Mundo: " : "World: ");
        std::string strVidas = (idiomaActual == Idioma::ESPANOL ? "Vidas: " : "Lives: ");
        std::string strInventario = (idiomaActual == Idioma::ESPANOL ? "Inventario: " : "Inventory: ");
        std::string strConstruido = (idiomaActual == Idioma::ESPANOL ? "Construido: " : "Built: ");
        std::string strTiempo = (idiomaActual == Idioma::ESPANOL ? "Tiempo: " : "Time: ");
        std::string strCongelado = (idiomaActual == Idioma::ESPANOL ? "CONGELADO" : "FROZEN");
        std::string strInvTitulo = (idiomaActual == Idioma::ESPANOL ? "-- Inventario --" : "-- Inventory --");
        std::string strVacio = (idiomaActual == Idioma::ESPANOL ? " (Vacio)" : " (Empty)");

        imprimirConAjusteYLimpieza(lineaYActualHUD, strEstado, hudAnchoInt);
        imprimirConAjusteYLimpieza(lineaYActualHUD, strMundo + std::to_string(mundoActual), hudAnchoInt);
        if (jugador) {
            imprimirConAjusteYLimpieza(lineaYActualHUD, strVidas + std::to_string(jugador->getVidas()), hudAnchoInt);
            imprimirConAjusteYLimpieza(lineaYActualHUD, strInventario + std::to_string(jugador->getTotalRecursosInventario()) + "/" + std::to_string(CAPACIDAD_INVENTARIO), hudAnchoInt);
            if (mundoActual == MUNDO_3_CONSTRUCCION) { int porc = (numZonasConstruccion > 0) ? (cuadrantesConstruidos * 100 / numZonasConstruccion) : 0;imprimirConAjusteYLimpieza(lineaYActualHUD, strConstruido + std::to_string(porc) + "%", hudAnchoInt); }
        }
        else {
            imprimirConAjusteYLimpieza(lineaYActualHUD, strVidas + "---", hudAnchoInt);imprimirConAjusteYLimpieza(lineaYActualHUD, strInventario + "---/" + std::to_string(CAPACIDAD_INVENTARIO), hudAnchoInt);
            if (mundoActual == MUNDO_3_CONSTRUCCION)imprimirConAjusteYLimpieza(lineaYActualHUD, strConstruido + "0%", hudAnchoInt);
        }
        int segsRest = (ticksRestantesJuego >= 0) ? (ticksRestantesJuego / TICKS_POR_SEGUNDO) : 0;
        if (mundoActual != MUNDO_3_CONSTRUCCION) { if (ticksCongelacionTiempoRestantes > 0)imprimirConAjusteYLimpieza(lineaYActualHUD, strTiempo + strCongelado, hudAnchoInt);else imprimirConAjusteYLimpieza(lineaYActualHUD, strTiempo + std::to_string(segsRest) + "s", hudAnchoInt); }
        else imprimirConAjusteYLimpieza(lineaYActualHUD, strTiempo + "N/A ", hudAnchoInt);
        lineaYActualHUD++;
        imprimirConAjusteYLimpieza(lineaYActualHUD, strInvTitulo, hudAnchoInt);
        if (jugador && jugador->getTotalRecursosInventario() > 0) {
            bool hayRecMostrados = false;
            for (int i = 0;i < NUM_TIPOS_RECURSOS;++i) {
                TipoRecurso tipoActEnum;
                if (i == 0)tipoActEnum = TipoRecurso::EMPATIA;else if (i == 1)tipoActEnum = TipoRecurso::ETICA;else if (i == 2)tipoActEnum = TipoRecurso::ADAPTABILIDAD;
                else if (i == 3)tipoActEnum = TipoRecurso::NUCLEO_IA;else if (i == 4)tipoActEnum = TipoRecurso::PAQUETE_DATOS;else if (i == 5)tipoActEnum = TipoRecurso::CELULA_ENERGIA;else continue;
                int cant = jugador->getCantidadRecurso(tipoActEnum);
                if (cant > 0) { if (lineaYActualHUD >= ALTO_CONSOLA - 5)break;imprimirConAjusteYLimpieza(lineaYActualHUD, nombreTipoRecurso(tipoActEnum) + ": " + std::to_string(cant), hudAnchoInt);hayRecMostrados = true; }
            }
            if (!hayRecMostrados && lineaYActualHUD < ALTO_CONSOLA - 5)imprimirConAjusteYLimpieza(lineaYActualHUD, strVacio, hudAnchoInt);
        }
        else if (lineaYActualHUD < ALTO_CONSOLA - 5)imprimirConAjusteYLimpieza(lineaYActualHUD, strVacio, hudAnchoInt);
        int yParaFeedback; if (mundoActual == MUNDO_3_CONSTRUCCION && !mostrandoMenuConstruccion)yParaFeedback = ALTO_CONSOLA - 5;else yParaFeedback = ALTO_CONSOLA - 4;
        int yIniLimpFeed = lineaYActualHUD + 1;if (yIniLimpFeed > yParaFeedback)yIniLimpFeed = yParaFeedback;
        for (int y_cl = yIniLimpFeed;y_cl <= yParaFeedback;++y_cl)if (y_cl >= POSICION_Y_HUD && y_cl < ALTO_CONSOLA - 1) { movexy(DESPLAZAMIENTO_X_HUD, y_cl);std::cout << std::string(hudAnchoInt, ' '); }
        if (!mensajeHUDInferior.empty()) { int tempYFeed = yParaFeedback;if (mensajeHUDInferior.length() > (size_t)hudAnchoInt && yParaFeedback > yIniLimpFeed)tempYFeed = yParaFeedback - 1;if (tempYFeed < yIniLimpFeed)tempYFeed = yIniLimpFeed;imprimirConAjusteYLimpieza(tempYFeed, mensajeHUDInferior, hudAnchoInt); }
        int posYCMsg = ALTO_CONSOLA - 4;
        if (posYCMsg >= POSICION_Y_HUD + 1) { movexy(DESPLAZAMIENTO_X_HUD, posYCMsg);std::cout << std::string(hudAnchoInt, ' ');if (mundoActual == MUNDO_3_CONSTRUCCION && !mostrandoMenuConstruccion) { std::string txtConst = (idiomaActual == Idioma::ESPANOL ? "C: Construir" : "C: Build");movexy(DESPLAZAMIENTO_X_HUD, posYCMsg);std::cout << txtConst.substr(0, hudAnchoInt); } }
        int posYEscMenu = ALTO_CONSOLA - 3;
        if (posYEscMenu >= POSICION_Y_HUD + 1) { movexy(DESPLAZAMIENTO_X_HUD, posYEscMenu);std::cout << std::string(hudAnchoInt, ' ');std::string txtMenu = (idiomaActual == Idioma::ESPANOL ? "ESC: Menu" : "ESC: Menu");movexy(DESPLAZAMIENTO_X_HUD, posYEscMenu);std::cout << txtMenu.substr(0, hudAnchoInt); }
        Console::ForegroundColor = ConsoleColor::White;
    }

    void mostrarPantallaFinJuego() {
        Console::Clear(); std::string msgP;
        if (victoria) { msgP = (idiomaActual == Idioma::ESPANOL ? "¡¡¡ GANASTE !!!" : "!!! YOU WON !!!");Console::ForegroundColor = ConsoleColor::Green; }
        else { msgP = (idiomaActual == Idioma::ESPANOL ? "--- PERDISTE ---" : "--- YOU LOST ---");Console::ForegroundColor = ConsoleColor::Red; }
        int posYP = (ALTO_CONSOLA / 2) - 6;int posXMsgP = (ANCHO_CONSOLA - msgP.length()) / 2;if (posXMsgP < 0)posXMsgP = 0;
        movexy(posXMsgP, posYP);std::cout << msgP;
        std::string msgV = (idiomaActual == Idioma::ESPANOL ? "Presiona Enter para volver al menu..." : "Press Enter to return to menu...");int posXMsgV = (ANCHO_CONSOLA - msgV.length()) / 2;if (posXMsgV < 0)posXMsgV = 0;
        movexy(posXMsgV, posYP + 2);Console::ForegroundColor = ConsoleColor::White;std::cout << msgV;
        while (_getch() != 13); enMenuPrincipal = true;nivelEnCurso = false;
    }

public:
    Juego() { inicializarVariablesGlobales(); }
    ~Juego() {
        liberarEntidadesNoPersistentes();
        if (jugador) { delete jugador;jugador = nullptr; }
        if (portalesActivos != nullptr) { for (int i = 0;i < numPortalesActivos;++i)if (portalesActivos[i])delete portalesActivos[i];delete[] portalesActivos;portalesActivos = nullptr; }
        if (zonasConstruccion != nullptr) { for (int i = 0;i < numZonasConstruccion;++i)if (zonasConstruccion[i])delete zonasConstruccion[i];delete[] zonasConstruccion;zonasConstruccion = nullptr; }
    }
    void cicloPrincipal() {
        while (!salirDelPrograma) {
            if (enMenuPrincipal)mostrarMenuPrincipal();
            else if (nivelEnCurso) {
                procesarEntradaJuego();
                if (nivelEnCurso && !mostrandoMenuConstruccion)actualizarEstadoJuego();
                if (nivelEnCurso)renderizarJuego();
                Sleep(100); if (juegoTerminado)mostrarPantallaFinJuego();
            }
            else salirDelPrograma = true;
        }
        Console::Clear();movexy(10, 10);std::cout << (idiomaActual == Idioma::ESPANOL ? "Saliendo del programa..." : "Exiting program...") << std::endl;Sleep(1000);
    }
};
