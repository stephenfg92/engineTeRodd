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
#include "../Components/GeradorDeForcaDeMola.h"

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

        void AplicarForca(ParticleComponent& particula, Vector2 forca){
            if (particula.massaInfinita) return;

            Vector2 forcaVezesMassa = Vector2Scale(forca, particula.massa);
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

        void AplicarForcaDeMola(ParticleComponent& particula, TransformComponent& transform, Entity* entidadeNaOutraExtremidade, GeradorDeForcaDeMola& mola){
            if (entidadeNaOutraExtremidade == nullptr)
                return;
            
            Registry* registry = entidadeNaOutraExtremidade->GetRegistry();
            Vector2 posicaoDaOutraExtremidade = registry->GetComponent<TransformComponent>(*entidadeNaOutraExtremidade).position;
            
            Vector2 forca;
            forca = transform.position;
            forca = Vector2Subtract(forca, posicaoDaOutraExtremidade);

            float magnitudeDaForca = Vector2Length(forca);
            magnitudeDaForca = fabs(magnitudeDaForca - mola.comprimentoDeRepouso);
            magnitudeDaForca *= mola.constanteDaMola;

            Vector2 forcaNormalizada = Vector2Normalize(forca);
            Vector2 forcaDaMola = Vector2Scale(forcaNormalizada, -magnitudeDaForca);
            particula.acumuladorDeForca = Vector2Add(particula.acumuladorDeForca, forcaDaMola);
        }

        // Pode ser usada para uma câmera presa a uma mola. Basta mover a entidadeAncora com pase na posição do personagem.
        void AplicarForcaDeMolaAncorada(ParticleComponent& particula, TransformComponent& transform, Entity* entidadeAncora, GeradorDeForcaDeMola& mola){
            if (entidadeAncora == nullptr)
                return;
            
            Registry* registry = entidadeAncora->GetRegistry();
            Vector2 posicaoDaOutraExtremidade = registry->GetComponent<TransformComponent>(*entidadeAncora).position;
            
            Vector2 forca;
            forca = transform.position;
            forca = Vector2Subtract(forca, posicaoDaOutraExtremidade);

            float magnitudeDaForca = Vector2Length(forca);
            magnitudeDaForca = (mola.comprimentoDeRepouso - magnitudeDaForca) * mola.constanteDaMola;

            Vector2 forcaNormalizada = Vector2Normalize(forca);
            Vector2 forcaDaMola = Vector2Scale(forcaNormalizada, magnitudeDaForca);
            particula.acumuladorDeForca = Vector2Add(particula.acumuladorDeForca, forcaDaMola);
        }

        void AplicarForcaDeElastico(ParticleComponent& particula, TransformComponent& transform, Entity* entidadeNaOutraExtremidade, GeradorDeForcaDeMola& mola){
            if (entidadeNaOutraExtremidade == nullptr)
                return;
            
            Registry* registry = entidadeNaOutraExtremidade->GetRegistry();
            Vector2 posicaoDaOutraExtremidade = registry->GetComponent<TransformComponent>(*entidadeNaOutraExtremidade).position;
            
            Vector2 forca;
            forca = transform.position;
            forca = Vector2Subtract(forca, posicaoDaOutraExtremidade);

            float magnitudeDaForca = Vector2Length(forca);
            if (magnitudeDaForca <= mola.comprimentoDeRepouso)
                return;
            magnitudeDaForca = (mola.comprimentoDeRepouso - magnitudeDaForca) * mola.constanteDaMola;

            Vector2 forcaNormalizada = Vector2Normalize(forca);
            Vector2 forcaElastica = Vector2Scale(forcaNormalizada, -magnitudeDaForca);
            particula.acumuladorDeForca = Vector2Add(particula.acumuladorDeForca, forcaElastica);
        }

        // Precisa que o comprimentoDeRepouso seja sempre igual a 0.
        void AplicarForcaDeFalsaMolaRigida(ParticleComponent& particula, TransformComponent& transform, Entity* entidadeAncorada, GeradorDeForcaDeMola& mola, float deltaTempo){
            if (entidadeAncorada == nullptr)
                return;

            if (particula.massaInfinita)
                return;
            
            Registry* registry = entidadeAncorada->GetRegistry();
            Vector2 posicaoDaAncora = registry->GetComponent<TransformComponent>(*entidadeAncorada).position;
            Vector2 posicaoRelativaAancora = Vector2Subtract(transform.position, posicaoDaAncora);

            float gamma = 0.5f * sqrt(4 * mola.constanteDaMola - particula.atenuacao * particula.atenuacao);
            if (gamma == 0.0f)
                return;
            Vector2 c = Vector2Scale(posicaoDaAncora, particula.atenuacao / (2.0f * gamma));
            Vector2 velocidadeEscolonadaReciprocoGama = Vector2Scale(particula.velocity, 1.0f / gamma);
            c = Vector2Add(c, velocidadeEscolonadaReciprocoGama);

            Vector2 posicaoAlvo = Vector2Scale(posicaoRelativaAancora, cos(gamma * deltaTempo));
            Vector2 cVezesSenoGamaVezesDelta = Vector2Scale(c, sin(gamma * deltaTempo));
            posicaoAlvo = Vector2Add(posicaoAlvo, cVezesSenoGamaVezesDelta);
            float exp = std::exp(-0.5f * deltaTempo * particula.atenuacao);
            posicaoAlvo = Vector2Scale(posicaoAlvo, exp);

            Vector2 aceleracao = Vector2Subtract(posicaoAlvo, posicaoRelativaAancora);
            aceleracao = Vector2Scale(aceleracao, 1.0f / (deltaTempo * deltaTempo));
            aceleracao = Vector2Subtract(aceleracao, Vector2Scale(particula.velocity, 1.0f / deltaTempo));

            particula.acumuladorDeForca = Vector2Scale(aceleracao, particula.massa);
        }
};