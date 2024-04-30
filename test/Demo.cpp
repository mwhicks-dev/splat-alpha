#include "Client.h"
#include "ConfigInterface.h"
#include "model/Character.h"
#include "model/Platform.h"
#include "model/MovingPlatform.h"
#include "events/AssetEvents.h"
#include "events/CharacterEvents.h"
#include "events/MovingPlatformEvents.h"
#include "events/PlatformEvents.h"
#include "events/KeyEvents.h"
#include "events/Listener.h"

#include <cereal/archives/json.hpp>

using namespace SPlat;

/// @brief overriden keypress event handler with movement
/// @param serialized serialized KeyEventArgs
static void keypress_override(std::string serialized) {
    // set key held
    Events::KeyPressEvent::handler(serialized);

    // deserialize KeyEventArgs from args
    Events::KeyEvent::Args args;
    std::stringstream ss; ss << serialized;
    {
        cereal::JSONInputArchive iar(ss);
        iar(args);
    }

    if (args.key == sf::Keyboard::Key::Escape) {
        LocalTimeline& dtl = Client::get_instance().get_config().get_timing_config().get_display_timeline();
        if (dtl.get_paused()) {
            dtl.unpause();
        } else {
            dtl.pause();
        }
    }

    try {
        // get asset
        Model::Character* ctl = Client::get_instance().get_config()
            .get_environment().get_controlled_asset();
        if (ctl == nullptr) {
#ifdef DEBUG
            std::cout << "No asset set as controlled" << std::endl;
#endif
            return;
        }

        // update velocity based on key pressed
        if (args.key == sf::Keyboard::Key::Left) {
            ctl->get_moving_properties().set_velocity(ctl
                ->get_moving_properties().get_velocity() + sf::Vector2f(
                    Client::get_instance().get_config().get_environment().get_unit() * -10, 0));
        } else if (args.key == sf::Keyboard::Key::Right) {
            ctl->get_moving_properties().set_velocity(ctl
                ->get_moving_properties().get_velocity() + sf::Vector2f(
                    Client::get_instance().get_config().get_environment().get_unit() * 10, 0));
        } else if (args.key == sf::Keyboard::Key::Up 
                && ctl->get_character_properties().get_standing_on() != nullptr) {
            ctl->get_moving_properties().set_velocity(ctl
                ->get_moving_properties().get_velocity() + sf::Vector2f(
                    0, Client::get_instance().get_config().get_environment().get_unit() * -11.5));
#ifdef DEBUG
            sf::Vector2f updated_debug_velocity = ctl->get_moving_properties().get_velocity();
            std::cout << "new velocity: <" << updated_debug_velocity.x << ", " << updated_debug_velocity.y << ">" << std::endl;
#endif
        }

    } catch (std::logic_error & e) {
        std::cout << "key press handler: " << e.what() << std::endl;
    }
}

/// @brief overriden keyrelease event handler with movement
/// @param serialized serialized KeyEventArgs
static void keyrelease_override(std::string serialized) {
    // use default handler to unset key held
    Events::KeyReleaseEvent::handler(serialized);

    // deserialize KeyEventArgs from args
    Events::KeyEvent::Args args;
    std::stringstream ss; ss << serialized;
    {
        cereal::JSONInputArchive iar(ss);
        iar(args);
    }

    try {
        // get asset
        Model::Character* ctl = Client::get_instance().get_config()
            .get_environment().get_controlled_asset();
        if (ctl == nullptr) {
#ifdef DEBUG
            std::cout << "No asset set as controlled" << std::endl;
#endif
            return;
        }

        // update velocity based on key pressed
        if (args.key == sf::Keyboard::Key::Left) {
            ctl->get_moving_properties().set_velocity(ctl
                ->get_moving_properties().get_velocity() - sf::Vector2f(
                    Client::get_instance().get_config().get_environment().get_unit() * -10, 0));
        } else if (args.key == sf::Keyboard::Key::Right) {
            ctl->get_moving_properties().set_velocity(ctl
                ->get_moving_properties().get_velocity() - sf::Vector2f(
                    Client::get_instance().get_config().get_environment().get_unit() * 10, 0));
        }

    } catch (std::logic_error & e) {
        std::cout << e.what() << std::endl;
    }
}

int main() {
    ConfigInterface& conf = Client::get_instance().get_config();
    // Create assets
    {
        Model::AssetProperties properties(
            sf::Vector2f(100, 100),  // position
            sf::Vector2f(50, 100),  // size
            sf::Color::Magenta  // fill_color
        );
        Model::Asset& asset = conf.get_asset_factory_config().get_character_factory().create_asset(properties);
        Model::Character& character = dynamic_cast<Model::Character&>(asset);
        Client::get_instance().get_config().get_environment().set_controlled_asset(&character);
    }
    {
        Model::AssetProperties properties(
            sf::Vector2f(0, 500),  // position
            sf::Vector2f(400, 100),  // size
            sf::Color::Green  // fill_color
        );
        conf.get_asset_factory_config().get_platform_factory().create_asset(properties);
    }
    {
        Model::AssetProperties properties(
            sf::Vector2f(200, 300),  // position
            sf::Vector2f(150, 25),  // size
            sf::Color::White  // fill_color
        );
        Model::Asset& asset = conf.get_asset_factory_config().get_moving_platform_factory().create_asset(properties);
        Model::MovingPlatform& moving_platform = dynamic_cast<Model::MovingPlatform&>(asset);

        // add states
        Model::MovingPlatformProperties& moving_platform_properties = moving_platform.get_moving_platform_properties();
        std::vector<Model::State> states = moving_platform_properties.get_states();
        states.push_back(
            Model::State(
                sf::Vector2f(5, 0) * conf.get_environment().get_unit(),  // velocity
                1.5 * conf.get_timing_config().get_anchor_steps_per_second(),  // anchor_steps
                true  // repeat
            )
        );
        states.push_back(
            Model::State(
                sf::Vector2f(-15, 0) * conf.get_environment().get_unit(),  // velocity
                0.5 * conf.get_timing_config().get_anchor_steps_per_second(),  // anchor_steps
                true  // repeat
            )
        );
        moving_platform_properties.set_last_state_change(conf.get_timing_config().get_anchor_timeline().get_time());
        moving_platform_properties.set_states(states);
    }

    Events::ForegroundListener::get_instance().set_handler(Events::KeyPressEvent::get_type(), keypress_override);
    Events::ForegroundListener::get_instance().set_handler(Events::KeyReleaseEvent::get_type(), keyrelease_override);
    
    conf.get_environment().set_framerate_limit(90);

    Client::get_instance().start();
}
