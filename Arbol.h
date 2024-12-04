#ifndef P3_PROYECTOFINAL_ARBOL_H
#define P3_PROYECTOFINAL_ARBOL_H
#include "Nodo.h"

class ArbolBinarioBusqueda {
private:
    Nodo* raiz;
    unordered_set<string> verMasTardeIds;
    unordered_set<string> likesIds;

    void insertarNodo(Nodo*& nodo, const string& imdb_id, const string& titulo, const string& sinopsis,
                      const string& tags);
    void buscarNodo(Nodo* nodo, const set<string>& palabrasClave, vector<pair<Nodo*, int>>& resultados,
                    const string& criterio);
    set<string> extraerPalabras(const string& entrada);
    void destruirArbol(Nodo* nodo);
    void guardarLista(const unordered_set<string>& lista, const string& archivo);
    unordered_set<string> cargarLista(const string& archivo);
    Nodo* buscarNodoPorID(Nodo* nodo, const string& id);
    vector<string> dividirTags(const string& tagsRaw);

public:
    ArbolBinarioBusqueda();
    ~ArbolBinarioBusqueda();
    void insertar(const string& imdb_id, const string& titulo, const string& sinopsis, const string& tags);
    vector<Nodo*> buscar(const string& entrada, const string& criterio = "titulo_sinopsis");
    void agregarVerMasTarde(const string& imdb_id);
    void quitarVerMasTarde(const string& imdb_id);
    bool estaEnVerMasTarde(const string& imdb_id);
    vector<Nodo*> obtenerVerMasTarde();
    void agregarLike(const string& imdb_id);
    void quitarLike(const string& imdb_id);
    bool tieneLike(const string& imdb_id);
    vector<Nodo*> obtenerPeliculasSimilares();
    void guardarDatos();
    void cargarDatos();
};

#endif