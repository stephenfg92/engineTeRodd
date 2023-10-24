#pragma once

#include "../ECS/ECS.h"
#include "../Events/Event.h"


class ProjectileRequestEvent: public Event {
    public:
        Entity entity;
        ProjectileRequestEvent(Entity entity): entity(entity) {}
};