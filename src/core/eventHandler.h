#pragma once
#include <phi.h>
#include "guid.h"

namespace phi
{
    using eventToken = guid;

    template<class ...Args>
    class eventHandler
    {
    private:
        struct eventTokenFunction
        {
            eventTokenFunction(eventToken token, std::function<void(Args...)> function) :
                token(token), function(function) {}

            eventToken token;
            std::function<void(Args...)> function;
        };

        std::vector<eventTokenFunction> _functions;

    public:
        eventToken assign(std::function<void(Args...)> function)
        {
            auto guid = guid::newGuid();
            _functions.push_back(eventTokenFunction(guid, function));
            return guid;
        }

        void unassign(const eventToken& t)
        {
            _functions.erase(
                std::remove_if(
                    _functions.begin(),
                    _functions.end(),
                    [&t](eventTokenFunction pair) { return pair.token == t; }));
        }

        void raise(Args... args)
        {
            for (auto& pair : _functions)
                pair.function(args...);
        }

        eventToken operator += (std::function<void(Args...)> function)
        {
            return assign(function);
        }

        void operator -= (const eventToken& token)
        {
            unassign(token);
        }
    };
}