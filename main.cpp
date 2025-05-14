#include <iostream>
#include <string>
#include <vector>
#include <limits>
#include <algorithm>
#include <stdexcept> 

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

// Include the new header files
#include "gui_utils.h" // Defines AppState, Button, InputField, render functions, extern consts
#include "app_logic.h" // Defines eventType, User, events etc.

// Using std namespace for convenience in this main file
using namespace std;

// --- Global Variable Definitions ---

// SDL and Font Globals (initialized in init_sdl)
SDL_Window* gWindow = nullptr;
SDL_Renderer* gRenderer = nullptr;
TTF_Font* gFont = nullptr;

// GUI Constants (Definitions for externs in gui_utils.h)
const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600; // Not currently externed, but good to keep with other consts
SDL_Color TEXT_COLOR = {0, 0, 0, 255};
SDL_Color ERROR_TEXT_COLOR = {200, 0, 0, 255};
SDL_Color SUCCESS_TEXT_COLOR = {0, 128, 0, 255};
SDL_Color INPUT_BG_COLOR = {220, 220, 220, 255};
SDL_Color BUTTON_COLOR = {180, 180, 220, 255};
SDL_Color BUTTON_HOVER_COLOR = {200, 200, 240, 255};

// AppState enum is defined in gui_utils.h
AppState currentState = STATE_USER_DETAILS; 

User* currentUser = nullptr; 
events* eventManager = nullptr; 

// UI Interaction Globals
string currentMessage = ""; 
vector<string> displayedEventList; 
string inputBuffer_generic = ""; 
string suggestedMatch; 
int actionToConfirm = -1; 
eventType pendingEventType; 

// Global InputField Instances
InputField eventTitleInput(50, 150, 300, 30, "Event Title");
InputField eventDescInput(50, 200, 300, 30, "Description (max 100 chars)");
InputField eventDateInput(50, 250, 300, 30, "Date & Time (DD/MM/YYYY HH:MM)");
InputField eventPlatformInput(50, 300, 300, 30, "Virtual Platform");
InputField eventCapacityInput(50, 350, 100, 30, "Capacity (number)");

InputField userNameInput(250, 150, 300, 30, "Your Name");
InputField userEmailInput(250, 200, 300, 30, "Your Email");
InputField userPhoneInput(250, 250, 300, 30, "Your Phone");
InputField userCompanyInput(250, 300, 300, 30, "Company/School");

InputField sharedInputField(SCREEN_WIDTH/2 - 150, 150, 300, 30, "Enter text here"); 

vector<InputField*> activeInputFieldsOnScreen; 
InputField* focusedInputField = nullptr;     

// --- Forward declarations for helper functions defined in this file ---
void clear_input_fields_for_create_event();
void clear_generic_input_buffer();
void reset_confirmation_state();
void handle_mouse_click(int mouseX, int mouseY, vector<Button>& buttons, vector<InputField*>& currentScreenInputFields);
void main_loop_iteration();
bool init_sdl();
void close_sdl();


// --- Main Application Logic Functions (specific to main.cpp orchestration) ---

