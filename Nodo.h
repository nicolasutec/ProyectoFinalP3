#ifndef P3_PROYECTOFINAL_NODO_H
#define P3_PROYECTOFINAL_NODO_H
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <cctype>
#include <iostream>
#include <fstream>
#include <sstream>
#include <set>
#include <unordered_set>
#include <unistd.h>
#include <regex>
#include <utility>
using namespace std;

struct Nodo {
    string imdb_id;
    string titulo;
    string sinopsis;
    string tags;
    Nodo* left;
    Nodo* right;

    Nodo(const string& imdb_id, const string& titulo, const string& sinopsis, const string& tags)
            : imdb_id(imdb_id), titulo(titulo), sinopsis(sinopsis), tags(tags), left(nullptr),
            right(nullptr) {}
};
#endif
