#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"
#include <iostream>
#include <cctype>
#include <unordered_map>
#include "pokedex.h"
#include "pokemon.h"
#include "PokemonCenter.h"

using namespace std;

class PokemonGame : public olc::PixelGameEngine {
private:
    Pokedex myPokedex;
    Pokemon pokemonDatabase;
    PokemonCenter pokemonCenter;
    vector<Pokemon> allPokemon;
    int selectedPokemonIndex = 0;
    int pokedexScrollOffset = 0;

    unordered_map<int, unique_ptr<olc::Sprite>> pokemonSprites;
    unordered_map<int, unique_ptr<olc::Decal>> pokemonDecals;

    enum GameState { MAIN_MENU, VIEW_ALL_POKEMON, BATTLE, MY_POKEDEX, TRANSFER, POKEMON_CENTER, SAVE, LOAD, SELECT_FIRST
    };
    GameState currentState = MAIN_MENU;

public:
    PokemonGame() { sAppName = "Pokemon Game!"; }

    bool OnUserCreate() override {
        pokemonDatabase.readJson();
        allPokemon = vector<Pokemon>(pokemonDatabase.pokemon.begin(), pokemonDatabase.pokemon.end());

        for (const auto& p : allPokemon) {
            string imagePath = "sprites/" + to_string(p.id) + ".png";
            unique_ptr<olc::Sprite> sprite = make_unique<olc::Sprite>(imagePath);

            if (sprite->width > 0 && sprite->height > 0) {
                pokemonSprites[p.id] = move(sprite);
                pokemonDecals[p.id] = make_unique<olc::Decal>(pokemonSprites[p.id].get());
            }
        }
        return true;
    }

    bool OnUserUpdate(float fElapsedTime) override {
        Clear(olc::DARK_BLUE);

        switch (currentState) {
        case MAIN_MENU: DisplayMainMenu(); break;
        case SELECT_FIRST: DisplayFirstPokemonSelection(); break;
        case VIEW_ALL_POKEMON: DisplayAllPokemon(); break;
        case BATTLE: DisplayBattleView(); break;
        case MY_POKEDEX: DisplayPokedexView(); break;
        case TRANSFER: DisplayTransferView(); break;
        case POKEMON_CENTER: DisplayPokemonCenterView(); break;
        case SAVE: SavePokedex(); break;
        case LOAD: LoadPokedex(); break;
        }
        return true;
    }

    void DisplayMainMenu() {
        DrawString(10, 10, "Welcome to Pokedex! ", olc::WHITE, 2);
        DrawString(10, 30, "Select your options:", olc::WHITE, 1);
        DrawString(10, 50, "1: Select Your First Pokemon", olc::WHITE);
        DrawString(10, 70, "2: See All Pokemon", olc::WHITE);
        DrawString(10, 90, "3: Capture Pokemon", olc::WHITE);
        DrawString(10, 110, "4: My Pokedex", olc::WHITE);
        DrawString(10, 130, "5: Pokemon Center", olc::WHITE);
        DrawString(10, 150, "6: Save Pokedex", olc::WHITE);
        DrawString(10, 170, "7: Load Pokedex", olc::WHITE);
        DrawString(10, 190, "8: Exit", olc::WHITE);

        if (GetKey(olc::Key::K1).bPressed) currentState = SELECT_FIRST;
        if (GetKey(olc::Key::K2).bPressed) currentState = VIEW_ALL_POKEMON;
        if (GetKey(olc::Key::K3).bPressed) currentState = BATTLE;
        if (GetKey(olc::Key::K4).bPressed) currentState = MY_POKEDEX;
        //if (GetKey(olc::Key::K4).bPressed) currentState = TRANSFER;
        if (GetKey(olc::Key::K5).bPressed) currentState = POKEMON_CENTER;
        if (GetKey(olc::Key::K6).bPressed) currentState = SAVE;
        if (GetKey(olc::Key::K7).bPressed) currentState = LOAD;
        if (GetKey(olc::Key::K8).bPressed) exit(0);
    }

