#include "events/Listener.h"

#include "events/AssetEvents.h"
#include "events/CharacterEvents.h"
#include "events/KeyEvents.h"
#include "events/MovingPlatformEvents.h"

using namespace SPlat::Events;

Listener::Listener() {
    set_handler(AddPositionEvent::get_type(), AddPositionEvent::handler);
    set_handler(AddVelocityEvent::get_type(), AddVelocityEvent::handler);
    set_handler(ControlAssetEvent::get_type(), ControlAssetEvent::handler);
    set_handler(CreateCharacterEvent::get_type(), CreateCharacterEvent::handler);
    set_handler(CreateControlCharacterEvent::get_type(), CreateControlCharacterEvent::handler);
    set_handler(CreateMovingPlatformEvent::get_type(), CreateMovingPlatformEvent::handler);
    set_handler(KeyPressEvent::get_type(), KeyPressEvent::handler);
    set_handler(KeyReleaseEvent::get_type(), KeyReleaseEvent::handler);
}

void Listener::dispatch(Command cmd) {
    // get handler string
    bool set = false; 
    void (*handler)(std::string);

    // attempt to retrieve set handler
    handlers_lock.lock();
    set = handlers.count(cmd.type) > 0;
    if (set) handler = handlers[cmd.type];
    handlers_lock.unlock();

    // use set if exists
    if (set) { handler(cmd.args); return; }

    throw std::invalid_argument("No such event class " + cmd.type);
}

void Listener::run() {
    while (true) {
        // break if no events to process
        commands_lock.lock();
        if (commands.empty()) {
            commands_lock.unlock();
            break;
        }
        // dequeue event if nonempty
        Command cmd = commands.top(); commands.pop();
        commands_lock.unlock();

        // dispatch event
        dispatch(cmd);
    }
}

void Listener::push_command(Command cmd) {
    commands_lock.lock();
    commands.push(cmd);
    commands_lock.unlock();
}

void Listener::set_handler(std::string type, void (*handler)(std::string)) {
    handlers_lock.lock();
    handlers[type] = handler;
    handlers_lock.unlock();
}
