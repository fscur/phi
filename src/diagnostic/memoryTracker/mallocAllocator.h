#pragma once
#include <phi.h>

namespace phi
{
    template<typename T>
    class mallocAllocator
    {
    public:
        typedef T value_type;
        typedef value_type* pointer;
        typedef const value_type* const_pointer;
        typedef value_type& reference;
        typedef const value_type& const_reference;
        typedef std::size_t size_type;
        typedef std::ptrdiff_t difference_type;

    public:
        //convert an allocator<T> to allocator<U>
        template<typename U>
        struct rebind 
        {
            typedef mallocAllocator<U> other;
        };

    public:
        mallocAllocator()
        {
        }

        ~mallocAllocator() 
        {
        }

        mallocAllocator(mallocAllocator const& allocator)
        {
        }

        template<typename U>
        mallocAllocator(mallocAllocator<U> const& allocator)
        {
        }

        pointer address(reference r)
        {
            return &r;
        }

        const_pointer address(const_reference r)
        {
            return &r;
        }

        pointer allocate(size_type count, typename std::allocator<void>::const_pointer = 0) 
        {
            return reinterpret_cast<pointer>(malloc(count * sizeof(T)));
        }

        void deallocate(pointer address, size_type)
        {
            free(address);
        }

        inline size_type max_size() const
        {
            return std::numeric_limits<size_type>::max() / sizeof(T);
        }

        void construct(pointer address, const T& type)
        {
            new(address) T(type);
        }

        void destroy(pointer p)
        {
            p->~T();
        }

        bool operator==(mallocAllocator const& allocator)
        {
            return true;
        }

        bool operator!=(mallocAllocator const& allocator)
        {
            return !operator==(allocator);
        }
    };
}