    void DisplayFirstPokemonSelection() {
        DrawString(10, 10, "Select Your First Pokémon", olc::WHITE, 2);

        static int selectedOption = 0;
        const vector<string> starterPokemon = { "Bulbasaur", "Charmander", "Squirtle" };
        const vector<int> starterIds = { 1, 4, 7 }; // Adjusted IDs

        // Display Pokémon options
        for (int i = 0; i < starterPokemon.size(); ++i) {
            olc::Pixel color = (i == selectedOption) ? olc::YELLOW : olc::WHITE;
            DrawString(20, 50 + i * 20, to_string(i + 1) + ". " + starterPokemon[i], color);
        }

        DrawString(10, 130, "Use UP/DOWN to navigate", olc::WHITE);
        DrawString(10, 150, "Press ENTER to select", olc::WHITE);

        // Navigation
        if (GetKey(olc::Key::UP).bPressed) {
            selectedOption = (selectedOption - 1 + starterPokemon.size()) % starterPokemon.size();
        }
        if (GetKey(olc::Key::DOWN).bPressed) {
            selectedOption = (selectedOption + 1) % starterPokemon.size();
        }
        

        // Confirm selection
        if (GetKey(olc::Key::ENTER).bPressed) {
            if (selectedOption == 1)
                selectedOption = 3;
            if (selectedOption == 2)
                selectedOption = 6;
            Pokemon firstPokemon = allPokemon[selectedOption]; // Using allPokemon list
            myPokedex.capturePokemon(firstPokemon);
            currentState = MAIN_MENU; // Move to the main menu
        }
        
        DrawString(10, 180, "Press SPACE to return", olc::WHITE);

        if (GetKey(olc::Key::SPACE).bPressed) currentState = MAIN_MENU;
    }

    void DisplayAllPokemon() {
        DrawString(10, 10, "All Pokemon", olc::WHITE, 2);

        if (!allPokemon.empty()) {
            Pokemon& p = allPokemon[selectedPokemonIndex];

            // Display the current Pokémon’s details
            DrawString(50, 70, "ID: " + to_string(p.id), olc::WHITE, 1);
            DrawString(50, 90, "Name: " + p.name, olc::WHITE, 1);
            DrawString(50, 110, "Type: " + p.type, olc::WHITE, 1);

            // Draw the Pokémon sprite if available
            if (pokemonDecals.count(p.id)) {
                DrawDecal({ 200, 50 }, pokemonDecals[p.id].get(), { 0.08f, 0.08f });
            }
            else {
                DrawRect(200, 40, 100, 100, olc::WHITE);  // Placeholder box
            }

            // Allow scrolling through Pokémon list
            if (GetKey(olc::Key::LEFT).bPressed) {
                selectedPokemonIndex = (selectedPokemonIndex - 1 + allPokemon.size()) % allPokemon.size();
            }
            if (GetKey(olc::Key::RIGHT).bPressed) {
                selectedPokemonIndex = (selectedPokemonIndex + 1) % allPokemon.size();
            }
        }
        DrawString(10, 180, "Press LEFT/RIGHT to browse Pokemon", olc::WHITE);
        DrawString(10, 200, "Press SPACE to return", olc::WHITE);

        if (GetKey(olc::Key::SPACE).bPressed) currentState = MAIN_MENU;
    }



    void DisplayBattleView() {
        DrawString(10, 10, "Capture Pokemon Battle", olc::WHITE, 2);

        static bool battleStarted = false;
        static Pokemon newPokemon;
        static Pokemon* myPokemon = nullptr;

        if (!battleStarted) {
            // Ensure Pokémon list isn't empty
            if (allPokemon.empty()) {
                DrawString(10, 40, "No Pokémon available!", olc::WHITE);
                return;
            }

            // Not work yet!
            if (selectedPokemonIndex < allPokemon.size()) {
                newPokemon = allPokemon[11];

            }
            else {
                DrawString(10, 40, "Invalid Pokémon selection!", olc::WHITE);
                return;
            }

            // Ensure the player has Pokémon to fight
            if (myPokedex.getCapturedPokemon().empty()) {
                DrawString(10, 60, "You have no Pokémon to fight!", olc::WHITE);
                return;
            }
            
            myPokemon = &myPokedex.selectMyPokemon(1); // Select first Pokémon for now
            battleStarted = true;
        }

        // Display both Pokémon stats
        DrawString(50, 50, "Your Pokemon: " + myPokemon->name, olc::WHITE, 1);
        DrawString(50, 70, "Health: " + to_string(myPokemon->health), olc::WHITE, 1);

        DrawString(200, 50, "Wild " + newPokemon.name, olc::WHITE, 1);
        DrawString(200, 70, "Health: " + to_string(newPokemon.health), olc::WHITE, 1);

        // Draw Pokémon sprites if available
        if (pokemonDecals.count(myPokemon->id)) {
            DrawDecal({ 50, 100 }, pokemonDecals[myPokemon->id].get(), { 0.08f, 0.08f });
        }
        if (pokemonDecals.count(newPokemon.id)) {
            DrawDecal({ 200, 100 }, pokemonDecals[newPokemon.id].get(), { 0.08f, 0.08f });
        }

        DrawString(10, 220, "Press ENTER to attack", olc::WHITE);
        DrawString(10, 250, "Press SPACE to return", olc::WHITE);

        if (GetKey(olc::Key::ENTER).bPressed) {
            // Battle sequence
            int damage = myPokemon->attack(); 

            if (newPokemon.health <= 0) {
                newPokemon.health = 0;
                myPokedex.capturePokemon(newPokemon);
                DrawString(10, 220, "You captured " + newPokemon.name + "!", olc::GREEN);
                battleStarted = false;  // Reset for next battle
                return;
            }

            int dmgReceived = newPokemon.attack();
            myPokemon->health -= dmgReceived;
            if (myPokemon->health <= 0) {
                myPokemon->health = 0;
                DrawString(10, 220, myPokemon->name + " fainted!", olc::RED);
            }
        }

        if (GetKey(olc::Key::SPACE).bPressed) {
            battleStarted = false;  // Reset battle on exit
            currentState = MAIN_MENU;
        }


    }

