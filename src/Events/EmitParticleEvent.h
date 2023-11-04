#pragma once

#include "../ECS/ECS.h"
#include "../Events/Event.h"

#include "../Components/TransformComponent.h"
#include "../Components/PlayerParticleEmitterComponent.h"
#include "../Components/SpriteComponent.h"


class EmitParticleEvent: public Event {
    public:
        Registry* registry;
        TransformComponent transform;
        PlayerParticleEmitterComponent playerParticleEmitter;
        SpriteComponent sprite;
        
        EmitParticleEvent(Registry* registry, TransformComponent transform, PlayerParticleEmitterComponent playerParticleEmitter, SpriteComponent sprite): 
        registry(registry), transform(transform), playerParticleEmitter(playerParticleEmitter), sprite(sprite) {}
};