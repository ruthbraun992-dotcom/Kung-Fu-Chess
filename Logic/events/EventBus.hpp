#pragma once
#include <functional>
#include <unordered_map>
#include <typeindex>
#include <vector>
#include <any>

class EventBus {
public:
    template<typename EventType>
    using Handler = std::function<void(const EventType&)>;

    template<typename EventType>
    void subscribe(Handler<EventType> handler) {
        auto& list = handlers_[std::type_index(typeid(EventType))];
        list.push_back([handler](const std::any& e) {
            handler(std::any_cast<const EventType&>(e));
        });
    }

    template<typename EventType>
    void publish(const EventType& event) {
        auto it = handlers_.find(std::type_index(typeid(EventType)));
        if (it == handlers_.end()) return;
        for (auto& wrapped : it->second) {
            wrapped(event);
        }
    }

private:
    std::unordered_map<std::type_index,
        std::vector<std::function<void(const std::any&)>>> handlers_;
};