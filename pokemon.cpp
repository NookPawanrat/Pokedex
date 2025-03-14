#include <fstream>
#include "Pokemon.h"
#include "PokemonCenter.h"

using namespace std;

using json = nlohmann::json;

bool Pokemon::operator<(const Pokemon& other) const {
    return id < other.id;
}

void Pokemon::readJson() {
    ifstream file("pokedex.json");
    if (!file.is_open()) {
        cout << "Error trying to open JSON\n";
        return;
    }

    json j;
    file >> j;

    for (const auto& item : j["pokemon"]) {
        Pokemon p;
        p.id = item["id"];
        p.health = 100;
        p.name = item["name"];
        p.type = item["type"][0];
        p.image = item["img"];
        pokemon.insert(p);
    }
}

void Pokemon::showAll() {
    for (auto p : pokemon) {
        cout << "- #" << p.id << " " << p.name << " -" << endl;
        cout << "Type: " << p.type << endl << endl;
    }
}

Pokemon Pokemon::searchPokemon(int id) {
    Pokemon poke = Pokemon();
    if (id > 151 || id < 1) {
        cout << "Not a valid pokemon number.\n  " << endl;
        return poke;
    }
    for (Pokemon p : pokemon) {
        if (p.id == id)
            poke = p;
    }
    return poke;
}

int Pokemon::attack() {
    int damage = rand() % 51; //between 0 and 50
    return damage;
}
