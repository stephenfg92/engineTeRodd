#pragma once

#include <bitset>
#include <vector>
#include <algorithm>
#include <unordered_map>
#include <deque>
#include <typeindex>
#include <memory>
#include <set>
#include <assert.h>
#include "Tag.h"
//#include <Logger/Logger.h>

/*
 * Entity
 */

class Entity{
    private:
        int id;

        // Forward declaration. Permite usar um pointer para registry,
        // mesmo que ele venha a ser declarado só lá em baixo.
        class Registry* registry;
    
    public:
        Entity(int id, Registry* registry): id(id), registry(registry){};
        Entity(const Entity& entity) = default;

        int GetId() const;
        Registry* GetRegistry();
        void Destroy();

        Entity& operator =(const Entity& other) = default;
        bool operator ==(const Entity& other) const {return id == other.id;}
        bool operator !=(const Entity& other) const {return id != other.id;}
        bool operator >(const Entity& other) const {return id > other.id;}
        bool operator <(const Entity& other) const {return id < other.id;}

        // Templates implementados após a definição de registry. Forward declaration exige que seja desta maneira.
        template <typename TComponent, typename ...TArgs> void AddComponent(TArgs&& ...args);
        template <typename TComponent> void RemoveComponent();
        template <typename TComponent> bool HasComponent() const;
        template <typename TComponent> TComponent& GetComponent() const;

        // Tag management
        void AddTag(const Tag tag);
        void RemoveTag(const Tag tag);
        bool HasTag(const Tag) const;
        std::set<Tag> GetTags() const;
};

/*
 * Component
 */

const unsigned int MAX_COMPONENTS = 32;
typedef std::bitset<MAX_COMPONENTS> Signature;

// Component interface
struct IComponent {
    protected:
        //static int nextId;
        inline static unsigned int nextId{0};
};

// Assigns a unique id to a component type
template <typename T> class Component: public IComponent{
    public:
        static int GetId(){
            static auto id = nextId++;
            return id;
        }
};

/*
 * System
 * Nota sobre os sistemas:
 * Se você esquecer de requisitar algum componente cujo sistema irá operar, coisas horríveis acontecem.
 * Ex.: vá ao sistema de animação, comente o RequireComponent<AnimationComponent>();
 * O tilemap vai ficar todo bugado e uma exceção será lançada quando o fechamento do programa for requisitado.
 * Minha hipótese é que a referência ao sistema inexistente traz algum pedaço nada a ver da memória.
 * O que é bem estranho, já que mesmo alterar o auto& para AnimationComponent& não resolve o problema.
 */

class System{
    private:
        // This system will only process entities with a matching componentSignature.
        Signature componentSignature;
        std::vector<Entity> entities;

    public:
        System() = default;
        ~System()= default;
        
        void AddEntity(Entity entity);
        void RemoveEntityFromSystem(Entity entity);
        const std::vector<Entity>& GetSystemEntities() const;
        void SetSystemEntities(std::vector<Entity>& entities);
        const Signature& GetComponentSignature() const;
        
        // Sets the componentSignature bitflags according to the passed component.
        template <typename TComponent> void RequireComponent();

};

// RequireComponent mexe em componentSignature, que é usado no método AddEntityToSystems
template <typename TComponent> void System::RequireComponent(){
    const auto componentId = Component<TComponent>::GetId();
    componentSignature.set(componentId);
}

/*
 * Pool
 * A vector of objects of type T.
 * Index = entity id.
 * To avoid resizing during runtime, set DEFAULT_POOL_SIZE to a value matching the game's needs.
 * Preferably, make the value a power of 2.
 * Ideally, this pool should fit in the processor's cache lines. Bonus points if it leaves space for other pools.
 */
const int DEFAULT_POOL_SIZE = 32;

class IPool {
    public:
        virtual ~IPool() = default;
        virtual void RemoveEntityFromPool(int entityId) = 0; //Pure virtual method
};

template <typename T>
class Pool: public IPool {
    private:
        std::vector<T> data;
        int currentlyFreeIndex;
        std::unordered_map<int, int> entityIdToIndex;
        std::unordered_map<int, int> indexToEntityId;

    public:
        Pool(int size = DEFAULT_POOL_SIZE){
            currentlyFreeIndex = 0;
            data.resize(size);
        }
        virtual ~Pool() = default;

        bool IsEmpty() const {return currentlyFreeIndex <= 0;}
        int GetSize() const {return currentlyFreeIndex;}
        void Clear() { currentlyFreeIndex = 0; data.clear(); }
        void Set(int entityId, T object);
        void RemoveEntity(int entityId);
        T& Get(int entityId); // Retorna referência pois seria impossível modificar uma cópia.
        T& operator[] (int entityId);

        void RemoveEntityFromPool(int entityId) override; //Permite métodos que só tem acesso a IPool usar o método RemoveEntity();
};

