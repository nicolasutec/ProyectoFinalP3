#include "Arbol.h"

void cargarPeliculas(const string& rutaArchivo, ArbolBinarioBusqueda& arbol) {
    ifstream archivo(rutaArchivo);
    if (!archivo.is_open()) {
        cerr << "Error al abrir el archivo: " << rutaArchivo << endl;
        return;
    }

    string contenido((istreambuf_iterator<char>(archivo)), istreambuf_iterator<char>());
    archivo.close();
    vector<string> valores;
    string valor;
    bool dentroDeComillas = false;

    for (char c : contenido) {
        if (c == '"') {
            dentroDeComillas = !dentroDeComillas;
        } else if (c == ',' && !dentroDeComillas) {
            valores.push_back(valor);
            valor.clear();
        } else if (c == '\n' && !dentroDeComillas) {
            valores.push_back(valor);
            valor.clear();

            if (valores.size() >= 6) {
                string imdb_id = valores[0];
                string titulo = valores[1];
                string sinopsis = valores[2];
                string tags = valores[3];
                arbol.insertar(imdb_id, titulo, sinopsis, tags);
            }
            valores.clear();
        } else {
            valor += c;
        }
    }
}

void mostrarInicio(ArbolBinarioBusqueda& arbol) {
    cout << "\nPeliculas guardadas en 'Ver mas tarde':\n";
    for (const auto& pelicula : arbol.obtenerVerMasTarde()) {
        cout << "- " << pelicula->titulo << " [" << pelicula->tags << "]\n";
    }

    cout << "\nPeliculas similares a las que diste Like:\n";
    for (const auto& pelicula : arbol.obtenerPeliculasSimilares()) {
        cout << "- " << pelicula->titulo << " [" << pelicula->tags << "]\n";
    }
}

void procesarSeleccion(Nodo* pelicula, ArbolBinarioBusqueda& arbol) {
    if (!pelicula) {
        cout << "Error: Pelicula no valida.\n";
        return;
    }

    string opcion;
    bool enVerMasTarde = arbol.estaEnVerMasTarde(pelicula->imdb_id);
    bool conLike = arbol.tieneLike(pelicula->imdb_id);

    cout << "\n*** Pelicula Seleccionada ***\n";
    cout << "Titulo: " << pelicula->titulo << "\n";
    cout << "Sinopsis: " << pelicula->sinopsis << "\n";
    cout << "****************************\n";

    cout << "\nOpciones:\n";
    if (enVerMasTarde) {
        cout << "1. Quitar de 'Ver mas tarde'\n";
    } else {
        cout << "1. Agregar a 'Ver mas tarde'\n";
    }
    if (conLike) {
        cout << "2. Quitar Like\n";
    } else {
        cout << "2. Dar Like\n";
    }
    cout << "0. Volver\nSeleccione una opcion: ";
    cin >> opcion;

    if (opcion == "1") {
        if (enVerMasTarde) {
            arbol.quitarVerMasTarde(pelicula->imdb_id);
            cout << "La pelicula fue removida de 'Ver mas tarde'.\n";
        } else {
            arbol.agregarVerMasTarde(pelicula->imdb_id);
            cout << "La pelicula fue agregada a 'Ver mas tarde'.\n";
        }
    } else if (opcion == "2") {
        if (conLike) {
            arbol.quitarLike(pelicula->imdb_id);
            cout << "Se removio el Like de la pelicula.\n";
        } else {
            arbol.agregarLike(pelicula->imdb_id);
            cout << "Diste Like a la pelicula.\n";
        }
    }
}

void mostrarResultados(const vector<Nodo*>& resultados, ArbolBinarioBusqueda& arbol) {
    size_t inicio = 0;
    string opcion;
    do {
        size_t fin = min(inicio + 5, resultados.size());
        cout << "\nResultados " << inicio + 1 << "-" << fin << " de " << resultados.size() << ":\n";
        for (size_t i = inicio; i < fin; ++i) {
            if (resultados[i]) {
                cout << i + 1 << ". " << resultados[i]->titulo << " [" << resultados[i]->tags << "]\n";
            }
        }

        if (fin < resultados.size()) {
            cout << "\n6. Mostrar mas resultados\n";
        }
        cout << "0. Volver al menu principal\nSeleccione una opcion: ";
        cin >> opcion;

        if (opcion == "6" && fin < resultados.size()) {
            inicio += 5;
        } else if (opcion == "0") {
            break;
        } else {
            int seleccion = stoi(opcion) - 1;
            if (seleccion >= 0 && seleccion < resultados.size()) {
                procesarSeleccion(resultados[seleccion], arbol);
            } else {
                cout << "Seleccion no valida. Intente de nuevo.\n";
            }
        }
    } while (true);
}

int main() {
    ArbolBinarioBusqueda arbol;
    cout << "Cargando peliculas, por favor espere..." << endl;
    try {
        cargarPeliculas(R"(C:\\Users\\nicov\\CLionProjects\\P3_ProyectoFinal\\mpst_full_data.csv)", arbol);
        arbol.cargarDatos();
    } catch (const exception& e) {
        cerr << "Error al cargar las peliculas: " << e.what() << endl;
        return 1;
    }
    mostrarInicio(arbol);
    string opcion;
    do {
        cout << "\n\tProyecto Final - Nicolas Vasquez de Velasco\n";
        cout << "1. Buscar por titulo o sinopsis\n";
        cout << "2. Buscar por tags\n";
        cout << "3. Salir\n";
        cout << "Seleccione una opcion: ";
        cin >> opcion;

        if (opcion == "1" || opcion == "2") {
            string entrada;
            cout << "Buscador: ";
            cin.ignore();
            getline(cin, entrada);
            vector<Nodo*> resultados = arbol.buscar(entrada, opcion == "1" ?
            "titulo_sinopsis" : "tags");
            if (!resultados.empty()) {
                mostrarResultados(resultados, arbol);
            } else {
                cout << "No se encontraron resultados.\n";
            }
        }
        system("cls");
    } while (opcion != "3");

    try {
        arbol.guardarDatos();
    } catch (const exception& e) {
        cerr << "Error al guardar datos: " << e.what() << endl;
    }
    cout << "Hasta pronto." << endl;
    sleep(1);
    return 0;
}