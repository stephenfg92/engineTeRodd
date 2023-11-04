#pragma once

#include "../ECS/ECS.h"
#include "../Components/BoxColliderComponent.h"
#include "../Components/HealthComponent.h"
#include "../Components/ProjectileComponent.h"
#include "../Events/EventBus.h"
#include "../Events/CollisionEvent.h"

class DamageSystem: public System {
    public:
        DamageSystem() {
            RequireComponent<BoxColliderComponent>();
        }

    void SubscribeToEvents(std::unique_ptr<EventBus>& eventBus){
        eventBus->SubscribeToEvent<CollisionEvent>(this, &DamageSystem::OnCollision);
    }

    void OnCollision(CollisionEvent& event) {

        Entity a = event.a;
        Entity b = event.b;

        // Enemy Projectiles Colliding with Player
        if (a.HasTag(Tag::PLAYER) && b.HasTag(Tag::ENEMY_PROJECTILE)) {
            OnProjectileCollidesWithEntity(a, b);
        }
        if (b.HasTag(Tag::PLAYER) && a.HasTag(Tag::ENEMY_PROJECTILE)) {
            OnProjectileCollidesWithEntity(b, a);
        }

        // Player Projectiles Colliding with Enemies
        if (a.HasTag(Tag::ENEMY) && b.HasTag(Tag::PLAYER_PROJECTILE)) {
            OnProjectileCollidesWithEntity(a, b);
        }
        if (b.HasTag(Tag::ENEMY) && a.HasTag(Tag::PLAYER_PROJECTILE)) {
            OnProjectileCollidesWithEntity(b, a);
        }
    }

    void OnProjectileCollidesWithEntity(Entity entity, Entity projectile) {
        auto& healthComponent = entity.GetComponent<HealthComponent>();
        const auto& danoAoContatoComponent = projectile.GetComponent<DanoAoContatoComponent>();

        healthComponent.lastHealthPercentage = healthComponent.healthPercentage;
        healthComponent.healthPercentage -= danoAoContatoComponent.dano;

        if (healthComponent.healthPercentage <= 0) {
            entity.Destroy();
        }

        projectile.Destroy();
    }

    void Update() {}

};