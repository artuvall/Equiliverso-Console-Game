#include "pch.h"
#include <iostream>
#include <vector>
#include <string>
#include "Juego.h"

int main()
{

    Juego* miJuego = new Juego();

    miJuego->cicloPrincipal();

    delete miJuego;
    miJuego = nullptr;

    return 0;
}