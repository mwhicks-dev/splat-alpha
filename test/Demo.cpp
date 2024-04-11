#include "Client.h"
#include "model/Character.h"
#include "utilities/Functions.h"

using namespace SPlat;

int main() {
    // Create asset with dispatched event
    SPlat::Events::CreateAssetEvent event({
        sf::Vector2f(100, 100), // position
        sf::Vector2f(50, 100),  // size
        SPlat::Model::Character::TYPE  // type
    });

    Utilities::set_default_handlers();

    Client client; client.start();
}
