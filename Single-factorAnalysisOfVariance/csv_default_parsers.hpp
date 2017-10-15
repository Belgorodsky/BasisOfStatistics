#pragma ones

#include <functional>
#include <sstream>
#include <tuple>
#include <type_traits>
#ifdef DEBUG
#include <typeinfo>
#include <iostream>
#endif

/***********************************************DefaultRowParser**********************************************/
// Row is std::tuple<Args ...>


template <class Row, class ColParsers, std::size_t N>
struct DefaultRowParserImpl;

template <class Row, class ColParsers>
struct DefaultRowParser
{
	static void parse(
		Row &row,
		const ColParsers &parsers,
		const std::array<
			std::string,
			std::tuple_size<ColParsers>::value
		> &parts
	)
	{
#ifdef DEBUG
		std::cout << __PRETTY_FUNCTION__ <<  std::endl;
		for ( auto& str : parts )
		{
		std::cout << str << std::endl;
		}
#endif 
		DefaultRowParserImpl<Row, ColParsers, std::tuple_size<ColParsers>::value>::doParse(row, parsers, parts);
	}

};


template <class Row, class ColParsers, std::size_t N>
struct DefaultRowParserImpl
{
	static void doParse(
		Row &row,
		const ColParsers &parsers,
		const std::array<
			std::string,
			std::tuple_size<ColParsers>::value
		> &parts
	)
	{
#ifdef DEBUG	
			std::cout << __PRETTY_FUNCTION__ << "parts[N-1]: '" << parts[N-1] << "'" << std::endl;
#endif 
#if __cplusplus >= 201700
		if constexpr (1 < N)
		{
			std::get<N-1>(row) = std::get<N-1>(parsers)(parts[N-1]);
			DefaultRowParserImpl<Row, ColParsers, N-1>::doParse(row, parsers, parts);
		}
		if constexpr (1 == N)
		{
			std::get<N-1>(row) = std::get<N-1>(parsers)(parts[N-1]);
		}
	}
#else
		std::get<N-1>(row) = std::get<N-1>(parsers)(parts[N-1]);
		DefaultRowParserImpl<Row, ColParsers, N-1>::doParse(row, parsers, parts);
	}
};

template <class Row, class ColParsers>
struct DefaultRowParserImpl<Row, ColParsers, 1>
{
	static void doParse(
		Row &row,
		const ColParsers &parsers,
		const std::array<
			std::string,
			std::tuple_size<ColParsers>::value
		> &parts
	)
	{
#ifdef DEBUG	
			std::cout << __PRETTY_FUNCTION__ << "parts[1-1]: '" << parts[1-1] << "'" << std::endl;
#endif 
		std::get<0>(row) = std::get<0>(parsers)(parts[0]);
	}
#endif
};

/*******************************************DefaultColParsersGetter*******************************************/

template <class ... Args>
struct DefaultColParsersGetter;

template <class T>
struct DefaultPartParser
{
#ifdef DEBUG	
	DefaultPartParser() 
	{
		std::cout << __PRETTY_FUNCTION__ << std::endl;
	}
#endif 

	static T parse(const std::string &str)
	{
#ifdef DEBUG	
		std::cout << __PRETTY_FUNCTION__ << " str: '" << str << "'"<< std::endl;
#endif 
		T t;
		if (!str.empty())
		{
			std::istringstream iss(str);
			iss >> t;

			if (iss.fail())
				throw std::runtime_error("Unsupported format of '" + str + "' (fail bit set)");
		}

		return t;
	}
};

template <>
struct DefaultPartParser<std::string>
{
#ifdef DEBUG	
	DefaultPartParser() 
	{
		std::cout << __PRETTY_FUNCTION__ << std::endl;
	}
#endif 

	static std::string parse(const std::string &str)
	{
#ifdef DEBUG	
		std::cout << __PRETTY_FUNCTION__ << " str: '" << str << "'"<< std::endl;
#endif 
		return str;
	}
};

template <class T>
struct DefaultColParsersGetter<T>
{
	using parsers_type = std::tuple<std::function<T(const std::string &)>>;

	static parsers_type get() {
		return parsers_type(DefaultPartParser<T>::parse);
	}
};

template <class T, class ... Args>
struct DefaultColParsersGetter<T, Args...>
{
#ifdef DEBUG	
	DefaultColParsersGetter() 
	{
		std::cout << __PRETTY_FUNCTION__ << std::endl << std::endl << typeid(parsers_type).name() << std::endl;
	}
#endif 

	using parsers_type = std::tuple<
		std::function<T(const std::string &)>,
		std::function<Args (const std::string &)>...
	>;

	static parsers_type get() {
		return 
			std::tuple_cat(
				DefaultColParsersGetter<T>::get(),
				DefaultColParsersGetter<Args...>::get()
			);
	}
};

/************************************************************************************************************/
