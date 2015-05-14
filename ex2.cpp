/*
	"`for_each_arg` explained and expanded"
	C++Now 2015 Lightning Talk
	- Vittorio Romeo

	http://vittorioromeo.info
	vittorio.romeo@outlook.com
*/

#include <iostream>
#include <vector>

template<typename TF, typename... Ts>
void forArgs(TF&& mFn, Ts&&... mArgs)
{
	return (void) std::initializer_list<int>
	{
		(
			mFn(std::forward<Ts>(mArgs)),
			0
		)...
	};
}

// Example use case: `make_vector` function.

// `make_vector` will take one or more arguments of the same type
// and return an `std::vector` containing them.

// We'll pass the first parameter outside of the variadic pack
// to help us deduce the type of the arguments.

template<typename T, typename... TArgs>
auto make_vector(T&& mArg, TArgs&&... mArgs)
{
	std::vector<T> result;
	result.reserve(1 + sizeof...(TArgs));

	forArgs
	(
		[&result](auto&& x)
		{
			result.emplace_back(std::forward<decltype(x)>(x));
		},

		std::forward<T>(mArg),
		std::forward<TArgs>(mArgs)...
	);

	return result;
}

int main()
{
	// Prints "12345".

	auto v(make_vector(1, 2, 3, 4, 5));
	for(const auto& x : v) std::cout << x;

	return 0;
}