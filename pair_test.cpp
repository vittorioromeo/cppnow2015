#include <utility>
#include <tuple>
#include <iostream>
#include <unordered_map>
#include <string>

template <typename B, typename C>
struct forNArgsImpl;

template <std::size_t N, typename TF, typename... Ts>
void forNArgs(TF mFn, Ts... mXs)
{
    static_assert(sizeof...(Ts) % N == 0, "Wrong number of arguments");

    forNArgsImpl<std::make_index_sequence<sizeof...(Ts) / N>,
        std::make_index_sequence<N>>::exec(mFn, std::forward_as_tuple(mXs...));
}

template <std::size_t... Bs, std::size_t... Cs>
struct forNArgsImpl<std::index_sequence<Bs...>, std::index_sequence<Cs...>>
{
    template <typename TF, typename... Ts>
    static void exec(TF mFn, const std::tuple<Ts...>& mXs)
    {
        using swallow = bool[];

        (void)swallow{(execN<Bs * sizeof...(Cs)>(mFn, mXs), true)...};
    }

    template <std::size_t N, typename TF, typename... Ts>
    static void execN(TF mFn, const std::tuple<Ts...>& mXs)
    {
        mFn(std::get<N + Cs>(mXs)...);
    }
};

template <typename TK, typename TV, typename... TArgs>
auto make_unordered_map(TK&& mK, TV&& mV, TArgs&&... mArgs)
{
    std::unordered_map<TK, TV> result;
    result.reserve(1 + (sizeof...(mArgs) / 2));

    forNArgs<2>(
        [&result](auto&& k, auto&& v)
        {
            result.emplace(
                std::forward<decltype(k)>(k), std::forward<decltype(v)>(v));
        },

        std::forward<TK>(mK), std::forward<TV>(mV),
        std::forward<TArgs>(mArgs)...);

    return result;
}

template <typename TK, typename TV, typename... TArgs>
auto make_unordered_map2(std::pair<TK, TV>&& mP, TArgs&&... mArgs)
{
    std::unordered_map<decltype(mP.first), decltype(mP.second)> result;
    result.reserve(1 + sizeof...(mArgs));

    forNArgs<1>(
        [&result](auto&& p)
        {
            result.emplace(std::forward<decltype(p.first)>(p.first),
                std::forward<decltype(p.second)>(p.second));
        },

        std::forward<decltype(mP)>(mP), std::forward<TArgs>(mArgs)...);

    return result;
}

int main()
{
    using namespace std::literals;

    auto m = make_unordered_map("zero"s, 0, "one"s, 1, "two"s, 2);

    auto m2 = make_unordered_map2({"zero"s, 0});

    std::cout << m["zero"] << " ";
    std::cout << m["one"] << " ";
    std::cout << m["two"] << " ";

    return 0;
}