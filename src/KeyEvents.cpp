#include "events/KeyEvents.h"
#include "events/Command.h"
#include "events/Listener.h"

#include <cereal/archives/json.hpp>

#include <sstream>

#ifdef DEBUG
#include <iostream>
#endif

using namespace SPlat::Events;

std::mutex KeyEvent::keys_held_lock;
std::set<sf::Keyboard::Key> KeyEvent::keys_held;

KeyEvent::KeyEvent(sf::Keyboard::Key key) {
    this->key = key;
}

bool KeyEvent::is_key_pressed(sf::Keyboard::Key key) {
    bool local = false;
    keys_held_lock.lock();
    if (keys_held.count(key) > 0)
        local = true;
    keys_held_lock.unlock();

    return local;
}

void KeyPressEvent::handler(std::string serialized) {
#ifdef DEBUG
    std::cout << "-> KeyPressEvent::handler(" << serialized << ")" << std::endl;
#endif
    // deserialize args from JSON string
    Args args;
    {
        std::stringstream ss; ss << serialized;
        cereal::JSONInputArchive iar(ss);
        iar(args);
    }

    // add keyboard key to pressed keys
    keys_held_lock.lock();
    keys_held.insert(args.key);
    keys_held_lock.unlock();
#ifdef DEBUG
    std::cout << "<- KeyPressEvent::handler" << std::endl;
#endif
}

void KeyPressEvent::raise() {
#ifdef DEBUG
    std::cout << "-> KeyPressEvent::raise()" << std::endl;
#endif
    // serialize args to JSON string
    Args args = {.key=key};
    std::stringstream ss;
    {
        cereal::JSONOutputArchive oar(ss);
        oar(args);
    }

    // create new command
    Command cmd = {
        .priority=-1,
        .type=KeyPressEvent::get_type(),
        .args=ss.str()
    };

    // send to foreground listener
    ForegroundListener::get_instance().push_command(cmd);
#ifdef DEBUG
    std::cout << "<- KeyPressEvent::raise" << std::endl;
#endif
}

void KeyReleaseEvent::handler(std::string serialized) {
#ifdef DEBUG
    std::cout << "-> KeyReleaseEvent::handler(" << serialized << ")" << std::endl;
#endif
    // deserialize args from JSON string
    Args args;
    {
        std::stringstream ss; ss << serialized;
        cereal::JSONInputArchive iar(ss);
        iar(args);
    }

    // remove keyboard key from pressed keys
    keys_held_lock.lock();
    keys_held.erase(args.key);
    keys_held_lock.unlock();
#ifdef DEBUG
    std::cout << "<- KeyReleaseEvent::handler" << std::endl;
#endif
}

void KeyReleaseEvent::raise() {
#ifdef DEBUG
    std::cout << "-> KeyReleaseEvent::raise()" << std::endl;
#endif
    // serialize args to JSON string
    Args args = {.key=key};
    std::stringstream ss;
    {
        cereal::JSONOutputArchive oar(ss);
        oar(args);
    }

    // create new command
    Command cmd = {
        .priority=-1,
        .type=KeyReleaseEvent::get_type(),
        .args=ss.str()
    };

    // send to foreground listener
    ForegroundListener::get_instance().push_command(cmd);
#ifdef DEBUG
    std::cout << "<- KeyReleaseEvent::raise" << std::endl;
#endif
}
