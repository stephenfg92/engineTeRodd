#pragma once

#include <raylib.h>

#include "../ECS/ECS.h"
#include "Event.h"

#include "../Components/ParticleComponent.h"

class AplicarForcaEvent: public Event {
    public:
        ParticleComponent particula;
        Vector2 forca;
        AplicarForcaEvent(ParticleComponent particula, Vector2 forca): particula(particula), forca(forca) {}
};