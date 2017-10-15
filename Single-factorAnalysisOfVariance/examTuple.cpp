
#include <functional>
#include <fstream>
#include <iostream>
#include <limits>
#include <sstream>
#include <tuple>
#include <vector>


template<class ... Args>
struct MultiplGet;

template <class T>
struct MultiplGet<T>
{
	using type = std::tuple<T>;

	static type get()
	{
		return std::make_tuple(T());
	}
};

template <class T, class ... Args>
struct MultiplGet<T, Args...>
{
	using type = std::tuple<T, Args...>;

	static type get()
	{
		return 
			std::tuple_cat(
				MultiplGet<T>::get()
				,
				MultiplGet<Args...>::get()
			);

	}
};


int main()
{
	MultiplGet<int, double, float, char> mg;

	mg.get();
}
