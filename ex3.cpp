/*
	"`for_each_arg` explained and expanded"
	C++Now 2015 Lightning Talk
	- Vittorio Romeo

	http://vittorioromeo.info
	vittorio.romeo@outlook.com
*/

#include <iostream>
#include <tuple>

template<typename TF, typename... Ts>
void forArgs(TF&& mFn, Ts&&... mArgs)
{
	return (void) std::initializer_list<int>
	{
		(
			std::forward<TF>(mFn)(std::forward<Ts>(mArgs)),
			0
		)...
	};
}

// Example use case: `forTuple` function.

// We can use `forArgs` as a building block for an `std::tuple`
// element iteration function. 

// To do so, we require an helper function that expands the
// elements of the tuple into a function call.

// The following helper function is taken from paper N3802,
// proposed by Peter Sommerlad. 

// Many similar implementations of the same function can be
// easily found online.

// ----------------------------------------------------------------

using namespace std;

template<typename F, typename Tuple, size_t... I>
decltype(auto) apply_impl(F&& f, Tuple&& t, index_sequence<I...>)
{
	return forward<F>(f)(get<I>(forward<Tuple>(t))...);
}
template<typename F, typename Tuple>
decltype(auto) apply(F&& f, Tuple&& t)
{
	using Indices =
		make_index_sequence<tuple_size<decay_t<Tuple>>::value>;
	return apply_impl(forward<F>(f), forward<Tuple>(t), Indices{});
}

// ----------------------------------------------------------------

// `forTuple` is a function that takes a callable object and
// and `std::tuple` as its parameters.

// It then calls the passed function individually passing every
// element of the tuple as its argument.

template<typename TFn, typename TTpl>
void forTuple(TFn&& mFn, TTpl&& mTpl)
{
	// We basically expand the tuple into a function call to
	// a variadic polymorphic lambda with `apply`, which in
	// turn passes the expanded tuple elements to `forArgs`,
	// one by one... which in turn calls `mFn` with every
	// single tuple element individually.

	apply
	(
		[&mFn](auto&&... xs)
		{
			forArgs
			(
				std::forward<TFn>(mFn),
				std::forward<decltype(xs)>(xs)...
			);
		},
		std::forward<TTpl>(mTpl)
	);
}

int main()
{
	// Prints "10 hello 15 c".

	forTuple
	(
		[](const auto& x){ std::cout << x << " "; },
		std::make_tuple(10, "hello", 15.f, 'c')
	);

	return 0;
}