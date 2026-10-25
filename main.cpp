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

int contarFabricante(const VectorCategorias& vectorCategorias, const char fabricante[]) {
    int cantidad = 0;

    for (int categoria = 0; categoria < CANT_CATEGORIAS; ++categoria) {
        NodoComponente* aux = vectorCategorias[categoria];

        while (aux != nullptr) {
            if (strcmp(aux->componente.fabricante, fabricante) == 0) {
                ++cantidad;
            }
            aux = aux->siguiente;
        }
    }

    return cantidad;
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
    const char NOMBRE_ARCHIVO[] = "componentes.dat";
    FILE* archivo = fopen(NOMBRE_ARCHIVO, "rb");
    if (archivo == nullptr) {
        cout << "No se pudo abrir el archivo de componentes.\n";
        return;
    }

    Componente componente;

    while (fread(&componente, sizeof(Componente), 1, archivo) == 1) {
        switch (insertarComponente(vectorCategorias, componente)) {
            case INSERCION_EXITOSA:
                break;
            case CATEGORIA_INVALIDA:
                cout << "Componente " << componente.id << " rechazado: categoria invalida.\n";
                break;
            case CADENA_INVALIDA:
                cout << "Componente " << componente.id << " rechazado: cadena invalida.\n";
                break;
            case ID_REPETIDO:
                cout << "Componente " << componente.id << " rechazado: id repetido.\n";
                break;
        }
    }

    fclose(archivo);
    cout << "Carga finalizada.\n";
}

void mostrarResultados(const VectorCategorias& vectorCategorias) {
    int total = 0;
    int categoriaMayor = -1;
    int cantidadMayor = 0;
    NodoComponente* masConsumo = nullptr;
    NodoComponente* masFrecuente = nullptr;
    int vecesMasFrecuente = 0;

    cout << "\nComponentes y promedio de consumo por categoria:\n";

    for (int categoria = 0; categoria < CANT_CATEGORIAS; ++categoria) {
        int cantidad = 0;
        int sumaWatts = 0;
        NodoComponente* aux = vectorCategorias[categoria];

        while (aux != nullptr) {
            ++cantidad;
            sumaWatts += aux->componente.consumoWatts;

            if (masConsumo == nullptr ||
                aux->componente.consumoWatts > masConsumo->componente.consumoWatts) {
                masConsumo = aux;
            }

            int veces = contarFabricante(vectorCategorias, aux->componente.fabricante);
            if (veces > vecesMasFrecuente) {
                vecesMasFrecuente = veces;
                masFrecuente = aux;
            }

            aux = aux->siguiente;
        }

        total += cantidad;

        cout << "Categoria " << categoria << ": " << cantidad << " componentes";
        if (cantidad > 0) {
            cout << ", promedio " << (double)sumaWatts / cantidad << " W";
        }
        cout << "\n";

        if (cantidad > cantidadMayor) {
            cantidadMayor = cantidad;
            categoriaMayor = categoria;
        }
    }

    if (total == 0) {
        cout << "No hay componentes cargados.\n";
        return;
    }

    cout << "\nTotal de componentes: " << total << "\n";
    cout << "Categoria con mas componentes: " << categoriaMayor
         << " (" << cantidadMayor << ")\n";
    cout << "Fabricante mas frecuente: " << masFrecuente->componente.fabricante
         << " (" << vecesMasFrecuente << ")\n";
    cout << "Mayor consumo: modelo " << masConsumo->componente.modelo
         << ", ID " << masConsumo->componente.id
         << " (" << masConsumo->componente.consumoWatts << " W)\n";
}
