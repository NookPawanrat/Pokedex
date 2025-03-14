#ifndef POKEMONCENTER_H
#define POKEMONCENTER_H
#include <queue>
#include "Pokemon.h"

class PokemonCenter {
private:
    std::queue<Pokemon*> healingQueue;

public:
    PokemonCenter() {};
    void enqueue(Pokemon& p);  // Add a Pokemon to the queue
    void dequeue();  // Heal and remove the first Pokemon 
    void showQueue();
    bool isEmpty();
};
#endif