void handle_mouse_click(int mouseX, int mouseY, vector<Button>& buttons, vector<InputField*>& currentScreenInputFields) {
    bool fieldClickedThisTurn = false;
    for (InputField* field : currentScreenInputFields) { 
        if (mouseX >= field->rect.x && mouseX <= field->rect.x + field->rect.w &&
            mouseY >= field->rect.y && mouseY <= field->rect.y + field->rect.h) {
            if(focusedInputField && focusedInputField != field) {
                focusedInputField->isActive = false; 
            }
            field->isActive = true;
            focusedInputField = field;
            fieldClickedThisTurn = true;
            std::cout << "Clicked input field. Placeholder: " << field->placeholder << ". Active: " << field->isActive << std::endl; 
            break; 
        }
    }
     if (!fieldClickedThisTurn && focusedInputField) { 
        bool clickedOnAnyButton = false;
        for (const auto& button : buttons) { if (mouseX >= button.rect.x && mouseX <= button.rect.x + button.rect.w && mouseY >= button.rect.y && mouseY <= button.rect.y + button.rect.h) { clickedOnAnyButton = true; break; }}
        if(!clickedOnAnyButton && focusedInputField->isActive) { 
             std::cout << "Clicked outside active field " << focusedInputField->placeholder << " and not on a button. Deactivating." << std::endl;
             focusedInputField->isActive = false; 
        }
    }

    for (const auto& button : buttons) {
        if (mouseX >= button.rect.x && mouseX <= button.rect.x + button.rect.w &&
            mouseY >= button.rect.y && mouseY <= button.rect.y + button.rect.h) {
            
            if(focusedInputField) {
                std::cout << "Button '" << button.text << "' clicked, deactivating input field: " << focusedInputField->placeholder << std::endl;
                focusedInputField->isActive = false;
                focusedInputField = nullptr; 
            }

            if (currentState == STATE_USER_DETAILS && button.actionId == 0) { 
                if(currentUser) { currentUser->setName(userNameInput.text); currentUser->setEmail(userEmailInput.text); currentUser->setPhoneNumber(userPhoneInput.text); currentUser->setCompanyOrSchool(userCompanyInput.text); }
                if (!userNameInput.text.empty() && !userEmailInput.text.empty()){ currentState = STATE_MAIN_MENU; currentMessage = ""; 
                } else { currentMessage = "Name and Email are required."; }
                return; 
            }
            else if (currentState == STATE_CREATE_EVENT_TYPE && (button.actionId >=0 && button.actionId <=2) ) { 
                if (button.actionId == 0) pendingEventType = Webinar; else if (button.actionId == 1) pendingEventType = Conference; else if (button.actionId == 2) pendingEventType = Workshop;
                clear_input_fields_for_create_event(); 
                currentState = STATE_CREATE_EVENT_DETAILS; currentMessage = "";
                focusedInputField = &eventTitleInput; if(focusedInputField) focusedInputField->isActive = true; 
                return; 
            }
            else if (currentState == STATE_CREATE_EVENT_DETAILS && button.actionId == 0) { 
                if(eventManager && currentUser) { currentMessage = eventManager->createEvent(currentUser, pendingEventType, eventTitleInput.text, eventDescInput.text, eventDateInput.text, eventPlatformInput.text, eventCapacityInput.text);
                } else { currentMessage = "Error: System not initialized."; }
                if (currentMessage.find("successfully") != string::npos) { clear_input_fields_for_create_event(); } 
                return;
            }
            else if (currentState == STATE_DELETE_EVENT && button.actionId == 0) { 
                bool fd, fs; if(eventManager) currentMessage = eventManager->attemptDeleteEvent(inputBuffer_generic, fd, fs); else { currentMessage = "Error: System not initialized."; fd=false; fs=false;}
                if (fd) clear_generic_input_buffer(); else if (fs) { actionToConfirm = 0; currentState = STATE_CONFIRM_ACTION;} return; 
            }
            else if (currentState == STATE_SIGN_UP_FOR_EVENT && button.actionId == 0) { 
                bool fd, fs; string sMsg; if(eventManager && currentUser) currentMessage = eventManager->attemptSignUp(currentUser, inputBuffer_generic, fd, fs, sMsg); else { currentMessage = "Error: System not initialized for sign up."; fd=false; fs=false;}
                if (fd) { currentMessage = sMsg; clear_generic_input_buffer();} else if (fs) { actionToConfirm = 1; currentState = STATE_CONFIRM_ACTION;} return; 
            }
            else if (currentState == STATE_SEARCH_EVENT && button.actionId == 0) { 
                if(eventManager) displayedEventList = eventManager->searchEvents(inputBuffer_generic); else displayedEventList.assign(1,"Error: Sys init.");
                currentState = STATE_SEARCH_RESULTS; currentMessage = ""; return;
            }
            else if (currentState == STATE_CONFIRM_ACTION) {
                if (button.actionId == 10) { 
                    if (actionToConfirm == 0) { if(eventManager) currentMessage = eventManager->confirmDeleteSuggestedEvent(); else currentMessage = "Err: Sys init."; } 
                    else if (actionToConfirm == 1) { string sMsg; if(eventManager && currentUser) currentMessage = eventManager->confirmSignUpSuggestedEvent(currentUser, sMsg); else currentMessage = "Err: Sys init."; if (!sMsg.empty() && currentMessage.find("Did you mean") == string::npos && currentMessage.find("Error:") == string::npos) currentMessage = sMsg; }
                    clear_generic_input_buffer(); reset_confirmation_state(); 
                } else if (button.actionId == 11) { currentMessage = "Action cancelled for '" + suggestedMatch +"'."; clear_generic_input_buffer(); reset_confirmation_state(); 
                } else if (button.actionId == 99) { currentState = STATE_MAIN_MENU; currentMessage = "Confirmation cancelled."; clear_generic_input_buffer(); reset_confirmation_state(); } 
                return;
            }
            else if (button.actionId == 9902) { 
                std::cout << "Back (9902) from CR_EV_TYP. State before: " << currentState << std::endl; currentState = STATE_MAIN_MENU; std::cout << "New state: " << currentState << std::endl;
                currentMessage = ""; clear_input_fields_for_create_event(); clear_generic_input_buffer(); reset_confirmation_state(); return; 
            }
            else if (button.actionId == 99) { 
                std::cout << "Back (99 GEN) State BEFORE: " << currentState << std::endl;
                if (currentState == STATE_CREATE_EVENT_DETAILS) { currentState = STATE_CREATE_EVENT_TYPE;} 
                else if (currentState == STATE_SEARCH_RESULTS) { currentState = STATE_SEARCH_EVENT;} 
                else { currentState = STATE_MAIN_MENU; } 
                std::cout << "Back (99 GEN) State AFTER: " << currentState << std::endl;
                currentMessage = ""; clear_input_fields_for_create_event(); clear_generic_input_buffer(); reset_confirmation_state(); return;
            }
            else if (button.actionId == 100) { // Exit button
                std::cout << "Exit button. Transitioning to EXITED state." << std::endl;
                if (eventManager) { bool saved = eventManager->saveEventsToFile(); std::cout << "Events save on exit: " << (saved ? "OK" : "FAIL") << std::endl; }
                currentState = STATE_EXITED; currentMessage = "";  return;
            }

            if (button.actionId == -1 && currentState != button.targetState) { 
                 std::cout << "Default nav (-1): From " << currentState << " To " << button.targetState << std::endl; 
                 currentState = button.targetState;
                 currentMessage = ""; 
                 reset_confirmation_state(); 
                 clear_generic_input_buffer(); 
                 
                 if (currentState == STATE_SHOW_ALL_EVENTS) { if(eventManager) displayedEventList = eventManager->getAllEventTitles();}
                 else if (currentState == STATE_CREATE_EVENT_TYPE || currentState == STATE_CREATE_EVENT_DETAILS){ clear_input_fields_for_create_event();}
                 else if (currentState == STATE_SEARCH_EVENT || currentState == STATE_SIGN_UP_FOR_EVENT || currentState == STATE_DELETE_EVENT) {
                    std::cout << "Setting focus to sharedInputField for state: " << currentState << std::endl;
                    focusedInputField = &sharedInputField; 
                    sharedInputField.isActive = true; 
                 }
            }
            return; 
        }
    }
}