    void DisplayPokedexView() {
        DrawString(10, 10, "My Pokedex", olc::WHITE, 2);
        const vector<Pokemon>& capturedPokemon = myPokedex.getCapturedPokemon();

        if (!capturedPokemon.empty()) {
            const Pokemon& p = capturedPokemon[selectedPokemonIndex];

            // Display Pokémon details
            DrawString(50, 70, "ID: " + to_string(p.id), olc::WHITE, 1);
            DrawString(50, 90, "Name: " + p.name, olc::WHITE, 1);
            DrawString(50, 110, "Type: " + p.type, olc::WHITE, 1);
            DrawString(50, 130, "Health: " + p.health, olc::WHITE, 1);

            // Draw Pokémon sprite if available
            if (pokemonDecals.count(p.id)) {
                DrawDecal({ 200, 50 }, pokemonDecals[p.id].get(), { 0.08f, 0.08f });
            }
            else {
                DrawRect(200, 40, 100, 100, olc::WHITE);  // Placeholder if no sprite
            }

            // Allow browsing through captured Pokémon
            if (GetKey(olc::Key::LEFT).bPressed) {
                selectedPokemonIndex = (selectedPokemonIndex - 1 + capturedPokemon.size()) % capturedPokemon.size();
            }
            if (GetKey(olc::Key::RIGHT).bPressed) {
                selectedPokemonIndex = (selectedPokemonIndex + 1) % capturedPokemon.size();
            }
        }
        else {
            DrawString(50, 100, "No Pokémon captured yet!", olc::WHITE, 1);
        }
        DrawString(10, 180, "Press LEFT/RIGHT to browse Pokémon", olc::WHITE);
        DrawString(10, 200, "Press SPACE to return", olc::WHITE);
        DrawString(10, 220, "Press T to Transfer a pokemon to Professor Oak", olc::WHITE);

        if (GetKey(olc::Key::SPACE).bPressed) currentState = MAIN_MENU;
        if (GetKey(olc::Key::T).bPressed) currentState = TRANSFER;
    }

