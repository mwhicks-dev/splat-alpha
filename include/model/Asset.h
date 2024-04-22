#ifndef SPLAT_ASSET
#define SPLAT_ASSET

#include <SFML/Graphics.hpp>

#include "Serialization.h"

namespace SPlat {

    namespace Model {

        /// @brief communicable properties to use for Asset events
        struct AssetProperties {

            /// @brief asset position vector
            sf::Vector2f position;

            /// @brief asset size vector
            sf::Vector2f size;

            /// @brief string type of asset
            std::string type;

            /// @brief convert AssetProperties to serializable string
            /// @tparam Archive cereal archive class
            /// @param ar Archive to use for AssetProperties conversion
            template <class Archive>
            void serialize(Archive& ar) {
                ar(position, size, type);
            }
        };

        class Asset : public sf::RectangleShape {

        public:

            size_t id;

            Asset * standing_on;

            std::unordered_set<size_t> standers;

            sf::Vector2f velocity;

            Asset(sf::Vector2f&);

            virtual int get_priority() = 0;

            virtual std::string get_type() = 0;

            virtual void update();

            AssetProperties get_properties() {
                return {getPosition(), getSize(), get_type()};
            }

        };

    }

}

#endif