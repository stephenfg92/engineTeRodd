#include "ECS.h"


/*
 * Entity
 */

int Entity::GetId() const {
    return id;
}

Registry* Entity::GetRegistry(){
    return registry;
}

void Entity::Destroy(){
    registry->DestroyEntity(*this);
}

void Entity::AddTag(const Tag tag) {
    registry->AddTagToEntity(tag, *this);
}

void Entity::RemoveTag(const Tag tag) {
    registry->RemoveTagFromEntity(tag, *this);
}

std::set<Tag> Entity::GetTags() const{
    return registry->GetEntityTags(*this);
}

bool Entity::HasTag(const Tag tag) const{
    return registry->EntityHasTag(tag, *this);
}

/*
 * Component
 */
//int IComponent::nextId = 0; Inicialização inline em ECS.h


/*
 * System
 */

void System::AddEntity(Entity entity){
    entities.push_back(entity);
}

void System::RemoveEntityFromSystem(Entity entity){
    // O terceiro argumento de remove_if é uma função lambda.
    // Ela recebe &entity como argumento.
    entities.erase(
        std::remove_if(entities.begin(),entities.end(),
                        [&entity](Entity other){
                            return entity == other; 
                        }), entities.end()
    );
}

const std::vector<Entity>& System::GetSystemEntities() const{
    return entities;
}

void System::SetSystemEntities(std::vector<Entity>& entities){
    this->entities = entities;
}

const Signature& System::GetComponentSignature() const{
    return componentSignature;
}


/*
 * Registry
 */

void Registry::Update() {
    // Deferred creation of entities
    for (auto entity: entitiesToBeAdded){
        AddEntityToSystems(entity);
    }
    entitiesToBeAdded.clear();    

    // Deferred destructiion of entities
    for (auto entity: entitiesToBeDestroyed){
        RemoveEntityFromSystems(entity);

        // Remove the entity from all the component pools
        for (auto pool : componentPools) {
            if (pool) { //pool pode ser um nullptr por algum motivo.
                int entityId = entity.GetId();
                pool->RemoveEntityFromPool(entityId);
            }
        }

        //Remove entity from tags
        RemoveAllTagsFromEntity(entity);
        entityComponentSignatures[entity.GetId()].reset();
        freeIds.push_back(entity.GetId());
    }
    entitiesToBeDestroyed.clear();

}

Entity Registry::CreateEntity() {
    int entityId;

    if (freeIds.empty()){
        entityId = numEntities++;

        if ((unsigned)entityId >= entityComponentSignatures.size()){
            // Potencialmente lento.
            entityComponentSignatures.resize(entityId + 1);
        }
    } else {
        entityId = freeIds.front();
        freeIds.pop_front();
    }

    // This é um pointer, por isso pode ocupar o lugar de Registry* na classe Entity.
    Entity entity(entityId, this);

    entitiesToBeAdded.insert(entity);
    //logger.Log("Entity created with id " + std::to_string(entityId));
    return entity;
}

void Registry::DestroyEntity(Entity entity){
    entitiesToBeDestroyed.insert(entity);
}

void Registry::AddEntityToSystems(Entity entity){
    const auto entityId = entity.GetId();
    
    const auto& entityComponentSignature = entityComponentSignatures[entityId];

    // systems is a map, so system is an iterator pointer.
    for (auto& system: systems){
        const auto& systemComponentSignature = system.second->GetComponentSignature();
        
        bool areMatchingSignatures = 
            (entityComponentSignature & systemComponentSignature)
            == systemComponentSignature;

        if (areMatchingSignatures) {
            system.second->AddEntity(entity);
        }
    }
}

void Registry::RemoveEntityFromSystems(Entity entity) {
    for (auto system: systems){
        // Systems é um map. Systems.second pega o valor de system.
        system.second->RemoveEntityFromSystem(entity);
    }
}

