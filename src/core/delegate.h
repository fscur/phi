#pragma once
#include <phi.h>

//http://molecularmusings.wordpress.com/2011/09/19/generic-type-safe-delegates-and-events-in-c/
//or
//http://www.codeproject.com/Articles/6197/Emulating-C-delegates-in-Standard-C

namespace phi
{
    template <typename T>
    class delegate
    {
        typedef void* instancePtr;
        typedef void (*internalFunction)(instancePtr);
        typedef std::pair<instancePtr, internalFunction> stub;
 
        // turns a free function into our internal function stub
        template <void (*function)()>
        static void functionStub(instancePtr)
        {
            // we don't need the instance pointer because we're dealing with free functions
            return (function)();
        }
 
        // turns a member function into our internal function stub
        template <class C, void (C::*function)()>
        static void classMethodStub(instancePtr instance)
        {
            // cast the instance pointer back into the original class instance
            return (static_cast<C*>(instance)->*function)();
        }
 
    public:
        delegate(void)
        : _stub(nullptr, nullptr)
        {
        }
 
        /// Binds a free function
        template <void (*function)()>
        void bind(void)
        {
            _stub.first = nullptr;
            _stub.second = &functionStub<function>;
        }
 
        /// Binds a class method
        template <class C, void (C::*function)()>
        void bind(C* instance)
        {
            _stub.first = instance;
            _stub.second = &classMethodStub<C, function>;
        }
 
        /// Invokes the delegate
        void invoke() const
        {
            //assert(_stub.second != nullptr, "Cannot invoke unbound delegate. Call bind() first.")();
            return _stub.second(_stub.first);
        }
 
    private:
        stub _stub;
    };

    template <typename R>
    class delegate<R ()>
    {
        typedef void* instancePtr;
        typedef void (*internalFunction)(instancePtr);
        typedef std::pair<instancePtr, internalFunction> stub;
     
        // turns a free function into our internal function stub
        template <R (*function)()>
        static R functionStub(instancePtr)
        {
        // we don't need the instance pointer because we're dealing with free functions
            return (function)();
        }
     
        // turns a member function into our internal function stub
        template <class C, R (C::*function)()>
        static R classMethodStub(instancePtr instance)
        {
            // cast the instance pointer back into the original class instance
            return (static_cast<C*>(instance)->*function)();
        }

    public:
        delegate(void)
        : _stub(nullptr, nullptr)
        {
        }
 
        /// Binds a free function
        template <R (*function)()>
        void bind(void)
        {
        _stub.first = nullptr;
        _stub.second = &functionStub<function>;
        }
 
        /// Binds a class method
        template <class C, R (C::*function)()>
        void bind(C* instance)
        {
        _stub.first = instance;
        _stub.second = &classMethodStub<C, function>;
        }
 
        /// Invokes the delegate
        R invoke() const
        {
        //assert(_stub.second != nullptr, "Cannot invoke unbound delegate. Call bind() first.")();
        return _stub.second(_stub.first);
        }
 
    private:
        stub _stub;
    };

    template <typename ARG0>
    class delegate<void (ARG0)>
    {
        typedef void* instancePtr;
        typedef void (*internalFunction)(instancePtr, ARG0);
        typedef std::pair<instancePtr, internalFunction> stub;
 
        // turns a free function into our internal function stub
        template <void (*function)(ARG0)>
        static void functionStub(instancePtr, ARG0 arg0)
        {
            // we don't need the instance pointer because we're dealing with free functions
            return (function)(arg0);
        }
 
        // turns a member function into our internal function stub
        template <class C, void (C::*function)(ARG0)>
        static void classMethodStub(instancePtr instance, ARG0 arg0)
        {
            // cast the instance pointer back into the original class instance
            return (static_cast<C*>(instance)->*function)(arg0);
        }
 
        public:
        delegate(void)
        {   
            _stubs = new vector<stub>();
        }

        ~delegate()
        {
            delete _stubs;
        }
        /// Binds a free function
        template <void (*function)(int)>
        void bind(void)
        {
            stub _stub = stub(nullptr, nullptr);
            _stub.first = nullptr;
            _stub.second = &functionStub<function>;

            _stubs->push_back(_stub);
        }
 
        /// Binds a class method
        template <class C, void (C::*function)(ARG0)>
        void bind(C* instance)
        {
            stub _stub = stub(nullptr, nullptr);
            _stub.first = instance;
            _stub.second = &classMethodStub<C, function>;

            _stubs->push_back(_stub);
        }

        template <class C, void(C::*function)(ARG0)>
        void unbind(C* instance)
        {
            stub s = stub(nullptr, nullptr);
            s.first = instance;
            s.second = &classMethodStub<C, function>;
            _stubs->erase(std::remove(_stubs->begin(), _stubs->end(), s), _stubs->end());
            _i--;
        }

        /// Invokes the delegate
        void invoke(ARG0 arg0)
        {
            //assert(_stub.second != nullptr, "Cannot invoke unbound delegate. Call bind() first.")();

            for (_i = 0; _i < _stubs->size(); ++_i)
            {
                stub a = (*_stubs)[_i];
                a.second(a.first, arg0);
            }
        }
 
        bool isBound()
        {
            return _stubs->size() > 0;
        }

    private:
        vector<stub>* _stubs;
        int _i;
    };
}