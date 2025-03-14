#ifndef POKEDEX_H
#define POKEDEX_H
#include <iostream>
#include <vector>
#include "Pokemon.h"

using namespace std;

class Pokedex {
private:
    vector<Pokemon> myPokedex;

public:
    Pokedex();
    void selectFirstPokemon(Pokemon& pokemon);
    void capturePokemon(Pokemon p);
    void deletePokemon(int id);
    void showMyPokemons();
    void saveMyPokedex(const string& filename);
    void loadMyPokedex(const string& filename);
    Pokemon& selectMyPokemon(int id);
    const vector<Pokemon>& getCapturedPokemon() const;
};
#endif