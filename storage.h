#ifndef STORAGE_H
#define STORAGE_H
#include <array>
#include "type_traits.h"

template <size_t size, size_t align>
struct aligned_storage_base
{
    alignas(align) std::byte data[size];
};

template <size_t size, size_t align>
using aligned_storage_t = aligned_storage_base<size, align>;

using S1 = aligned_storage_t<16, 16>;
static_assert(sizeof(S1) == 16, "Size incorrect pour S1");
static_assert(alignof(S1) == 16, "Align incorrect pour S1");

using S2 = aligned_storage_t<1, 1>;
static_assert(sizeof(S2) == 1, "Size incorrect pour S2");
static_assert(alignof(S2) == 1, "Align incorrect pour S2");

using S3 = aligned_storage_t<32, 16>;
static_assert(sizeof(S3) == 32, "Size incorrect pour S3");
static_assert(alignof(S3) == 16, "Align incorrect pour S3");

template <typename... Ts>
struct destructor_dispatcher;

template <typename T, typename... Ts>
struct destructor_dispatcher<T, Ts...>
{
    static void destroy(std::size_t index, void* storage)
    {
        if (index == 0)
        {
            reinterpret_cast<T*>(storage)-> ~T();
        }
        else
        {
            destructor_dispatcher<Ts...>::destroy(index - 1, storage);
        }
    }
};

template <>
struct destructor_dispatcher<>
{
    static void destroy(std::size_t, void*)
    {

    }
};

template <typename... Ts>
struct copy_constructor_dispatcher;

template <typename T, typename... Ts>
struct copy_constructor_dispatcher<T, Ts...>
{
    static void copy(std::size_t index, void* destination, const void* source)
    {
        if (index == 0)
        {
            new (destination) T(*reinterpret_cast<const T*>(source));
        }
        else
        {
            copy_constructor_dispatcher<Ts...>::copy(index - 1, destination, source);
        }
    }
};

template <>
struct copy_constructor_dispatcher<>
{
    static void copy(std::size_t, void*, const void*)
    {

    }
};

template <typename... Ts>
struct move_constructor_dispatcher;

template <typename T, typename... Ts>
struct move_constructor_dispatcher<T, Ts...>
{
    static void move(std::size_t index, void* destination, void* source)
    {
        if (index == 0)
        {
            new (destination) T(std::move(*reinterpret_cast<T*>(source)));
        }
        else
        {
            move_constructor_dispatcher<Ts...>::move(index - 1, destination, source);
        }
    }
};

template <>
struct move_constructor_dispatcher<>
{
    static void move(std::size_t, void*, void*)
    {

    }
};

template <typename... Ts>
struct copy_assignment_dispatcher;

template <typename T, typename... Ts>
struct copy_assignment_dispatcher<T, Ts...>
{
    static void copy(std::size_t index, void* destination, const void* source)
    {
        if (index == 0)
        {
            *reinterpret_cast<T*>(destination) = *reinterpret_cast<const T*>(source);
        }
        else
        {
            copy_assignment_dispatcher<Ts...>::copy(--index, destination, source);
        }
    }
};

template <>
struct copy_assignment_dispatcher<>
{
    static void copy(std::size_t , void* destination, const void* source)
    {

    }
};

template <typename... Ts>
struct move_assignment_dispatcher;

template <typename T, typename... Ts>
struct move_assignment_dispatcher<T, Ts...>
{
    static void move(std::size_t index, void* destination, void* source)
    {
        if (index == 0)
        {
            *reinterpret_cast<T*>(destination) = std::move(*reinterpret_cast<const T*>(source));
        }
        else
        {
            move_assignment_dispatcher<Ts...>::move(--index, destination, source);
        }
    }
};

template <>
struct move_assignment_dispatcher<>
{
    static void move(std::size_t, void*, void*)
    {

    }
};

constexpr std::size_t invalid_index = static_cast<std::size_t>(-1);

template <bool TriviallyDestructible, typename... Ts>
struct variant_storage_impl
{
    aligned_storage_t<max_size<Ts...>::value, max_align<Ts...>::value> storage;
    std::size_t index;

    aligned_storage_t<max_size<Ts...>::value, max_align<Ts...>::value>* data()
    {
        return &storage;
    }

    void destroy()
    {
        destructor_dispatcher<Ts...>::destroy(index, storage.data);
    }
};

template <typename... Ts>
struct variant_storage_impl<true, Ts...>
{

    ~variant_storage_impl<true,Ts...>() = default;

    aligned_storage_t<max_size<Ts...>::value, max_align<Ts...>::value> storage;
    std::size_t index;

    aligned_storage_t<max_size<Ts...>::value, max_align<Ts...>::value>* data()
    {
        return &storage;
    }

    void destroy()
    {
        index = invalid_index;
    }
};

template <typename... Ts>
struct variant_storage_impl<false, Ts...>
{
    aligned_storage_t<max_size<Ts...>::value, max_align<Ts...>::value> storage;
    std::size_t index;

    ~variant_storage_impl<false,Ts...>()
    {
        destroy();
    }

    aligned_storage_t<max_size<Ts...>::value, max_align<Ts...>::value>* data()
    {
        return &storage;
    }

    void destroy()
    {
        destructor_dispatcher<Ts...>::destroy(index, storage.data);
    }
};

#endif //STORAGE_H
