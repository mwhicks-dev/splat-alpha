#ifndef SPLAT_MODEL_UPDATEHANDLER_H
#define SPLAT_MODEL_UPDATEHANDLER_H

#include "model/MovingProperties.h"

namespace SPlat {

    namespace Model {

        class UpdateHandler {

            std::mutex m;

            MovingProperties * properties = nullptr;

        public:

            UpdateHandler() = default;

            void set_properties(MovingProperties* properties) { 
                m.lock();
                this->properties = properties;
                m.unlock();
            }

            MovingProperties * get_properties() {
                m.lock();
                MovingProperties * local = properties;
                m.unlock();

                return local;
            }

            virtual void update(time_t) = 0;

        };

    }

}

#endif