#include <action.h>
#include <screen.h>

ItemAction::ItemAction(void (*action)(void)) : action(action) {}

void ItemAction::execute() {
    action();
}

OpenScreenAction::OpenScreenAction(Screen *screen) : screen(screen) {}

extern void open_screen(Screen*);
void OpenScreenAction::execute() {
    open_screen(screen);
}
