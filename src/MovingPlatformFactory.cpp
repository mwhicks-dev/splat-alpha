#include "model/GameObjectModel.h"
#include "model/MovingPlatformFactory.h"
#include "model/MovingPlatform.h"
#include "Client.h"

#ifdef DEBUG
#include <iostream>
#endif

using namespace SPlat::Model;

Asset& MovingPlatformFactory::create_asset(AssetProperties& properties) {
    properties.set_collision_priority(-1);
    // create new moving and movingplatform properties
    MovingProperties moving_properties(sf::Vector2f(0, 0), 
        Client::get_instance().get_config().get_timing_config().get_display_timeline().get_time());
    MovingPlatformProperties moving_platform_properties(-1, {});

    MovingPlatform * mplat = new MovingPlatform(
        *new AssetProperties(properties), 
        *new MovingProperties (moving_properties), 
        *new MovingPlatformProperties(moving_platform_properties));

    return GameObjectModel::get_instance().create_asset(*mplat);
}
