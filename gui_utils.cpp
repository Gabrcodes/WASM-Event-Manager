#include "gui_utils.h" // This now brings in the extern declarations
#include <iostream> 

// The extern declarations for gRenderer, gFont, SCREEN_WIDTH, colors etc.,
// are now in gui_utils.h and will be linked from main.cpp's definitions.
// No need for local 'extern' keywords here anymore.

// --- GUI Helper Struct Definitions ---
Button::Button(int x, int y, int w, int h, std::string t, AppState ts, int aid)
    : text(std::move(t)), targetState(ts), actionId(aid), hovered(false) {
    rect = {x, y, w, h};
}

InputField::InputField(int x, int y, int w, int h, std::string p)
    : placeholder(std::move(p)), text(""), isActive(false), maxLength(100) {
    rect = {x, y, w, h};
}


// --- GUI Function Definitions ---
void render_text(const std::string& text, int x, int y, SDL_Color color, bool center, int wrapWidth) { 
    if (!gRenderer) { return; }
    if (!gFont) { std::cerr << "render_text: gFont is null! Cannot render: " << text.substr(0,50) << std::endl; return; }
    if (text.empty()) { return; }

    SDL_Surface* textSurface = nullptr;
    if (wrapWidth > 0) {
        textSurface = TTF_RenderText_Blended_Wrapped(gFont, text.c_str(), color, wrapWidth);
    } else {
        textSurface = TTF_RenderText_Blended(gFont, text.c_str(), color);
    }
    
    if (!textSurface) {
        std::cerr << "Unable to render text surface! SDL_ttf Error: " << TTF_GetError() << " for text: " << text.substr(0,50) << std::endl;
        return;
    }

    SDL_Texture* textTexture = SDL_CreateTextureFromSurface(gRenderer, textSurface);
    if (!textTexture) {
        std::cerr << "Unable to create texture from rendered text! SDL Error: " << SDL_GetError() << std::endl;
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
        std::string displayText = field.text; 
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

void update_button_hover(int mouseX, int mouseY, std::vector<Button>& buttons) { 
    for (auto& button : buttons) { 
        button.hovered = (mouseX >= button.rect.x && mouseX <= button.rect.x + button.rect.w &&
                          mouseY >= button.rect.y && mouseY <= button.rect.y + button.rect.h);
    }
}