void Registry::AddTagToEntity(const Tag tag, Entity& entity) {
    const auto entityId = entity.GetId();
    std::shared_ptr<Entity> entityPtr = std::make_shared<Entity>(entity);
  
    // entitiesPerTag
    if (!entitiesPerTag.contains(tag)) {
        std::set<std::shared_ptr<Entity>> entityIdSet;
        entitiesPerTag.insert(std::make_pair(tag, entityIdSet));
    }

    auto entitiesPerTagIterator = entitiesPerTag.find(tag);
    entitiesPerTagIterator->second.emplace(entityPtr);

    // tagPerEntity
    if (!tagsPerEntity.contains(entityId)) {
        std::set<Tag> tagSet;
        tagsPerEntity.insert(std::make_pair(entityId, tagSet));
    }

    auto tagsPerEntityIterator = tagsPerEntity.find(entityId);
    tagsPerEntityIterator->second.emplace(tag);
}

void Registry::RemoveTagFromEntity(const Tag tag, Entity& entity) {
    // entitiesPerTag
    
    // Assim são feitas duas operações de busca, uma em contains e outra em find.
    //if (entitiesPerTag.contains(tag)) {
    //    auto entitiesPerTagIterator = entitiesPerTag.find(tag);
    //    entitiesPerTagIterator->second.erase(entity);
    //}

    // Assim apenas um find é realizado.
    std::shared_ptr<Entity> entityPtr = std::make_shared<Entity>(entity);
    auto entitiesPerTagIterator = entitiesPerTag.find(tag);
    if (entitiesPerTagIterator != entitiesPerTag.end()){
        entitiesPerTagIterator->second.erase(entityPtr);
    }

    // tagPerEntity
    const auto entityId = entity.GetId();
    auto tagsPerEntityIterator = tagsPerEntity.find(entityId);
    if (tagsPerEntityIterator != tagsPerEntity.end()) {
        tagsPerEntityIterator->second.erase(tag);
    }
}

void Registry::RemoveAllTagsFromEntity(Entity& entity) {
    std::set<Tag> tagList;
    const auto entityId = entity.GetId();
    auto tagsPerEntityIterator = tagsPerEntity.find(entityId);
    if (tagsPerEntityIterator != tagsPerEntity.end()) {
        tagList = tagsPerEntityIterator->second;
    }
    tagsPerEntity.erase(entityId);

    std::shared_ptr<Entity> entityPtr = std::make_shared<Entity>(entity);
    for (Tag tag : tagList) {
        auto entitiesPerTagItr = entitiesPerTag.find(tag);
        entitiesPerTagItr->second.erase(entityPtr);
    }
}

// To get the actual entity, use Entity& entity = *entityPointer; or Entity entity = *entityPointer
// To get from an iterator, use Entity chopperFromTag = *(*(registry->GetEntitiesWithTag("player").begin()));
// No futuro, retornar algo como std::vector<Entity>(setOfEntities.begin(), setOfEntities.end());
// Pegar a entidade usando o pointer não é muito rápido.
// O ideal seria guardar o ID e pegar a entidade usando ele. Veja se é possível fazer algo mais rápido usando std::set<EntityID>
// ACHO que std::set<Entity> não funciona pois std::set::insert faz uma cópia do objeto. Então, GetEntitiesWithTag retornaria
// uma cópia possivelmente desatualizada da entidade.
std::set<std::shared_ptr<Entity>> Registry::GetEntitiesWithTag(const Tag tag){
    //if (entitiesPerTag.contains(tag)) {
    //    auto entitiesPerTagIterator = entitiesPerTag.find(tag);
    //    return entitiesPerTagIterator->second;
    //}

    auto entitiesPerTagIterator = entitiesPerTag.find(tag);
    if (entitiesPerTagIterator != entitiesPerTag.end()) {
        return entitiesPerTagIterator->second;
    }

    return std::set<std::shared_ptr<Entity>>{};
}

std::set<Tag> Registry::GetEntityTags(const Entity& entity) const{
    const auto entityId = entity.GetId();
    auto tagsPerEntityIterator = tagsPerEntity.find(entityId);
    if (tagsPerEntityIterator != tagsPerEntity.end()) {
        return tagsPerEntityIterator->second;
    }

    return std::set<Tag>{};
}

bool Registry::EntityHasTag(const Tag tag, const Entity& entity) const{
    const auto entityId = entity.GetId();
    auto tagsPerEntityIterator = tagsPerEntity.find(entityId);
    if (tagsPerEntityIterator != tagsPerEntity.end()) {
        return tagsPerEntityIterator->second.contains(tag);
    }

    return false;
}