template <typename T> void Pool<T>::Set(int entityId, T object) {
    if (entityIdToIndex.find(entityId) != entityIdToIndex.end()) {
        int indexToBeReplaced = entityIdToIndex[entityId];
        data[indexToBeReplaced] = object;
        return;
    }

    if (static_cast<std::size_t>(currentlyFreeIndex) >= data.capacity())
        data.resize(data.size() * 2); // Duplica o tamanho para evitar resizes a cada adição.

    data[currentlyFreeIndex] = object;
    entityIdToIndex.emplace(entityId, currentlyFreeIndex);
    indexToEntityId.emplace(currentlyFreeIndex, entityId);

    currentlyFreeIndex++;
}

template <typename T> void Pool<T>::RemoveEntity(int entityId) {
    int indexToBeRemoved = entityIdToIndex[entityId];
    int indexOfLastItem = currentlyFreeIndex - 1;

    // Último item passa a assumir a posição do item removido
    data[indexToBeRemoved] = data[indexOfLastItem];

    int entityIdOfFormerLastItem = indexToEntityId[indexOfLastItem];
    int newIndexOfFormerLastItem = indexToBeRemoved;

    entityIdToIndex[entityIdOfFormerLastItem] = newIndexOfFormerLastItem;
    indexToEntityId[newIndexOfFormerLastItem] = entityIdOfFormerLastItem;

    entityIdToIndex.erase(entityId);
    indexToEntityId.erase(indexOfLastItem);

    currentlyFreeIndex--;
}

template <typename T> void Pool<T>::RemoveEntityFromPool(int entityId) {
    if (entityIdToIndex.find(entityId) != entityIdToIndex.end())
        RemoveEntity(entityId);
}

template <typename T> T& Pool<T>::Get(int entityId) {
    int indexOfRequestedItem = entityIdToIndex[entityId];
    return data[indexOfRequestedItem];
}

template <typename T> T& Pool<T>::operator[](int entityId) {
    return Get(entityId);
}

/*
 * Registry
 * To avoid resizing during runtime, set DEFAULT_COMPONENT_POOLS_SIZE to a value matching the game's needs.
 * Preferably, make the value a power of 2.
 */

const int DEFAULT_COMPONENT_POOLS_SIZE = 32;

class Registry{
    private:
        int numEntities = 0;
        std::set<Entity> entitiesToBeAdded; // Defered creation.
        std::set<Entity> entitiesToBeDestroyed; // Defered destruction.
        std::deque<int> freeIds; // Makes recently freed ids available to new entities.

        // Vector of component pools. Vector index = component type id.
        std::vector<std::shared_ptr<IPool>> componentPools;
        // Keeps track of what component are used in each entity. Index = entity id.
        std::vector<Signature> entityComponentSignatures;
        // All the systems used in the game
        std::unordered_map<std::type_index, std::shared_ptr<System>> systems;
        // Entity tags.
        std::unordered_map<Tag, std::set<std::shared_ptr<Entity>>> entitiesPerTag;
        std::unordered_map<int, std::set<Tag>> tagsPerEntity;

        //Logger logger; //TODO: faça disso um singleton!

    public:
        Registry() {} //componentPools.resize(DEFAULT_COMPONENT_POOLS_SIZE); }

        void Update();

        // Entity Management
        Entity CreateEntity();
        void DestroyEntity(Entity entity);

        // Component management
        template <typename TComponent, typename ...TArgs> void AddComponent(Entity entity, TArgs&& ...args);
        template <typename TComponent> void RemoveComponent(Entity entity);
        template <typename TComponent> bool HasComponent(Entity entity);
        template <typename TComponent> TComponent& GetComponent(Entity entity) const;
        template <typename TComponent> TComponent& GetComponent(int entityId) const;

        // System management
        template <typename TSystem, typename ...TArgs> void AddSystem(TArgs&& ...args);
        template <typename TSystem> void RemoveSystem();
        template <typename TSystem> bool HasSystem();
        template <typename TSystem> TSystem& GetSystem() const;
        
        // Adds the entity to systems matching the entity's component signature.
        void AddEntityToSystems(Entity entity);
        void RemoveEntityFromSystems(Entity entity);

        // Tag management
        void AddTagToEntity(const Tag tag, Entity& entity);
        void RemoveTagFromEntity(const Tag tag, Entity& entity);
        void RemoveAllTagsFromEntity(Entity& entity);
        std::set<std::shared_ptr<Entity>> GetEntitiesWithTag(const Tag tag);
        std::set<Tag> GetEntityTags(const Entity& entity) const;
        bool EntityHasTag(const Tag, const Entity& entity) const;

};

