#include "gui_utils.h"
#include <iostream> // For cerr if needed in these functions

// Since AppState is only forward-declared in gui_utils.h,
// the Button constructor which takes AppState as a parameter type is fine.
// If specific AppState::SOME_VALUE was used in this .cpp file AND AppState
// was only forward-declared, it would be an issue. But it's not.

// --- GUI Helper Struct Definitions ---
Button::Button(int x, int y, int w, int h, string t, AppState ts, int aid)
    : text(std::move(t)), targetState(ts), actionId(aid), hovered(false) {
    rect = {x, y, w, h};
}

InputField::InputField(int x, int y, int w, int h, string p)
    : placeholder(std::move(p)), text(""), isActive(false), maxLength(100) {
    rect = {x, y, w, h};
}


// --- GUI Function Definitions ---

// These functions rely on global variables defined in main.cpp
// (gRenderer, gFont, SCREEN_WIDTH, various SDL_Colors).
// This is a common way to handle it in simpler SDL apps, but for larger
// projects, passing a "RenderContext" struct or class might be cleaner.
extern SDL_Renderer* gRenderer; 
extern TTF_Font* gFont;
extern const int SCREEN_WIDTH; 
extern SDL_Color BUTTON_HOVER_COLOR; 
extern SDL_Color BUTTON_COLOR;
extern SDL_Color TEXT_COLOR;
extern SDL_Color INPUT_BG_COLOR;


void render_text(const string& text, int x, int y, SDL_Color color, bool center, int wrapWidth) { 
    if (!gRenderer) { /* cerr << "render_text: gRenderer is null!" << endl; */ return; }
    if (!gFont) { /* cerr << "render_text: gFont is null! Cannot render: " << text.substr(0,50) << endl; */ return; }
    if (text.empty()) { return; }

    SDL_Surface* textSurface = nullptr;
    if (wrapWidth > 0) {
        textSurface = TTF_RenderText_Blended_Wrapped(gFont, text.c_str(), color, wrapWidth);
    } else {
        textSurface = TTF_RenderText_Blended(gFont, text.c_str(), color);
    }
    
    if (!textSurface) {
        cerr << "Unable to render text surface! SDL_ttf Error: " << TTF_GetError() << " for text: " << text.substr(0,50) << endl;
        return;
    }

    SDL_Texture* textTexture = SDL_CreateTextureFromSurface(gRenderer, textSurface);
    if (!textTexture) {
        cerr << "Unable to create texture from rendered text! SDL Error: " << SDL_GetError() << endl;
        SDL_FreeSurface(textSurface);
        return;
    }

    int textW = textSurface->w;
    int textH = textSurface->h;
    
    SDL_Rect renderQuad = {x, y, textW, textH};
    if (center) {
        renderQuad.x = (SCREEN_WIDTH - textW) / 2; 
    }

    SDL_FreeSurface(textSurface); 

    SDL_RenderCopy(gRenderer, textTexture, nullptr, &renderQuad);
    SDL_DestroyTexture(textTexture);
}

void render_button(Button& button) {
    if (!gRenderer) return;

    SDL_Color colorToUse = button.hovered ? BUTTON_HOVER_COLOR : BUTTON_COLOR;
    SDL_SetRenderDrawColor(gRenderer, colorToUse.r, colorToUse.g, colorToUse.b, colorToUse.a);
    SDL_RenderFillRect(gRenderer, &button.rect);

    if (gFont && !button.text.empty()) {
      int text_h = TTF_FontHeight(gFont);
      render_text(button.text, 
                  button.rect.x + 10, 
                  button.rect.y + (button.rect.h - text_h) / 2, 
                  TEXT_COLOR, 
                  false, 
                  button.rect.w - 20); 
    }
}

void render_input_field(const InputField& field) {
    if (!gRenderer) return;

    SDL_SetRenderDrawColor(gRenderer, INPUT_BG_COLOR.r, INPUT_BG_COLOR.g, INPUT_BG_COLOR.b, INPUT_BG_COLOR.a);
    SDL_RenderFillRect(gRenderer, &field.rect);

    if (field.isActive) {
        SDL_SetRenderDrawColor(gRenderer, TEXT_COLOR.r, TEXT_COLOR.g, TEXT_COLOR.b, 255); 
        SDL_RenderDrawRect(gRenderer, &field.rect);
    }

    if (gFont) {
        string displayText = field.text;
        int text_h = TTF_FontHeight(gFont);
        int text_y = field.rect.y + (field.rect.h - text_h) / 2; 

        if (field.text.empty() && !field.isActive && !field.placeholder.empty()) {
            displayText = field.placeholder;
            render_text(displayText, field.rect.x + 5, text_y, {128, 128, 128, 255}); 
        } else {
             render_text(displayText + (field.isActive ? "_" : ""), field.rect.x + 5, text_y, TEXT_COLOR); 
        }
    }
}

void update_button_hover(int mouseX, int mouseY, vector<Button>& buttons) {
    for (auto& button : buttons) { 
        button.hovered = (mouseX >= button.rect.x && mouseX <= button.rect.x + button.rect.w &&
                          mouseY >= button.rect.y && mouseY <= button.rect.y + button.rect.h);
    }
}
