#include "Arbol.h"

string convertirMinusculas(const string& texto) {
    string resultado = texto;
    transform(resultado.begin(), resultado.end(), resultado.begin(), ::tolower);
    return resultado;
}

ArbolBinarioBusqueda::ArbolBinarioBusqueda() : raiz(nullptr) {}

ArbolBinarioBusqueda::~ArbolBinarioBusqueda() {
    destruirArbol(raiz);
}

void ArbolBinarioBusqueda::destruirArbol(Nodo* nodo) {
    if (nodo) {
        destruirArbol(nodo->left);
        destruirArbol(nodo->right);
        delete nodo;
    }
}

void ArbolBinarioBusqueda::insertar(const string& imdb_id, const string& titulo, const string& sinopsis, const string& tags) {
    if (!imdb_id.empty() && !titulo.empty()) {
        insertarNodo(raiz, imdb_id, titulo, sinopsis, tags);
    } else {
        cerr << "Error: Nodo con datos vacíos no insertado (" << titulo << ").\n";
    }
}

void ArbolBinarioBusqueda::insertarNodo(Nodo*& nodo, const string& imdb_id, const string& titulo, const string& sinopsis, const string& tags) {
    if (!nodo) {
        nodo = new Nodo(imdb_id, titulo, sinopsis, tags);
    } else if (imdb_id < nodo->imdb_id) {
        insertarNodo(nodo->left, imdb_id, titulo, sinopsis, tags);
    } else {
        insertarNodo(nodo->right, imdb_id, titulo, sinopsis, tags);
    }
}

vector<Nodo*> ArbolBinarioBusqueda::buscar(const string& entrada, const string& criterio) {
    vector<pair<Nodo*, int>> resultadosConPuntaje;
    vector<Nodo*> resultadosFinales;
    set<string> palabrasClave = extraerPalabras(entrada);

    if (criterio == "titulo_sinopsis" || criterio == "tags") {
        buscarNodo(raiz, palabrasClave, resultadosConPuntaje, criterio);
    }

    if (criterio == "tags") {
        sort(resultadosConPuntaje.begin(), resultadosConPuntaje.end(),
             [](const pair<Nodo*, int>& a, const pair<Nodo*, int>& b) {
            return a.first->imdb_id < b.first->imdb_id;
        });
    } else {
        sort(resultadosConPuntaje.begin(), resultadosConPuntaje.end(),
             [](const pair<Nodo*, int>& a, const pair<Nodo*, int>& b) {
            return a.second > b.second;
        });
    }

    for (const auto& resultado : resultadosConPuntaje) {
        resultadosFinales.push_back(resultado.first);
    }

    return resultadosFinales;
}

set<string> ArbolBinarioBusqueda::extraerPalabras(const string& entrada) {
    set<string> palabras;
    istringstream stream(convertirMinusculas(entrada));
    string palabra;
    while (stream >> palabra) {
        palabras.insert(palabra);
    }
    return palabras;
}

void ArbolBinarioBusqueda::buscarNodo(Nodo* nodo, const set<string>& palabrasClave, vector<pair<Nodo*,
                                      int>>& resultados, const string& criterio) {
    if (!nodo) return;
    int puntaje = 0;

    if (criterio == "titulo_sinopsis") {
        for (const auto& palabra : palabrasClave) {
            string tituloLower = convertirMinusculas(nodo->titulo);
            string sinopsisLower = convertirMinusculas(nodo->sinopsis);

            size_t posTitulo = 0, posSinopsis = 0;
            while ((posTitulo = tituloLower.find(palabra, posTitulo)) != string::npos) {
                puntaje += 10;
                posTitulo += palabra.length();
            }
            while ((posSinopsis = sinopsisLower.find(palabra, posSinopsis)) != string::npos) {
                puntaje += 5;
                posSinopsis += palabra.length();
            }
        }
    } else if (criterio == "tags") {
        vector<string> tagsNodo = dividirTags(nodo->tags);
        for (const auto& palabra : palabrasClave) {
            for (const auto& tag : tagsNodo) {
                if (convertirMinusculas(tag) == palabra) {
                    puntaje += 10;
                    break;
                }
            }
        }
    }

    if (puntaje > 0) {
        resultados.push_back({nodo, puntaje});
    }

    buscarNodo(nodo->left, palabrasClave, resultados, criterio);
    buscarNodo(nodo->right, palabrasClave, resultados, criterio);
}

