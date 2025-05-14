#pragma once 

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <string>
#include <vector>

using namespace std;

// Forward declare AppState if its full definition is in main.cpp
// This is okay as long as we only use AppState as a type here (e.g., for parameters or members)
// and don't need to know its specific enumerator values within this header.
enum AppState; 

// --- GUI Helper Struct Definitions ---
struct Button {
    SDL_Rect rect;
    string text;
    AppState targetState; 
    int actionId;         
    bool hovered;

    Button(int x, int y, int w, int h, string t, AppState ts, int aid = -1);
};

struct InputField {
    SDL_Rect rect;
    string text;
    string placeholder;
    bool isActive;
    int maxLength;

    InputField(int x, int y, int w, int h, string p);
};


// --- GUI Function Declarations ---
void render_text(const string& text, int x, int y, SDL_Color color, bool center = false, int wrapWidth = 0);
void render_button(Button& button); 
void render_input_field(const InputField& field);
void update_button_hover(int mouseX, int mouseY, vector<Button>& buttons);
