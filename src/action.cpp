#include <action.h>
#include <screen.h>

extern void open_screen(Screen*, bool forced=false);

Action::Action(std::string name, const uint8_t *icon)
    : name(name), icon(icon) {};

DummyAction::DummyAction(std::string name, const uint8_t *icon) : Action(name, icon) {};

void DummyAction::execute() {};

FunctionAction::FunctionAction(std::string name, void (*action_ptr)(void), const uint8_t *icon)
    : Action(name, icon), action_ptr(action_ptr) {}

void FunctionAction::execute() {
    action_ptr();
}

OpenScreenAction::OpenScreenAction(std::string name, Screen *screen, const uint8_t *icon)
    : Action(name, icon), screen(screen) {}

void OpenScreenAction::execute() {
    open_screen(screen);
}
