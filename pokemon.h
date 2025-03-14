#ifndef POKEMON_H
#define POKEMON_H
#include <set>
#include <iostream>
#include "json.hpp"

using namespace std;

class Pokemon {
public:
    int id;
    string name;
    string type;
    string image;
    int health;
    set<Pokemon> pokemon;
    bool operator<(const Pokemon& other) const;
    void readJson();
    void showAll();
    int attack();
    Pokemon searchPokemon(int id);
    Pokemon() : id(0), health(100) {}
};

#endif