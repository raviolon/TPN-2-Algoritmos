#include <cstdio>
#include <cstring>
#include <iostream>

using namespace std;

const int CANT_CATEGORIAS = 20;
const int MAX_CARACTERES = 50;

struct Componente {
    int id;
    char tipo[MAX_CARACTERES + 1];
    int codigoCategoria;
    char modelo[MAX_CARACTERES + 1];
    char fabricante[MAX_CARACTERES + 1];
    int consumoWatts;
};

struct NodoComponente {
    Componente componente;
    NodoComponente* siguiente;
};

typedef NodoComponente* VectorCategorias[CANT_CATEGORIAS];

enum ResultadoInsercion {
    INSERCION_EXITOSA,
    CATEGORIA_INVALIDA,
    CADENA_INVALIDA,
    ID_REPETIDO
};

NodoComponente* insertarOrdenado(NodoComponente*& lista, const Componente& componente);
bool cadenaValida(const char cadena[]);
bool existeId(const VectorCategorias& vectorCategorias, int id);
ResultadoInsercion insertarComponente(VectorCategorias& vectorCategorias, const Componente& componente);
void liberarLista(NodoComponente*& lista);
void liberarCategorias(VectorCategorias& vectorCategorias);
void cargarYProcesarDatos(VectorCategorias& vectorCategorias);
void mostrarResultados(const VectorCategorias& vectorCategorias);

bool cadenaValida(const char cadena[]) {
    for (int i = 0; i <= MAX_CARACTERES; ++i) {
        if (cadena[i] == '\0') {
            return true;
        }
    }

    return false;
}

bool existeId(const VectorCategorias& vectorCategorias, int id) {
    for (int categoria = 0; categoria < CANT_CATEGORIAS; ++categoria) {
        NodoComponente* aux = vectorCategorias[categoria];

        while (aux != nullptr) {
            if (aux->componente.id == id) {
                return true;
            }

            aux = aux->siguiente;
        }
    }

    return false;
}

ResultadoInsercion insertarComponente(VectorCategorias& vectorCategorias, const Componente& componente) {
    if (componente.codigoCategoria < 0 || componente.codigoCategoria >= CANT_CATEGORIAS) {
        return CATEGORIA_INVALIDA;
    }

    if (!cadenaValida(componente.tipo) ||
        !cadenaValida(componente.modelo) ||
        !cadenaValida(componente.fabricante)) {
        return CADENA_INVALIDA;
    }

    if (existeId(vectorCategorias, componente.id)) {
        return ID_REPETIDO;
    }

    insertarOrdenado(vectorCategorias[componente.codigoCategoria], componente);
    return INSERCION_EXITOSA;
}

NodoComponente* insertarOrdenado(NodoComponente*& lista, const Componente& componente) {
    NodoComponente* anterior = nullptr;
    NodoComponente* aux = lista;

    while (aux != nullptr && aux->componente.id < componente.id) {
        anterior = aux;
        aux = aux->siguiente;
    }

    if (aux != nullptr && aux->componente.id == componente.id) {
        return nullptr;
    }

    NodoComponente* nuevo = new NodoComponente;
    nuevo->componente = componente;
    nuevo->siguiente = aux;

    if (anterior == nullptr) {
        lista = nuevo;
    } else {
        anterior->siguiente = nuevo;
    }

    return nuevo;
}

void liberarLista(NodoComponente*& lista) {
    while (lista != nullptr) {
        NodoComponente* siguiente = lista->siguiente;
        delete lista;
        lista = siguiente;
    }
}

void liberarCategorias(VectorCategorias& vectorCategorias) {
    for (int categoria = 0; categoria < CANT_CATEGORIAS; ++categoria) {
        liberarLista(vectorCategorias[categoria]);
    }
}

int main() {
    VectorCategorias vectorCategorias = {};
    int opcion = 0;

    do {
        cout << "\n--- Menu principal ---\n"
             << "1. Cargar y procesar datos de componentes\n"
             << "2. Mostrar resultados\n"
             << "3. Salir\n"
             << "Seleccione una opcion: ";

        if (!(cin >> opcion)) {
            if (cin.eof()) {
                break;
            }

            cout << "Entrada invalida. Ingrese un numero del 1 al 3.\n";
            cin.clear();
            char caracter;
            while (cin.get(caracter) && caracter != '\n') {
            }
            continue;
        }

        char caracter;
        while (cin.get(caracter) && caracter != '\n') {
        }

        switch (opcion) {
            case 1:
                cargarYProcesarDatos(vectorCategorias);
                break;
            case 2:
                mostrarResultados(vectorCategorias);
                break;
            case 3:
                cout << "Saliendo del programa.\n";
                break;
            default:
                cout << "Opcion invalida. Ingrese un numero del 1 al 3.\n";
        }
    } while (opcion != 3);

    liberarCategorias(vectorCategorias);
    return 0;
}

void cargarYProcesarDatos(VectorCategorias& vectorCategorias) {
    (void)vectorCategorias;
    cout << "La carga y el procesamiento de datos todavia no estan implementados.\n";
}

void mostrarResultados(const VectorCategorias& vectorCategorias) {
    (void)vectorCategorias;
    cout << "La presentacion de resultados todavia no esta implementada.\n";
}
