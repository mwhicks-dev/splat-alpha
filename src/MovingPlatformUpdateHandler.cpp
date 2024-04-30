#include "model/handler/MovingPlatformUpdateHandler.h"
#include "Client.h"

#ifdef DEBUG
#include <iostream>
#endif

using namespace SPlat::Model;

AssetProperties& MovingPlatformUpdateHandler::get_asset_properties() {
    m.lock();
    AssetProperties& local = asset;
    m.unlock();

    return local;
}

MovingProperties& MovingPlatformUpdateHandler::get_moving_properties() {
    m.lock();
    MovingProperties& local = moving;
    m.unlock();

    return local;
}

MovingPlatformProperties& MovingPlatformUpdateHandler::get_moving_platform_properties() {
    m.lock();
    MovingPlatformProperties& local = moving_platform;
    m.unlock();

    return local;
}

void MovingPlatformUpdateHandler::update() {
    ConfigInterface& conf = Client::get_instance().get_config();
    AssetProperties& asset_properties = get_asset_properties();
    MovingProperties& moving_properties = get_moving_properties();
    MovingPlatformProperties& moving_platform_properties = get_moving_platform_properties();

    // check if state change needed
    {
        std::vector<State> states = moving_platform_properties.get_states();
        if (states.size() > 0 && conf.get_timing_config().get_anchor_timeline()
                .get_time() > moving_platform_properties
                .get_last_state_change() + states[0].anchor_steps) {
            if (states[0].repeat) states.push_back(states[0]);
            states.erase(states.begin());
            moving_platform_properties.set_last_state_change(conf
                .get_timing_config().get_anchor_timeline().get_time());
        }
        moving_platform_properties.set_states(states);
    }
    
    // set velocity to current state velocity
    {
        std::vector<State> states = moving_platform_properties.get_states();
        sf::Vector2f velocity(0, 0);
        if (states.size() > 0) {
            velocity = states[0].velocity;
        }
        moving_properties.set_velocity(velocity);
    }

    // get velocity
    sf::Vector2f update_velocity;
    {
        float dt = static_cast<float>(conf.get_timing_config()
            .get_display_timeline().get_time() - moving_properties
            .get_last_updated()) / static_cast<float>(conf.get_environment()
            .get_framerate_limit());
        update_velocity = moving_properties.get_velocity() * dt;
    }

    // update self and children
    {
        asset_properties.set_position(asset_properties.get_position() + update_velocity);
        conf.get_environment().get_standing_config().push_update_to_children(
            asset_properties.get_id(), update_velocity);
    }
}
