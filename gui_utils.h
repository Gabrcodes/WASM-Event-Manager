#pragma once 

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <string>
#include <vector>

// Define AppState here as it's fundamental to GUI button actions
enum AppState {
    STATE_USER_DETAILS, 
    STATE_MAIN_MENU, 
    STATE_CREATE_EVENT_TYPE, 
    STATE_CREATE_EVENT_DETAILS,
    STATE_SHOW_ALL_EVENTS, 
    STATE_SIGN_UP_FOR_EVENT, 
    STATE_DELETE_EVENT, 
    STATE_SEARCH_EVENT,
    STATE_SEARCH_RESULTS, 
    STATE_CONFIRM_ACTION, 
    STATE_EXITED 
};

// --- GUI Helper Struct Definitions ---
struct Button {
    SDL_Rect rect;
    std::string text; 
    AppState targetState; 
    int actionId;         
    bool hovered;

    Button(int x, int y, int w, int h, std::string t, AppState ts, int aid = -1);
};

struct InputField {
    SDL_Rect rect;
    std::string text; 
    std::string placeholder; 
    bool isActive;
    int maxLength;

    InputField(int x, int y, int w, int h, std::string p);
};


// --- GUI Function Declarations ---
void render_text(const std::string& text, int x, int y, SDL_Color color, bool center = false, int wrapWidth = 0);
void render_button(Button& button); 
void render_input_field(const InputField& field);
void update_button_hover(int mouseX, int mouseY, std::vector<Button>& buttons);

// --- Extern declarations for Global GUI Constants (defined in main.cpp) ---
extern const int SCREEN_WIDTH;
extern const int SCREEN_HEIGHT; // If needed by gui_utils.cpp, though not currently
extern SDL_Color TEXT_COLOR;
extern SDL_Color ERROR_TEXT_COLOR;
extern SDL_Color SUCCESS_TEXT_COLOR;
extern SDL_Color INPUT_BG_COLOR;
extern SDL_Color BUTTON_COLOR;
extern SDL_Color BUTTON_HOVER_COLOR;

// Extern declarations for Global SDL variables (defined in main.cpp)
// These are pointers, not const, so their linkage is already external by default if defined globally.
// However, explicitly declaring them extern in the header is good practice for clarity.
extern SDL_Window* gWindow;
extern SDL_Renderer* gRenderer;
extern TTF_Font* gFont;
