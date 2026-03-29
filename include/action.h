#ifndef ACTION_H
#define ACTION_H

#include <string>

class Screen;
class U8G2;

class Action {
public:
    std::string name;
    const uint8_t *icon;

    Action(std::string name, const uint8_t *icon = nullptr);
    virtual void execute() = 0;
};

class DummyAction: public Action {
public:
    DummyAction(std::string name, const uint8_t *icon = nullptr);
    void execute() override;
};

class FunctionAction: public Action {
private:
    void (*action_ptr)(void);
public:
    FunctionAction(std::string name, void (*action_ptr)(void), const uint8_t *icon = nullptr);
    void execute() override;
};

class OpenScreenAction: public Action {
private:
    Screen *screen;
public:
    OpenScreenAction(std::string name, Screen *screen, const uint8_t *icon = nullptr);
    void execute() override;
};

#endif
