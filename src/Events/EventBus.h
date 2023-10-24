#pragma once

#include <map>
#include <typeindex>
#include <memory>
#include <list>
#include <functional>

#include "Event.h"

class IEventCallback {
    private:
        virtual void Call(Event& e) = 0; // "= 0" significa que isso é uma pure virtual function.

    public:
        virtual ~IEventCallback() = default;

        void Execute(Event& e) {
            Call(e);
        }
};

template <typename TOwner, typename TEvent> class EventCallback: public IEventCallback {
    private:

        // CallbackFunction desreferenciada, vinda de TOwner, recebe como argumento
        // uma referência ao tipo de evento.
        // Edit: 14/09. Na verdade é um function pointer https://www.cprogramming.com/tutorial/function-pointers.html;
        // Então, CallbackFunction é um ponteiro para uma função que recebe uma referência a TEvent e retorna void.
        // Portanto, *CallbackFunction é uma função, assim como CallbackFunction é o ponteiro para uma função.
        typedef void (TOwner::*CallbackFunction)(TEvent&);

        TOwner* ownerInstance;
        CallbackFunction callbackFunction; // Observe que o typedef void pode ser usado como var desta classe.

        virtual void Call(Event& e) override {
            // O 1o argumento é a função, o segundo é o dono da função, os demais são argumentos desta função.
            // Isto é equivalente a ownerInstance->callbackFunction(static_cast<TEvent&>(e)). https://devblogs.microsoft.com/oldnewthing/20220401-00/?p=106426
            std::invoke(callbackFunction, ownerInstance, static_cast<TEvent&>(e));
        }

    public:
        EventCallback(TOwner* ownerInstance, CallbackFunction callbackFunction) {
            this->ownerInstance = ownerInstance;
            this->callbackFunction = callbackFunction;
        }

        virtual ~EventCallback() override = default;    
};

typedef std::list<std::unique_ptr<IEventCallback>> HandlerList;

class EventBus {
    private:
        std::map<std::type_index, std::unique_ptr<HandlerList>> subscribers;

    public:
        EventBus() {};

        ~EventBus() {};

        // Clears the subscribers list
        void Reset() {
            subscribers.clear();
        }

        template <typename TEvent, typename TOwner> 
        void SubscribeToEvent(TOwner* ownerInstance, void (TOwner::*callbackFunction)(TEvent&)){
            if (!subscribers[typeid(TEvent)].get()){
                subscribers[typeid(TEvent)] = std::make_unique<HandlerList>();
            }
            auto subscriber = std::make_unique<EventCallback<TOwner, TEvent>>(ownerInstance, callbackFunction);
            // Já que subscriber é um unique pointer, é necessário usar std::move para realizar o push.
            // Std::move altera o proprietário do objeto de um unique_ptr para outro unique_ptr.
            subscribers[typeid(TEvent)]->push_back(std::move(subscriber));
        }

        template<typename TEvent, typename ...TArgs>
        void EmitEvent(TArgs&& ...args){
            auto handlers = subscribers[typeid(TEvent)].get();
            if (handlers){
                TEvent event(std::forward<TArgs>(args)...);
                for (auto it = handlers->begin(); it != handlers->end(); it++) {
                    auto handler = it->get();
                    //TEvent event(std::forward<TArgs>(args)...); // Isso causa um bug! Ver: https://courses.pikuma.com/courses/take/2dgameengine/lessons/19684016-event-handlers/discussions/4820936
                    handler->Execute(event);
                }
            }             
        }

};