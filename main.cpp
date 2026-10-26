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

const int INSERCION_EXITOSA = 0;
const int CATEGORIA_INVALIDA = 1;
const int CADENA_INVALIDA = 2;
const int ID_REPETIDO = 3;

struct EstadisticasLista {
    int cantidad;
    double sumaWatts;
    bool hayMayorConsumo;
    Componente mayorConsumo;
};

struct EstadisticasFabricante {
    int frecuencia;
    bool hayFabricante;
    char fabricante[MAX_CARACTERES + 1];
};

NodoComponente* insertarOrdenado(NodoComponente*& lista, const Componente& componente);
NodoComponente* buscarComponente(NodoComponente* lista, int id);
bool cadenaValida(const char cadena[]);
bool existeId(
    NodoComponente* const (&vectorCategorias)[CANT_CATEGORIAS],
    int id
);
int insertarComponente(
    NodoComponente* (&vectorCategorias)[CANT_CATEGORIAS],
    const Componente& componente
);
EstadisticasLista obtenerEstadisticasLista(NodoComponente* lista);
int contarFabricante(NodoComponente* lista, const char fabricante[]);
void copiarCadena(char destino[], const char origen[]);
bool fabricanteAparecioAntes(
    NodoComponente* const (&vectorCategorias)[CANT_CATEGORIAS],
    int categoriaActual,
    NodoComponente* nodoActual,
    const char fabricante[]
);
EstadisticasFabricante fabricanteMasFrecuente(
    NodoComponente* const (&vectorCategorias)[CANT_CATEGORIAS]
);
void liberarLista(NodoComponente*& lista);
void liberarCategorias(
    NodoComponente* (&vectorCategorias)[CANT_CATEGORIAS]
);
bool cerrarArchivo(FILE*& archivo);
bool crearArchivoEjemplo();
void cargarYProcesarDatos(
    NodoComponente* (&vectorCategorias)[CANT_CATEGORIAS]
);
void mostrarResultados(
    NodoComponente* const (&vectorCategorias)[CANT_CATEGORIAS]
);

bool cadenaValida(const char cadena[]) {
    for (int i = 0; i <= MAX_CARACTERES; ++i) {
        if (cadena[i] == '\0') {
            return true;
        }
    }

    return false;
}

bool existeId(
    NodoComponente* const (&vectorCategorias)[CANT_CATEGORIAS],
    int id
) {
    for (int categoria = 0; categoria < CANT_CATEGORIAS; ++categoria) {
        if (buscarComponente(vectorCategorias[categoria], id) != NULL) {
            return true;
        }
    }

    return false;
}