void main_loop_iteration() {
    SDL_Event e; vector<Button> currentButtonsOnScreen; activeInputFieldsOnScreen.clear(); 
    int mX_poll=0, mY_poll=0; bool mouseClickedThisFrame = false;

    while (SDL_PollEvent(&e) != 0) {
        if (currentState == STATE_EXITED) { if (e.type == SDL_QUIT) {
            #ifdef __EMSCRIPTEN__
            emscripten_cancel_main_loop(); 
            #endif
        } continue; }

        if (e.type == SDL_QUIT) {
            std::cout << "SDL_QUIT. Saving and exiting." << std::endl; if (eventManager) eventManager->saveEventsToFile();
            currentState = STATE_EXITED; currentMessage = ""; continue; 
        }
        if (e.type == SDL_MOUSEBUTTONDOWN && e.button.button == SDL_BUTTON_LEFT) { mX_poll = e.button.x; mY_poll = e.button.y; mouseClickedThisFrame = true; }

        if (focusedInputField && focusedInputField->isActive) { 
             if (e.type == SDL_TEXTINPUT || (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_BACKSPACE)) {
                 std::cout << "Key/Text Event. Focused field: " << focusedInputField->placeholder 
                           << ", Active: " << focusedInputField->isActive 
                           << ", Text BEFORE: '" << focusedInputField->text; 
             }
            if (e.type == SDL_TEXTINPUT) { 
                string new_text_input = e.text.text;
                if (!new_text_input.empty()) { 
                    if (focusedInputField->text.length() + new_text_input.length() <= (size_t)focusedInputField->maxLength) {
                        focusedInputField->text += new_text_input; 
                    }
                }
            } 
            else if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_BACKSPACE && !focusedInputField->text.empty()) { 
                focusedInputField->text.pop_back(); 
            }
            if (e.type == SDL_TEXTINPUT || (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_BACKSPACE)) {
                 std::cout << "', Text AFTER: '" << focusedInputField->text 
                           << "', AppState: " << currentState << std::endl;
                 if (focusedInputField == &sharedInputField) {
                     inputBuffer_generic = sharedInputField.text;
                     std::cout << "   Synced sharedInputField.text to inputBuffer_generic: '" << inputBuffer_generic << "'" << std::endl;
                 }
             }
        } else if ( (e.type == SDL_TEXTINPUT || (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_BACKSPACE)) && 
                    (currentState == STATE_SEARCH_EVENT || currentState == STATE_SIGN_UP_FOR_EVENT || currentState == STATE_DELETE_EVENT || 
                     currentState == STATE_USER_DETAILS || currentState == STATE_CREATE_EVENT_DETAILS) ) 
        { 
            std::cout << "Key/Text Event occurred BUT ";
            if (!focusedInputField) std::cout << "focusedInputField is NULL.";
            else std::cout << "focusedInputField (" << focusedInputField->placeholder << ") is NOT active.";
            std::cout << " AppState: " << currentState << std::endl;
        }
    }
    
    int mX_curr, mY_curr; SDL_GetMouseState(&mX_curr, &mY_curr);
    SDL_SetRenderDrawColor(gRenderer, 255, 255, 255, 255); SDL_RenderClear(gRenderer);
    render_text("Event Management System", 10, 10, TEXT_COLOR, true, SCREEN_WIDTH - 20);
    
    if (currentState == STATE_EXITED) {
        render_text("Program Exited", 0, SCREEN_HEIGHT / 2 - 50, TEXT_COLOR, true, SCREEN_WIDTH);
        render_text("All data should be saved. You can close this browser tab.", 0, SCREEN_HEIGHT / 2, TEXT_COLOR, true, SCREEN_WIDTH - 40);
        SDL_RenderPresent(gRenderer); return; 
    }

    if (!currentMessage.empty()) { SDL_Color mc = TEXT_COLOR; if (currentMessage.find("Error:") != string::npos || currentMessage.find("Sorry!") != string::npos || currentMessage.find("not found") != string::npos) mc = ERROR_TEXT_COLOR; else if (currentMessage.find("successfully") != string::npos || currentMessage.find("Signed up") != string::npos) mc = SUCCESS_TEXT_COLOR; render_text(currentMessage, 20, SCREEN_HEIGHT - 60, mc , true, SCREEN_WIDTH - 40); }
    
    if (currentState == STATE_USER_DETAILS) { 
        render_text("Enter Your Details:", 50,100,TEXT_COLOR,false,SCREEN_WIDTH-100); activeInputFieldsOnScreen={&userNameInput,&userEmailInput,&userPhoneInput,&userCompanyInput};
        if((!focusedInputField || !focusedInputField->isActive || std::find(activeInputFieldsOnScreen.begin(), activeInputFieldsOnScreen.end(), focusedInputField) == activeInputFieldsOnScreen.end()) && !activeInputFieldsOnScreen.empty()){
            if(focusedInputField) focusedInputField->isActive = false;
            focusedInputField=activeInputFieldsOnScreen[0]; 
            if(focusedInputField)focusedInputField->isActive=true;
        }
        for(auto f:activeInputFieldsOnScreen)render_input_field(*f); currentButtonsOnScreen.emplace_back(SCREEN_WIDTH/2-100,400,200,40,"Submit Details",STATE_MAIN_MENU,0);
    }
    else if (currentState == STATE_MAIN_MENU) { 
        render_text("Main Menu",50,60,TEXT_COLOR); int by=100,bs=50;
        currentButtonsOnScreen.emplace_back(SCREEN_WIDTH/2-110,by,220,40,"1. Create Event",STATE_CREATE_EVENT_TYPE,-1);by+=bs;
        currentButtonsOnScreen.emplace_back(SCREEN_WIDTH/2-110,by,220,40,"2. Delete Event",STATE_DELETE_EVENT,-1);by+=bs;
        currentButtonsOnScreen.emplace_back(SCREEN_WIDTH/2-110,by,220,40,"3. Show All Events",STATE_SHOW_ALL_EVENTS,-1);by+=bs;
        currentButtonsOnScreen.emplace_back(SCREEN_WIDTH/2-110,by,220,40,"4. Sign Up for Event",STATE_SIGN_UP_FOR_EVENT,-1);by+=bs;
        currentButtonsOnScreen.emplace_back(SCREEN_WIDTH/2-110,by,220,40,"5. Search Events",STATE_SEARCH_EVENT,-1);by+=bs;
        currentButtonsOnScreen.emplace_back(SCREEN_WIDTH/2-110,by,220,40,"6. Exit",currentState,100);
    }
    else if (currentState == STATE_CREATE_EVENT_TYPE) { 
        render_text("Select Event Type:",50,100,TEXT_COLOR,false,SCREEN_WIDTH-100);
        currentButtonsOnScreen.emplace_back(SCREEN_WIDTH/2-75,150,150,40,"Webinar",STATE_CREATE_EVENT_DETAILS,0);
        currentButtonsOnScreen.emplace_back(SCREEN_WIDTH/2-75,200,150,40,"Conference",STATE_CREATE_EVENT_DETAILS,1);
        currentButtonsOnScreen.emplace_back(SCREEN_WIDTH/2-75,250,150,40,"Workshop",STATE_CREATE_EVENT_DETAILS,2);
        currentButtonsOnScreen.emplace_back(SCREEN_WIDTH/2-50,350,100,40,"Back",STATE_MAIN_MENU,9902);
    }
    else if (currentState == STATE_CREATE_EVENT_DETAILS) { 
        string ts="Enter Event Details for "; if(pendingEventType==Webinar)ts+="Webinar";else if(pendingEventType==Conference)ts+="Conference";else ts+="Workshop"; render_text(ts,50,100,TEXT_COLOR,false,SCREEN_WIDTH-100);
        activeInputFieldsOnScreen={&eventTitleInput,&eventDescInput,&eventDateInput,&eventPlatformInput,&eventCapacityInput};
        if((!focusedInputField || !focusedInputField->isActive || std::find(activeInputFieldsOnScreen.begin(), activeInputFieldsOnScreen.end(), focusedInputField) == activeInputFieldsOnScreen.end()) && !activeInputFieldsOnScreen.empty()){
            if(focusedInputField) focusedInputField->isActive = false;
            focusedInputField=activeInputFieldsOnScreen[0]; 
            if(focusedInputField)focusedInputField->isActive=true;
        }
        for(auto f:activeInputFieldsOnScreen)render_input_field(*f);
        currentButtonsOnScreen.emplace_back(SCREEN_WIDTH/2-160,400,150,40,"Create Event",currentState,0);
        currentButtonsOnScreen.emplace_back(SCREEN_WIDTH/2+10,400,100,40,"Back",STATE_CREATE_EVENT_TYPE,99);
    }
    else if (currentState == STATE_SHOW_ALL_EVENTS) { 
        render_text("All Events:",50,60,TEXT_COLOR,false,SCREEN_WIDTH-100); int yp=100;
        if(!eventManager||displayedEventList.empty()||(displayedEventList.size()==1&&displayedEventList[0].find("No events")!=string::npos)){render_text((eventManager&&displayedEventList.empty())?"No events created.":(displayedEventList.empty()?"System Error.":displayedEventList[0]),50,yp,TEXT_COLOR,false,SCREEN_WIDTH-100);}
        else{for(const auto&t:displayedEventList){render_text("- "+t,50,yp,TEXT_COLOR,false,SCREEN_WIDTH-60);if(gFont)yp+=TTF_FontHeight(gFont)+5;else yp+=20; if(yp>SCREEN_HEIGHT-100){render_text("...more (not shown).",50,yp,TEXT_COLOR);break;}}}
        currentButtonsOnScreen.emplace_back(50,SCREEN_HEIGHT-80,100,40,"Back",STATE_MAIN_MENU,99);
    }
    else if (currentState == STATE_DELETE_EVENT || currentState == STATE_SIGN_UP_FOR_EVENT || currentState == STATE_SEARCH_EVENT) {
        string titleText, buttonText;
        if (currentState == STATE_DELETE_EVENT) { titleText = "Enter title of event to DELETE:"; buttonText = "Delete"; sharedInputField.placeholder = "Title to Delete"; }
        else if (currentState == STATE_SIGN_UP_FOR_EVENT) { titleText = "Enter title of event to SIGN UP for:"; buttonText = "Sign Up"; sharedInputField.placeholder = "Title for Sign Up"; }
        else { titleText = "Enter event title to SEARCH:"; buttonText = "Search"; sharedInputField.placeholder = "Title to Search"; }
        render_text(titleText, 50, 100, TEXT_COLOR, false, SCREEN_WIDTH-100);
        
        activeInputFieldsOnScreen = {&sharedInputField}; 
        
        if (focusedInputField != &sharedInputField || !sharedInputField.isActive) {
            if(focusedInputField) focusedInputField->isActive = false; 
            focusedInputField = &sharedInputField;
            sharedInputField.isActive = true; 
            std::cout << "State " << currentState << ": Set focus to sharedInputField. Placeholder: " << sharedInputField.placeholder << std::endl;
        }
        render_input_field(sharedInputField);

        currentButtonsOnScreen.emplace_back(SCREEN_WIDTH/2-160,200,150,40,buttonText,currentState,0);
        currentButtonsOnScreen.emplace_back(SCREEN_WIDTH/2+10,200,100,40,"Back",STATE_MAIN_MENU,99);
    }
    else if (currentState == STATE_SEARCH_RESULTS) { 
        render_text("Search Results:",50,60,TEXT_COLOR,false,SCREEN_WIDTH-100); int yp=100;
        if(displayedEventList.empty()||(displayedEventList.size()==1&&displayedEventList[0].find("No events found")!=string::npos)){render_text(displayedEventList.empty()?"No results.":displayedEventList[0],50,yp,TEXT_COLOR,false,SCREEN_WIDTH-100);}
        else{for(size_t i=0;i<displayedEventList.size();++i){render_text(to_string(i+1)+". "+displayedEventList[i],50,yp,TEXT_COLOR,false,SCREEN_WIDTH-60);if(gFont)yp+=TTF_FontHeight(gFont)+5;else yp+=20; if(yp>SCREEN_HEIGHT-100){render_text("...more (not shown).",50,yp,TEXT_COLOR);break;}}}
        currentButtonsOnScreen.emplace_back(50,SCREEN_HEIGHT-80,100,40,"Back",STATE_SEARCH_EVENT,99);
    }
    else if (currentState == STATE_CONFIRM_ACTION) { 
        render_text("Confirm Action:",0,130,TEXT_COLOR,true,SCREEN_WIDTH);
        currentButtonsOnScreen.emplace_back(SCREEN_WIDTH/2-110,200,100,40,"Yes",currentState,10);
        currentButtonsOnScreen.emplace_back(SCREEN_WIDTH/2+10,200,100,40,"No",currentState,11);
        currentButtonsOnScreen.emplace_back(SCREEN_WIDTH/2-50,250,100,40,"Cancel",STATE_MAIN_MENU,99);
    }
    
    update_button_hover(mX_curr, mY_curr, currentButtonsOnScreen); 
    if (mouseClickedThisFrame) { handle_mouse_click(mX_poll, mY_poll, currentButtonsOnScreen, activeInputFieldsOnScreen); }
    for (auto& btn : currentButtonsOnScreen) { render_button(btn); }
    SDL_RenderPresent(gRenderer);
}