void ArbolBinarioBusqueda::agregarVerMasTarde(const string& imdb_id) {
    verMasTardeIds.insert(imdb_id);
}

void ArbolBinarioBusqueda::quitarVerMasTarde(const string& imdb_id) {
    verMasTardeIds.erase(imdb_id);
}

bool ArbolBinarioBusqueda::estaEnVerMasTarde(const string& imdb_id) {
    return verMasTardeIds.count(imdb_id) > 0;
}

vector<Nodo*> ArbolBinarioBusqueda::obtenerVerMasTarde() {
    vector<Nodo*> resultado;
    for (const auto& id : verMasTardeIds) {
        Nodo* nodo = buscarNodoPorID(raiz, id);
        if (nodo) resultado.push_back(nodo);
    }
    return resultado;
}

void ArbolBinarioBusqueda::agregarLike(const string& imdb_id) {
    likesIds.insert(imdb_id);
}

void ArbolBinarioBusqueda::quitarLike(const string& imdb_id) {
    likesIds.erase(imdb_id);
}

bool ArbolBinarioBusqueda::tieneLike(const string& imdb_id) {
    return likesIds.count(imdb_id) > 0;
}

vector<Nodo*> ArbolBinarioBusqueda::obtenerPeliculasSimilares() {
    unordered_set<string> tagsLikes;
    for (const auto& id : likesIds) {
        Nodo* nodo = buscarNodoPorID(raiz, id);
        if (nodo) {
            vector<string> tags = dividirTags(nodo->tags);
            tagsLikes.insert(tags.begin(), tags.end());
        }
    }

    vector<Nodo*> similares;
    function<void(Nodo*)> buscarSimilares = [&](Nodo* nodo) {
        if (!nodo || similares.size() >= 3) return;

        if (likesIds.find(nodo->imdb_id) == likesIds.end()) {
            vector<string> tagsNodo = dividirTags(nodo->tags);
            int coincidencias = count_if(tagsNodo.begin(), tagsNodo.end(), [&tagsLikes](const string& tag) {
                return tagsLikes.count(tag) > 0;
            });

            if (coincidencias > 0) {
                similares.push_back(nodo);
            }
        }

        buscarSimilares(nodo->left);
        buscarSimilares(nodo->right);
    };

    buscarSimilares(raiz);
    return similares;
}

void ArbolBinarioBusqueda::guardarLista(const unordered_set<string>& lista, const string& archivo) {
    ofstream out(archivo);
    if (!out.is_open()) return;

    for (const auto& id : lista) {
        out << id << "\n";
    }

    out.close();
}

unordered_set<string> ArbolBinarioBusqueda::cargarLista(const string& archivo) {
    unordered_set<string> lista;
    ifstream in(archivo);
    if (!in.is_open()) return lista;

    string id;
    while (getline(in, id)) {
        lista.insert(id);
    }

    in.close();
    return lista;
}

void ArbolBinarioBusqueda::guardarDatos() {
    guardarLista(verMasTardeIds, "ver_mas_tarde.txt");
    guardarLista(likesIds, "likes.txt");
}

void ArbolBinarioBusqueda::cargarDatos() {
    verMasTardeIds = cargarLista("ver_mas_tarde.txt");
    likesIds = cargarLista("likes.txt");
}

vector<string> ArbolBinarioBusqueda::dividirTags(const string& tagsRaw) {
    vector<string> tags;
    stringstream ss(tagsRaw);
    string tag;
    while (getline(ss, tag, ',')) {
        tags.push_back(tag);
    }
    return tags;
}

Nodo* ArbolBinarioBusqueda::buscarNodoPorID(Nodo* nodo, const string& id) {
    if (!nodo) return nullptr;

    if (nodo->imdb_id == id) return nodo;
    if (id < nodo->imdb_id) return buscarNodoPorID(nodo->left, id);
    return buscarNodoPorID(nodo->right, id);
}