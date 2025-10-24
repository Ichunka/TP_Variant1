#ifndef TYPE_TRAITS_H
#define TYPE_TRAITS_H

#include <string>
#include <type_traits>

template <std::size_t I, typename T, typename... Ts>
struct type_at_impl
{
    using type = typename type_at_impl<I - 1, Ts...>::type;
};

template <typename T, typename... Ts>
struct type_at_impl<0, T, Ts...>
{
    using type = T;
};

template <std::size_t I, typename... Ts>
using type_at = typename type_at_impl<I, Ts...>::type;

static_assert(std::is_same_v<type_at<0, int, double, float>, int>);
static_assert(std::is_same_v<type_at<2, int, double, float>, float>);

template <typename T, typename... Ts>
struct index_of;

template <typename T, typename U, typename... Ts>
struct index_of<T,U,Ts...>
{
    static constexpr std::size_t value = 1 + index_of<T, Ts...>::value;
};

template <typename T, typename... Ts>
struct index_of<T, T, Ts...>
{
    static constexpr std::size_t value = 0;
};

static_assert(index_of<double, int, double, float>::value == 1) ;
static_assert(index_of<int, int, double, float>::value == 0) ;

template <typename T, typename... Ts>
struct contains : std::false_type
{};

template <typename T, typename... Ts>
struct contains<T, T, Ts...> : std::true_type
{};

template <typename T, typename U, typename... Ts>
struct contains<T, U, Ts...> : contains<T, Ts...>
{};

static_assert(contains<int, float, int, float>::value == true);
static_assert(contains<float, float, int, float>::value == true);
static_assert(contains<std::string, float, int, float>::value == false);

template <std::size_t... Ts>
struct max_n;

template <std::size_t S>
struct max_n<S>
{
    static constexpr std::size_t value = S;
};

template <std::size_t S1, std::size_t... Ts>
struct max_n<S1, Ts...>
{
    static constexpr std::size_t value = (std::max)(S1, max_n<Ts...>::value);
};

static_assert(max_n<1,8,12>::value == 12);

template <typename... Ts>
struct max_size
{
    static constexpr std::size_t value = max_n<sizeof(Ts)...>::value;
};

template <typename... Ts>
struct max_align
{
    static constexpr std::size_t value = max_n<alignof(Ts)...>::value;
};

static_assert(max_size<int, char, double>::value == sizeof(double));
static_assert(max_align<int, char, double>::value == alignof(double));

template<typename... Ts>
struct all_trivially_destructible;

struct Foo {
    std::string str;

    ~Foo() noexcept {
    };

    Foo(Foo &foo) {
    }
};

template<typename T, typename... Ts>
struct all_trivially_destructible<T, Ts...> {
    static constexpr bool value = std::is_trivially_destructible<T>::value && all_trivially_destructible<Ts...>::value;
};

template<typename T>
struct all_trivially_destructible<T> {
    static constexpr bool value = std::is_trivially_destructible<T>::value;
};

static_assert(all_trivially_destructible<int, float>::value == true);
static_assert(all_trivially_destructible<std::string, Foo, int>::value == false);

template<typename... Ts>
struct all_trivially_copy_constructible;

template<typename T, typename... Ts>
struct all_trivially_copy_constructible<T, Ts...> {
    static constexpr bool value = std::is_trivially_copy_constructible<T>::value && std::is_trivially_copy_constructible
                                  <Ts...>::value;
};

template<typename T>
struct all_trivially_copy_constructible<T> {
    static constexpr bool value = std::is_trivially_copy_constructible<T>::value;
};

static_assert(all_trivially_copy_constructible<int, float>::value == true);
static_assert(all_trivially_copy_constructible<Foo, float>::value == false);

template<typename... Ts>
struct all_trivially_move_constructible;

template<typename T, typename... Ts>
struct all_trivially_move_constructible<T, Ts...> {
    static constexpr bool value = std::is_trivially_move_constructible<T>::value && std::is_trivially_move_constructible
                                  <Ts...>::value;
};

template<typename T>
struct all_trivially_move_constructible<T> {
    static constexpr bool value = std::is_trivially_move_constructible<T>::value;
};

static_assert(all_trivially_move_constructible<int, float>::value == true);
static_assert(all_trivially_move_constructible<Foo, float>::value == false);

namespace detail {
    template<bool B, typename T = void>
    using enable_if_t = typename std::enable_if<B, T>::type;
    template<typename T>
    using decay_t = typename std::decay<T>::type;
    template<typename T>
    using remove_cv_t = typename std::remove_cv<T>::type;

    template<typename T>
    using remove_reference_t = typename std::remove_reference<T>::type;
}

#endif //TP_TUPLE_TYPE_TRAITS_H