/*
	"`for_each_arg` explained and expanded"
	C++Now 2015 Lightning Talk
	- Vittorio Romeo

	http://vittorioromeo.info
	vittorio.romeo@outlook.com
*/

// The files used in this talk are available at:
/*
	https://github.com/SuperV1234/cppnow2015
*/

#include <iostream>

template<typename TF, typename... Ts>
void forArgs(TF&& mFn, Ts&&... mArgs)
{
	// We use an `initializer_list` to create a context
	// where variadic parameter expansion can take place.
	return (void) std::initializer_list<int>
	{
		// Every element of the `initializer_list` is an
		// expression enclosed in round parenthesis.
		(
			// In the parenthesis, the result of the `mFn`
			// function call is followed by the comma
			// operator and an integer (zero in this
			// case).

			// Thanks to the comma operator, the expression
			// evaluates to an (unused) integer, which is
			// accepted by the `initializer_list`.

			std::forward<TF>(mFn)
			(
				std::forward<Ts>(mArgs)
			),

			0
		)...
	};
}

/*

// The following `forArgs` call...

forArgs
(
	[](const auto& x){ std::cout << x << " "; },
	"hello",
	1,
	2,
	3
);

// ..roughly expands to...

(void) std::initializer_list<int>
{
	([](const auto& x){ std::cout << x; }("hello"), 0),
	([](const auto& x){ std::cout << x; }(1), 0),
	([](const auto& x){ std::cout << x; }(2), 0),
	([](const auto& x){ std::cout << x; }(3), 0)
}

// ...which is the same as writing...

std::cout << "hello";
std::cout << 1;
std::cout << 2;
std::cout << 3;

*/

int main()
{
	// Prints "hello 1 2 3".

	forArgs
	(
		[](const auto& x){ std::cout << x << " "; },
		"hello",
		1,
		2,
		3
	);

	return 0;
}