#ifndef SPlat_Events_DeleteAssetHandler_h
#define SPlat_Events_DeleteAssetHandler_h

#include "events/CommandHandlerInterface.h"

#include <cereal/cereal.hpp>

namespace SPlat {

    namespace Events {

        class DeleteAssetHandler : public CommandHandlerInterface {

        public:

            struct Args {

                size_t id;

                template <class Archive>
                void serialize(Archive& ar) {
                    ar(id);
                }

            };

            void handle(std::string) override;

            static std::string get_type() { return "delete_asset"; }

        };

    }

}

#endif
