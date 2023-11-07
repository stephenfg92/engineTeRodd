#pragma once

#include <raylib.h>
#include <raymath.h>

#include <cmath>

#include "../ECS/ECS.h"
#include "../ECS/Tag.h"
#include "../Components/TransformComponent.h"
#include "../Components/ParticleComponent.h"
#include "../Components/BoxColliderComponent.h"
#include "../Components/PlayerParticleEmitterComponent.h"
#include "../Components/SpriteComponent.h"
#include "../Components/DanoAoContatoComponent.h"

#include "../Events/Event.h"
#include "../Events/EventBus.h"
#include "../Events/EmitParticleEvent.h"

class ParticleSystem: public System {
    public:
        ParticleSystem(){
            RequireComponent<TransformComponent>();
            RequireComponent<ParticleComponent>();
        }

        void SubscribeToEvents(std::unique_ptr<EventBus>& eventBus){
            eventBus->SubscribeToEvent<EmitParticleEvent>(this, &ParticleSystem::OnParticleEmissionRequested);
        }

        void OnParticleEmissionRequested(EmitParticleEvent& event){
            Registry* registry = event.registry;
            PlayerParticleEmitterComponent particleEmitter = event.playerParticleEmitter;
            const TransformComponent transform = event.transform;
            const SpriteComponent sprite = event.sprite;
            
            Vector2 projectilePos = transform.position;
            projectilePos.x += (transform.scale.x * sprite.width / 2);
            projectilePos.y += (transform.scale.y * sprite.height / 2);    
            Vector2 velocidade = Vector2Scale(particleEmitter.sentido, particleEmitter.rapidez);

            Entity novaParticula = registry->CreateEntity();
            novaParticula.AddTag(Tag::PLAYER_PROJECTILE);
            novaParticula.AddComponent<TransformComponent>(projectilePos, transform.scale);
            novaParticula.AddComponent<ParticleComponent>(velocidade, Vector2{}, particleEmitter.massa, particleEmitter.atenuacao, particleEmitter.tempoDeVida);
            novaParticula.AddComponent<BoxColliderComponent>(4, 4);
            novaParticula.AddComponent<SpriteComponent>("bullet-image", LAYER_2, 4, 4);
            if (particleEmitter.danoPercentual > 0)
                novaParticula.AddComponent<DanoAoContatoComponent>(particleEmitter.danoPercentual);
        }

        void Update(std::unique_ptr<EventBus>& eventBus, double deltaTempo){
            for (auto entity: GetSystemEntities()){
                ParticleComponent& particle = entity.GetComponent<ParticleComponent>();

                if (GetTime() - particle.tempoDeInicio > particle.tempoDeVida){
                    entity.Destroy();
                    continue;
                }

                TransformComponent& transform = entity.GetComponent<TransformComponent>();

                Integrar(transform, particle, static_cast<float>(deltaTempo));
            }
        }

    private:
        void Integrar(TransformComponent& transform, ParticleComponent& particle, float deltaTempo){
                if (particle.massaReciproca <= 0.0f) return;

                transform.position.x += particle.velocity.x * deltaTempo;
                transform.position.y += particle.velocity.y * deltaTempo;

                Vector2 aceleracaoResultante = particle.aceleracao;
                aceleracaoResultante.x *= particle.acumuladorDeForca.x + particle.massaReciproca;
                aceleracaoResultante.y *= particle.acumuladorDeForca.y + particle.massaReciproca;
                particle.velocity.x += aceleracaoResultante.x * deltaTempo;
                particle.velocity.y += aceleracaoResultante.y * deltaTempo;

                float coeficienteDeAtenuacao = pow(particle.atenuacao, deltaTempo);
                particle.velocity.x *= coeficienteDeAtenuacao;
                particle.velocity.y *= coeficienteDeAtenuacao;

                particle.acumuladorDeForca.x = .0f;
                particle.acumuladorDeForca.y = .0f;
        }

        void AplicarForcaDeGravidade(ParticleComponent& particula, Vector2 forcaGravitacional){
            if (particula.massaInfinita) return;

            Vector2 forcaVezesMassa = Vector2Scale(forcaGravitacional, particula.massa);
            particula.acumuladorDeForca = Vector2Add(particula.acumuladorDeForca, forcaVezesMassa);
        }

        void AplicarForcaDeArrasto(ParticleComponent& particula, float coefiecienteDeArrastoK1, float coeficienteDeArrastoK2){
            Vector2 velocidade = particula.velocity;

            float magnitudeVelocidade = Vector2Length(velocidade);
            float coeficienteDeArrasto = 
                (coefiecienteDeArrastoK1 * magnitudeVelocidade) +
                (coeficienteDeArrastoK2 * magnitudeVelocidade * magnitudeVelocidade);

            Vector2 velocidadeNormalizada = Vector2Normalize(velocidade);
            Vector2 forcaDeArrasto = Vector2Scale(velocidadeNormalizada, -coeficienteDeArrasto);

            particula.acumuladorDeForca = Vector2Add(particula.acumuladorDeForca, forcaDeArrasto);
        }
};