#ifndef SPlat_Events_OrderedPriorityListener_h
#define SPlat_Events_OrderedPriorityListener_h

#include "ListenerInterface.h"

#include <queue>
#include <map>
#include <mutex>

namespace SPlat {

    namespace Events {

        class OrderedPriorityListener : public ListenerInterface {

            std::mutex m;

            std::priority_queue<Event> event_queue;

            std::map<std::string, CommandHandlerInterface*>& handlers;

            Event poll_event();

            CommandHandlerInterface* get_handler(std::string);

            bool command_available();

            void listener_loop();

        public:

            OrderedPriorityListener();

            void set_handler(std::string, CommandHandlerInterface&) override;

            void push_event(Event) override;

            void run() override;

            void await(Event) override;

        };

    }

}

#endif