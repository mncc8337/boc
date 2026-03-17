#ifndef ACTION_H
#define ACTION_H

class Screen;
class U8G2;

class Action {
public:
    virtual void execute() = 0;
};

class ItemAction: public Action {
private:
    void (*action)(void);
public:
    ItemAction(void (*action)(void));
    void execute() override;
};

class OpenScreenAction: public Action {
private:
    Screen *screen;
public:
    OpenScreenAction(Screen *screen);
    void execute() override;
};

#endif