    void DisplayTransferView() {
        DrawString(10, 10, "Transfer Pokemon", olc::WHITE, 2);

        const vector<Pokemon>& capturedPokemon = myPokedex.getCapturedPokemon();

        if (!capturedPokemon.empty()) {
            const Pokemon& p = capturedPokemon[selectedPokemonIndex];  // Select the Pokémon to transfer

            // Display selected Pokémon details
            DrawString(50, 70, "ID: " + to_string(p.id), olc::WHITE, 1);
            DrawString(50, 90, "Name: " + p.name, olc::WHITE, 1);
            DrawString(50, 110, "Type: " + p.type, olc::WHITE, 1);

            // Draw Pokémon sprite if available
            if (pokemonDecals.count(p.id)) {
                DrawDecal({ 200, 50 }, pokemonDecals[p.id].get(), { 0.08f, 0.08f });
            }
            else {
                DrawRect(200, 40, 100, 100, olc::WHITE);  // Placeholder if no sprite
            }

            // Instructions for navigation
            DrawString(10, 180, "Press LEFT/RIGHT to browse Pokemon", olc::WHITE);
            DrawString(10, 200, "Press ENTER to transfer", olc::WHITE);
            DrawString(10, 220, "Press SPACE to return", olc::WHITE);

            // Change selected Pokémon
            if (GetKey(olc::Key::LEFT).bPressed) {
                selectedPokemonIndex = (selectedPokemonIndex - 1 + capturedPokemon.size()) % capturedPokemon.size();
            }
            if (GetKey(olc::Key::RIGHT).bPressed) {
                selectedPokemonIndex = (selectedPokemonIndex + 1) % capturedPokemon.size();
            }

            // Transfer selected Pokémon
            if (GetKey(olc::Key::ENTER).bPressed) {
                myPokedex.deletePokemon(p.id);
                selectedPokemonIndex = 0;  // Reset selection
                if (capturedPokemon.empty()) {
                    currentState = MAIN_MENU;  // Return to main menu if no Pokémon left
                }
            }
        }
        else {
            DrawString(50, 100, "No Pokemon to transfer!", olc::WHITE, 1);
        }

        // Return to main menu
        if (GetKey(olc::Key::SPACE).bPressed) currentState = MAIN_MENU;
    }

    void DisplayPokemonCenterView() {
        //DrawString(10, 10, "Pokemon Center", olc::WHITE, 2);

        //static int selectedIndex = 0;
        //const vector<Pokemon>& myPokemon = myPokedex.getCapturedPokemon(); // Get player's Pokémon
        //

        //if (myPokemon.empty()) {
        //    DrawString(20, 50, "No Pokémon available for healing.", olc::WHITE);
        //}
        //else {
        //    DrawString(10, 30, "Select Pokémon to heal:", olc::WHITE);

        //    // Display Pokémon options
        //    for (int i = 0; i < myPokemon.size(); ++i) {
        //        olc::Pixel color = (i == selectedIndex) ? olc::YELLOW : olc::WHITE;
        //        DrawString(20, 50 + i * 20, to_string(myPokemon[i].id) + " - " + myPokemon[i].name, color);
        //    }

        //    DrawString(10, 200, "Press UP/DOWN to navigate", olc::WHITE);
        //    DrawString(10, 220, "Press ENTER to heal", olc::WHITE);
        //}

        //DrawString(10, 240, "Press SPACE to return", olc::WHITE);

        //// Navigation through Pokémon list
        //if (GetKey(olc::Key::UP).bPressed) {
        //    selectedIndex = (selectedIndex - 1 + myPokemon.size()) % myPokemon.size();
        //}
        //if (GetKey(olc::Key::DOWN).bPressed) {
        //    selectedIndex = (selectedIndex + 1) % myPokemon.size();
        //}

        //// Heal the selected Pokémon when ENTER is pressed
        //if (GetKey(olc::Key::ENTER).bPressed && !myPokemon.empty()) {
        //    pokemonCenter.enqueue(myPokemon[selectedIndex]); // Send Pokémon to healing queue
        //    while (!pokemonCenter.isEmpty()) {
        //        pokemonCenter.dequeue(); // Heal the Pokémon
        //    }
        //}

        // Return to main menu
        if (GetKey(olc::Key::SPACE).bPressed) currentState = MAIN_MENU;
    

        DrawString(10, 10, "Pokemon Center", olc::WHITE, 2);
        DrawString(10, 70, "Pokemon Center will open soon...", olc::WHITE, 1);
        DrawString(10, 90, "Press SPACE to return", olc::WHITE);
        if (GetKey(olc::Key::SPACE).bPressed) currentState = MAIN_MENU;
    }


    void SavePokedex() {
        DrawString(10, 10, "Pokedex is saved!", olc::WHITE, 2);
        myPokedex.saveMyPokedex("my_pokedex");
        currentState = MAIN_MENU;
        DrawString(10, 40, "Press SPACE to return", olc::WHITE);
        if (GetKey(olc::Key::SPACE).bPressed) currentState = MAIN_MENU;
    }

    void LoadPokedex() {
        DrawString(10, 10, "Pokedex is loade!", olc::WHITE, 2);
        myPokedex.loadMyPokedex("my_pokedex");
        currentState = MAIN_MENU;
        DrawString(10, 40, "Press SPACE to return", olc::WHITE);
        if (GetKey(olc::Key::SPACE).bPressed) currentState = MAIN_MENU;
    }
};

int main() {
    PokemonGame game;
    if (game.Construct(400, 300, 2, 2))
        game.Start();
    return 0;
}