int insertarComponente(
    NodoComponente* (&vectorCategorias)[CANT_CATEGORIAS],
    const Componente& componente
) {
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

NodoComponente* buscarComponente(NodoComponente* lista, int id) {
    NodoComponente* aux = lista;

    while (aux != NULL && aux->componente.id != id) {
        aux = aux->siguiente;
    }

    return aux;
}

NodoComponente* insertarOrdenado(NodoComponente*& lista, const Componente& componente) {
    NodoComponente* anterior = NULL;
    NodoComponente* aux = lista;

    while (aux != NULL && aux->componente.id <= componente.id) {
        anterior = aux;
        aux = aux->siguiente;
    }

    NodoComponente* nuevo = new NodoComponente;
    nuevo->componente = componente;
    nuevo->siguiente = aux;

    if (anterior == NULL) {
        lista = nuevo;
    } else {
        anterior->siguiente = nuevo;
    }

    return nuevo;
}

void liberarLista(NodoComponente*& lista) {
    NodoComponente* aux;

    while (lista != NULL) {
        aux = lista;
        lista = lista->siguiente;
        delete aux;
    }
}

void liberarCategorias(
    NodoComponente* (&vectorCategorias)[CANT_CATEGORIAS]
) {
    for (int categoria = 0; categoria < CANT_CATEGORIAS; ++categoria) {
        liberarLista(vectorCategorias[categoria]);
    }
}

bool cerrarArchivo(FILE*& archivo) {
    int resultado = fclose(archivo);
    archivo = NULL;

    if (resultado != 0) {
        cout << "Error al cerrar el archivo.\n";
        return false;
    }

    return true;
}

EstadisticasLista obtenerEstadisticasLista(NodoComponente* lista) {
    EstadisticasLista estadisticas = {0, 0, false, {0, "", 0, "", "", 0}};
    NodoComponente* aux = lista;

    while (aux != NULL) {
        ++estadisticas.cantidad;
        estadisticas.sumaWatts += aux->componente.consumoWatts;

        if (!estadisticas.hayMayorConsumo ||
            aux->componente.consumoWatts >
                estadisticas.mayorConsumo.consumoWatts) {
            estadisticas.mayorConsumo = aux->componente;
            estadisticas.hayMayorConsumo = true;
        }

        aux = aux->siguiente;
    }

    return estadisticas;
}

int contarFabricante(NodoComponente* lista, const char fabricante[]) {
    int cantidad = 0;
    NodoComponente* aux = lista;

    while (aux != NULL) {
        if (strcmp(aux->componente.fabricante, fabricante) == 0) {
            ++cantidad;
        }

        aux = aux->siguiente;
    }

    return cantidad;
}

void copiarCadena(char destino[], const char origen[]) {
    int posicion = 0;

    while (posicion < MAX_CARACTERES && origen[posicion] != '\0') {
        destino[posicion] = origen[posicion];
        ++posicion;
    }

    destino[posicion] = '\0';
}

bool fabricanteAparecioAntes(
    NodoComponente* const (&vectorCategorias)[CANT_CATEGORIAS],
    int categoriaActual,
    NodoComponente* nodoActual,
    const char fabricante[]
) {
    for (int categoria = 0; categoria <= categoriaActual; ++categoria) {
        NodoComponente* aux = vectorCategorias[categoria];

        while (aux != NULL && aux != nodoActual) {
            if (strcmp(aux->componente.fabricante, fabricante) == 0) {
                return true;
            }

            aux = aux->siguiente;
        }

        if (categoria == categoriaActual) {
            break;
        }
    }

    return false;
}

EstadisticasFabricante fabricanteMasFrecuente(
    NodoComponente* const (&vectorCategorias)[CANT_CATEGORIAS]
) {
    EstadisticasFabricante estadisticas;
    estadisticas.frecuencia = 0;
    estadisticas.hayFabricante = false;
    estadisticas.fabricante[0] = '\0';

    for (int categoria = 0; categoria < CANT_CATEGORIAS; ++categoria) {
        NodoComponente* aux = vectorCategorias[categoria];

        while (aux != NULL) {
            if (fabricanteAparecioAntes(
                    vectorCategorias,
                    categoria,
                    aux,
                    aux->componente.fabricante
                )) {
                aux = aux->siguiente;
                continue;
            }

            int cantidad = 0;

            for (int categoriaConteo = 0;
                 categoriaConteo < CANT_CATEGORIAS;
                 ++categoriaConteo) {
                cantidad += contarFabricante(
                    vectorCategorias[categoriaConteo],
                    aux->componente.fabricante
                );
            }

            if (cantidad > estadisticas.frecuencia) {
                estadisticas.frecuencia = cantidad;
                copiarCadena(estadisticas.fabricante, aux->componente.fabricante);
                estadisticas.hayFabricante = true;
            }

            aux = aux->siguiente;
        }
    }

    return estadisticas;
}

int main() {
    NodoComponente* vectorCategorias[CANT_CATEGORIAS];
    for (int categoria = 0; categoria < CANT_CATEGORIAS; ++categoria) {
        vectorCategorias[categoria] = NULL;
    }

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

bool crearArchivoEjemplo() {
    const char NOMBRE_ARCHIVO[] = "componentes.dat";
    Componente datos[] = {
        {101, "GPU", 0, "RTX 4070", "NVIDIA", 200},
        {102, "CPU", 0, "Ryzen 7", "AMD", 105},
        {103, "RAM", 1, "Fury", "Kingston", 5},
        {104, "GPU", 0, "RTX 4060", "NVIDIA", 115},
        {105, "GPU", 2, "RX 7800", "AMD", 150},
        {106, "SSD", 2, "NV2", "Kingston", 80}
    };
    const int CANT_DATOS = sizeof(datos) / sizeof(datos[0]);

    FILE* archivo = fopen(NOMBRE_ARCHIVO, "wb");
    if (archivo == NULL) {
        cout << "No se pudo crear el archivo de ejemplo.\n";
        return false;
    }

    size_t escritos = fwrite(datos, sizeof(Componente), CANT_DATOS, archivo);
    bool cierreCorrecto = cerrarArchivo(archivo);

    if (escritos != CANT_DATOS || !cierreCorrecto) {
        cout << "No se pudieron guardar todos los datos de ejemplo.\n";
        return false;
    }

    cout << "Se creo componentes.dat con " << CANT_DATOS
         << " registros de demostracion.\n";
    return true;
}

void cargarYProcesarDatos(
    NodoComponente* (&vectorCategorias)[CANT_CATEGORIAS]
) {
    const char NOMBRE_ARCHIVO[] = "componentes.dat";
    FILE* archivo = fopen(NOMBRE_ARCHIVO, "rb");

    if (archivo == NULL) {
        char respuesta;
        cout << "No se pudo abrir componentes.dat. "
             << "Crear o reemplazarlo con datos de demostracion? (s/n): ";

        if (!(cin >> respuesta)) {
            cout << "No se pudo leer la respuesta.\n";
            return;
        }

        char caracter;
        while (cin.get(caracter) && caracter != '\n') {
        }

        if (respuesta != 's' && respuesta != 'S') {
            cout << "Copia componentes.dat a la carpeta del programa y vuelve a intentar.\n";
            return;
        }

        if (!crearArchivoEjemplo()) {
            return;
        }

        archivo = fopen(NOMBRE_ARCHIVO, "rb");
        if (archivo == NULL) {
            cout << "No se pudo abrir el archivo de ejemplo para leerlo.\n";
            return;
        }
    }

    if (fseek(archivo, 0, SEEK_END) != 0) {
        cout << "No se pudo determinar el tamano del archivo.\n";
        cerrarArchivo(archivo);
        return;
    }

    long tamanoArchivo = ftell(archivo);
    if (tamanoArchivo < 0 || fseek(archivo, 0, SEEK_SET) != 0) {
        cout << "No se pudo posicionar el archivo para su lectura.\n";
        cerrarArchivo(archivo);
        return;
    }

    if (tamanoArchivo % (long)sizeof(Componente) != 0) {
        cout << "El archivo tiene un tamano incompatible o un registro incompleto.\n";
        cerrarArchivo(archivo);
        return;
    }

    NodoComponente* categoriasLeidas[CANT_CATEGORIAS];
    for (int categoria = 0; categoria < CANT_CATEGORIAS; ++categoria) {
        categoriasLeidas[categoria] = NULL;
    }

    int registrosLeidos = 0;
    int registrosAceptados = 0;
    int registrosRechazados = 0;
    bool errorLectura = false;
    Componente componente;

    while (true) {
        size_t cantidadLeida = fread(&componente, sizeof(Componente), 1, archivo);

        if (cantidadLeida == 1) {
            ++registrosLeidos;

            switch (insertarComponente(categoriasLeidas, componente)) {
                case INSERCION_EXITOSA:
                    ++registrosAceptados;
                    break;
                case CATEGORIA_INVALIDA:
                    cout << "Componente " << componente.id
                         << " rechazado: categoria invalida.\n";
                    ++registrosRechazados;
                    break;
                case CADENA_INVALIDA:
                    cout << "Componente " << componente.id
                         << " rechazado: cadena invalida.\n";
                    ++registrosRechazados;
                    break;
                case ID_REPETIDO:
                    cout << "Componente " << componente.id
                         << " rechazado: ID repetido.\n";
                    ++registrosRechazados;
                    break;
            }
        } else if (ferror(archivo)) {
            cout << "Error durante la lectura del archivo.\n";
            errorLectura = true;
            break;
        } else if (feof(archivo)) {
            break;
        } else {
            cout << "La lectura termino sin alcanzar el fin del archivo.\n";
            errorLectura = true;
            break;
        }
    }

    bool cierreCorrecto = cerrarArchivo(archivo);
    if (errorLectura || !cierreCorrecto) {
        liberarCategorias(categoriasLeidas);
        return;
    }

    liberarCategorias(vectorCategorias);
    for (int categoria = 0; categoria < CANT_CATEGORIAS; ++categoria) {
        vectorCategorias[categoria] = categoriasLeidas[categoria];
        categoriasLeidas[categoria] = NULL;
    }

    cout << "Carga finalizada. Registros leidos: " << registrosLeidos
         << ", aceptados: " << registrosAceptados
         << ", rechazados: " << registrosRechazados << ".\n";
}

void mostrarResultados(
    NodoComponente* const (&vectorCategorias)[CANT_CATEGORIAS]
) {
    int total = 0;
    int categoriaMayor = -1;
    int cantidadMayor = 0;
    bool hayMasConsumo = false;
    Componente masConsumo = {0, "", 0, "", "", 0};
    EstadisticasFabricante fabricanteFrecuente =
        fabricanteMasFrecuente(vectorCategorias);

    cout << "\nComponentes y promedio de consumo por categoria:\n";

    for (int categoria = 0; categoria < CANT_CATEGORIAS; ++categoria) {
        EstadisticasLista estadisticas =
            obtenerEstadisticasLista(vectorCategorias[categoria]);
        total += estadisticas.cantidad;

        cout << "Categoria " << categoria << ": "
             << estadisticas.cantidad << " componente(s)";
        if (estadisticas.cantidad > 0) {
            cout << ", promedio "
                 << estadisticas.sumaWatts / estadisticas.cantidad
                 << " W";
        }
        cout << "\n";

        if (estadisticas.cantidad > cantidadMayor) {
            cantidadMayor = estadisticas.cantidad;
            categoriaMayor = categoria;
        }

        if (estadisticas.hayMayorConsumo &&
            (!hayMasConsumo ||
             estadisticas.mayorConsumo.consumoWatts > masConsumo.consumoWatts)) {
            masConsumo = estadisticas.mayorConsumo;
            hayMasConsumo = true;
        }
    }

    if (total == 0) {
        cout << "No hay componentes cargados.\n";
        return;
    }

    cout << "\nTotal de componentes: " << total << "\n";
    cout << "Categoria con mas componentes: " << categoriaMayor
         << " (" << cantidadMayor << ")\n";
    if (fabricanteFrecuente.hayFabricante) {
        cout << "Fabricante mas frecuente: " << fabricanteFrecuente.fabricante
             << " (" << fabricanteFrecuente.frecuencia << ")\n";
    }

    if (hayMasConsumo) {
        cout << "Mayor consumo: modelo " << masConsumo.modelo
             << ", ID " << masConsumo.id
             << " (" << masConsumo.consumoWatts << " W)\n";
    }
}