// --- Helper function definitions ---
void clear_input_fields_for_create_event() { 
    eventTitleInput.text = ""; 
    eventDescInput.text = ""; 
    eventDateInput.text = ""; 
    eventPlatformInput.text = ""; 
    eventCapacityInput.text = ""; 
}
void clear_generic_input_buffer(){ 
    inputBuffer_generic = ""; 
    sharedInputField.text = ""; 
}
void reset_confirmation_state() { 
    suggestedMatch = ""; 
    actionToConfirm = -1; 
}


bool init_sdl() { 
    if(SDL_Init(SDL_INIT_VIDEO|SDL_INIT_EVENTS)<0){cerr<<"SDL Init Fail: "<<SDL_GetError()<<endl;return false;}
    gWindow=SDL_CreateWindow("Event GUI",SDL_WINDOWPOS_UNDEFINED,SDL_WINDOWPOS_UNDEFINED,SCREEN_WIDTH,SCREEN_HEIGHT,SDL_WINDOW_SHOWN);
    if(!gWindow){cerr<<"Window Fail: "<<SDL_GetError()<<endl;return false;}
    gRenderer=SDL_CreateRenderer(gWindow,-1,SDL_RENDERER_ACCELERATED|SDL_RENDERER_PRESENTVSYNC);
    if(!gRenderer){cerr<<"Renderer Fail: "<<SDL_GetError()<<endl;SDL_DestroyWindow(gWindow);gWindow=nullptr;return false;}
    if(TTF_Init()==-1){cerr<<"TTF Init Fail: "<<TTF_GetError()<<endl;SDL_DestroyRenderer(gRenderer);gRenderer=nullptr;SDL_DestroyWindow(gWindow);gWindow=nullptr;return false;}
    
    gFont = TTF_OpenFont("arial.ttf", 18);
    if (!gFont) {
        #ifndef __EMSCRIPTEN__
        std::cout << "arial.ttf not found, trying fallback fonts for native build..." << std::endl;
        string fb_paths[] = { 
            "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf",
            "C:/Windows/Fonts/Arial.ttf",
            "/System/Library/Fonts/Supplemental/Arial.ttf"
        };
        for (const string& p : fb_paths) {
            gFont = TTF_OpenFont(p.c_str(), 18);
            if (gFont) {
                cout << "Loaded font from fallback: " << p << endl;
                break;
            }
        }
        #endif 
        if (!gFont) { 
            cerr << "Failed to load font 'arial.ttf' or any fallback. SDL_ttf Error: " << TTF_GetError() << endl;
        }
    }
    SDL_StartTextInput(); return true;
}
void close_sdl() { 
    SDL_StopTextInput(); if(gFont)TTF_CloseFont(gFont);gFont=nullptr; if(gRenderer)SDL_DestroyRenderer(gRenderer);gRenderer=nullptr; if(gWindow)SDL_DestroyWindow(gWindow);gWindow=nullptr; TTF_Quit(); SDL_Quit();
}
#ifdef __EMSCRIPTEN__
extern "C" { void filesystem_ready_callback() { std::cout << "C++: Filesystem ready callback." << std::endl; } }
#endif
int main(int argc, char* argv[]) {
    if (!init_sdl()) { close_sdl(); return 1; }
    #ifdef __EMSCRIPTEN__ 
    EM_ASM(
        if (typeof FS !=='undefined'&&FS.mkdir&&FS.mount&&FS.syncfs&&typeof IDBFS !=='undefined') {
            const idbfsP = '/database_eventmgm'; FS.mkdir(idbfsP); FS.mount(IDBFS,{},idbfsP);
            console.log('Attempting FS.syncfs for ' + idbfsP);
            FS.syncfs(true, function(err){ if(err)console.error("FS.syncfs load err for "+idbfsP+":",err); else console.log("FS.syncfs load OK for "+idbfsP); });
        } else console.warn("FS/IDBFS not fully available for persistence.");
    );
    #endif
    currentUser = User::getInstance(); eventManager = new events(); 
    if (currentState == STATE_USER_DETAILS ) { focusedInputField = &userNameInput; if(focusedInputField)focusedInputField->isActive = true;}


    #ifdef __EMSCRIPTEN__
        emscripten_set_main_loop(main_loop_iteration, 0, 1);
    #else
        bool running_native = true; SDL_Event event_native; Uint32 last_tick = SDL_GetTicks();
        while (running_native) { 
            Uint32 current_tick = SDL_GetTicks(); if (current_tick - last_tick < 16) SDL_Delay(16 - (current_tick - last_tick)); last_tick = current_tick;
            while(SDL_PollEvent(&event_native)!=0){ if(event_native.type == SDL_QUIT){ running_native=false; if(eventManager && currentState != STATE_EXITED)eventManager->saveEventsToFile();}} 
            if (!running_native && currentState != STATE_EXITED) { currentState = STATE_EXITED; } 
            main_loop_iteration(); if (currentState == STATE_EXITED && running_native) running_native = false; 
        }
    #endif
    cout << "Exiting main. Cleaning up..." << endl; 
    if (eventManager) { delete eventManager; eventManager = nullptr; } 
    close_sdl(); cout << "SDL closed. Terminated." << endl;
    return 0;
}