template <typename TComponent, typename ...TArgs> void Registry::AddComponent(Entity entity, TArgs&& ...args) {
    const auto componentId = Component<TComponent>::GetId();
    const auto entityId = entity.GetId();

    // If needed, resizes the component pool.

    if (static_cast<std::size_t>(componentId) >= componentPools.size()) {
        //Possivelmente lento. Buscar alternativas. Ou não... o número de tipos de componentes criados durante o runtime não deve variar muito.
        // Se variar, vai ser uma vez ou outra.
        componentPools.resize(componentId + 1, nullptr);
    }

    // If needed, create a component pool.
    if (!componentPools[componentId]) {
        std::shared_ptr<Pool<TComponent>> newComponentPool = std::make_shared<Pool<TComponent>>(); //É equivalente a linha 204?
        //std::shared_ptr<Pool<TComponent>> newComponentPool(new Pool<TComponent>());
        componentPools[componentId] = newComponentPool;
    }

    // Pointer to the required componentPool.
    // Usar o std::static_pointer_cast<> é necessário pois componentPools guarda variáveis do tipo IPool.
    // Como sabemos que todos pools herdam desta classe, o cast funciona sem problemas.
    // Sem o cast, a variável não seria do tipo std::shared_ptr<Pool<Tcomponent>> e sim std::shared_ptr<Pool<IPool>>.
    std::shared_ptr<Pool<TComponent>> componentPool = std::static_pointer_cast<Pool<TComponent>>(componentPools[componentId]);

    // Forwards the arguments to the chosen component's constructor.
    TComponent newComponent(std::forward<TArgs>(args)...);

    // newComponent now occupies the index denoted by the entityId.
    componentPool->Set(entityId, newComponent);

    // adds the component id to the entity's bitset, effectively turning the component on for this entity.
    entityComponentSignatures[entityId].set(componentId);

    //logger.Log("Component id = " + std::to_string(componentId) + " was added to entity id " + std::to_string(entityId));
}

// Pensando no rollback, será que RemoveComponent deve ser deferred também?
template <typename TComponent> void Registry::RemoveComponent(Entity entity) {
    const auto componentId = Component<TComponent>::GetId();
    const auto entityId = entity.GetId();

    // Desabilita o componente.
    entityComponentSignatures[entityId].set(componentId, false);

    // Remove entidade do pool
    std::shared_ptr<Pool<TComponent>> requestedComponentPool = std::static_pointer_cast<Pool<TComponent>>(componentPools.at(componentId));
    requestedComponentPool->RemoveEntity(entityId);    
}

template <typename TComponent> bool Registry::HasComponent(Entity entity) {
    const auto componentId = Component<TComponent>::GetId();
    const auto entityId = entity.GetId();

    return entityComponentSignatures[entityId].test(componentId);
}

template <typename TComponent> TComponent& Registry::GetComponent(Entity entity) const {
    const auto componentId = Component<TComponent>::GetId();
    const auto entityId = entity.GetId();

    auto componentPool = std::static_pointer_cast<Pool<TComponent>>(componentPools[componentId]);

    return componentPool->Get(entityId);
}

template <typename TComponent> TComponent& Registry::GetComponent(int entityId) const {
    const auto componentId = Component<TComponent>::GetId();
    auto componentPool = std::static_pointer_cast<Pool<TComponent>>(componentPools[componentId]);

    return componentPool->Get(entityId);
}

template <typename TComponent, typename ...TArgs> void Entity::AddComponent(TArgs&& ...args) {
    registry->AddComponent<TComponent>(*this, std::forward<TArgs>(args)...);
}

template <typename TComponent> void Entity::RemoveComponent() {
    registry->RemoveComponent<TComponent>(*this);
};

template <typename TComponent> bool Entity::HasComponent() const {
    return registry->HasComponent<TComponent>(*this);
};

template <typename TComponent> TComponent& Entity::GetComponent() const {
    return registry->GetComponent<TComponent>(*this);
};

template <typename TSystem, typename ...TArgs> void Registry::AddSystem(TArgs&& ...args) {
    std::shared_ptr<TSystem> newSystem = std::make_shared<TSystem>(std::forward<TArgs>(args)...);
    //std::type_index(typeid()) pega o typeid definido pelo compilador para nosso tipo gerado por template.
    systems.insert(std::make_pair(std::type_index(typeid(TSystem)), newSystem));
}

template <typename TSystem> void Registry::RemoveSystem() {
    auto systemToBeRemoved = systems.find(std::type_index(typeid(TSystem)));
    systems.erase(systemToBeRemoved);
}

template <typename TSystem> bool Registry::HasSystem() {
    // if the returned value points to the end of the map, the keys does not exist.
    return systems.find(std::type_index(typeid(TSystem))) != systems.end();
}

template <typename TSystem> TSystem& Registry::GetSystem() const {
    // The returned value from systems.find is an iterador with two members. "first" is the key,
    // "second" is the value.
    auto systemToFind = systems.find(std::type_index(typeid(TSystem)));
    //*() derreferencing the pointer, making sure that the returned value is an
    // actual object and not a reference.
    return *(std::static_pointer_cast<TSystem>(systemToFind->second));
}