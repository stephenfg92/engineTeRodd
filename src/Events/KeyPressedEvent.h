#pragma once

#include <raylib.h>

#include "../ECS/ECS.h"
#include "Event.h"

class KeyPressedEvent: public Event {
    public:
        int key;

        KeyPressedEvent(int key): key(key) {}

        int getKeycode(){
            return key;
        }
};