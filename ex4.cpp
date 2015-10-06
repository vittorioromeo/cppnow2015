/*
    "`for_each_arg` explained and expanded"
    C++Now 2015 Lightning Talk
    - Vittorio Romeo

    http://vittorioromeo.info
    vittorio.romeo@outlook.com
*/

#include <utility>
#include <tuple>
#include <iostream>

// We can also go further and implement a generic `forNArgs`
// function that passes its arguments to a callable object
// in groups of `N` arguments, where `N` is divisible by
// the number of total arguments.

// The following implementation was created by Daniel Frey.

// It's a very clever iterative implementation that was
// posted as an answer to one of my questions on StackOverflow:
// http://stackoverflow.com/questions/29900928

// ----------------------------------------------------------------

// Forward declaration of the implementation function.
template <typename, typename>
struct forNArgsImpl;

// Main `forNArgs` function - this will be called by the user.
template <std::size_t N, typename TF, typename... Ts>
void forNArgs(TF mFn, Ts... mXs)
{
    // Check if the number of passed arguments is divisible by `N`.
    static_assert(sizeof...(Ts) % N == 0, "Wrong number of arguments");

    // Call the implementation function with...
    forNArgsImpl<
        // ...a sequence from `0` to the number of `mFn`
        // calls that will be made.
        // ("number of args" divided by `N`)
        std::make_index_sequence<sizeof...(Ts) / N>,

        // ...a sequence from `0` to `N`
        // ("size of a group of arguments")
        std::make_index_sequence<N>>::
        exec(
            // Pass the callable object to the implementation.
            mFn,

            // Forward the passed arguments as an `std::tuple`.
            std::forward_as_tuple(mXs...));
}

// Specialization of the implementation function that expands
// the index sequences into variadic `std::size_t` packs.
template <std::size_t... Bs, std::size_t... Cs>
struct forNArgsImpl<
    // `Bs...` goes from `0` to the number of function calls.
    // ("number of args" divided by `N`)
    std::index_sequence<Bs...>,

    // `Cs...` goes from `0` to the number of arguments per
    // function call (`N`).
    std::index_sequence<Cs...>>
{
    template <typename TF, typename... Ts>
    static void exec(TF mFn, const std::tuple<Ts...>& mXs)
    {
        // `swallow` is a nice and readable way of creating a
        // context for parameter expansion, like `initializer_list`.
        using swallow = bool[];

        (void)swallow{// `Bs...` is the sequence we are expanding here.

            // The code inside `swallow` gets expanded to
            // the number of function calls previously calculated.
            // ("number of args" divided by `N`)
            (execN<Bs * sizeof...(Cs)>(mFn, mXs), true)...};

        /*
            // Example expansion for:
            forNArgs<2>([](auto x, auto y){ }, 0, 1, 2, 3);

            (void) swallow
            {
                (execN<0 * 2>(fn, tpl), true),
                (execN<1 * 2>(fn, tpl), true)
            };
        */
    }

    // `execN` simply calls the function getting the correct
    // elements from the tuple containing the forwarded arguments.
    template <std::size_t N, typename TF, typename... Ts>
    static void execN(TF mFn, const std::tuple<Ts...>& mXs)
    {
        // `N`, in this context, is the "base index" of the current
        // function call.

        // `Cs...` gets expanded from 0 to the number of arguments per
        // function call (`N`).
        mFn(std::get<N + Cs>(mXs)...);

        /*
            // Example expansion for `swallow` elements of:
            forNArgs<2>([](auto x, auto y){ }, 0, 1, 2, 3);

            (execN<0 * 2>(fn, tpl), true)
            // ...expands to...
            fn
            (
                std::get<0 + 0>(tpl),	// 0
                std::get<0 + 1>(tpl)	// 1
            );

            (execN<1 * 2>(fn, tpl), true)
            // ...expands to...
            fn
            (
                std::get<2 + 0>(tpl),	// 2
                std::get<2 + 1>(tpl)	// 3
            );
        */
    }
};

int main()
{
    // Prints "2 4 6 8".
    forNArgs<2>(
        [](auto x, auto y)
        {
            std::cout << x * y << " ";
        },

        2, 1, // 2 * 1 = 2
        2, 2, // 2 * 2 = 4
        2, 3, // 2 * 3 = 6
        2, 4  // 2 * 4 = 8
        );

    std::cout << "\n";

    // Prints "3 6 9 12".
    forNArgs<3>(
        [](auto x, auto y, auto z)
        {
            std::cout << x + y + z << " ";
        },

        1, 1, 1, // 1 + 1 + 1 = 3
        2, 2, 2, // 2 + 2 + 2 = 6
        3, 3, 3, // 3 + 3 + 3 = 9
        4, 4, 4  // 4 + 4 + 4 = 12
        );

    return 0;
}