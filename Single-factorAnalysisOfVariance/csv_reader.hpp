#pragma ones

#include <fstream>
#include <iostream>
#include <vector>
#include <limits>

#include "csv_default_parsers.hpp"


template <
	  template <class...> 
	  class ColParsersGetter, 
	  template<class RowType, class ColParsers> 
	  class RowParser,
	  class ... Args
>
class ParametrizedCsvReader
{
public:
	ParametrizedCsvReader() = default;
	ParametrizedCsvReader(const std::string &filename);
	
	using row_type = std::tuple<Args ...>;
	using part_parsers_type = std::tuple<std::function<Args(const std::string &)>...>;
	using row_parser_type = RowParser<row_type, part_parsers_type>;

	void open(const std::string &filename);
	bool is_open() const;

	void set_ignore_bad_row_len(bool value);
	void set_skip_empty_lines(bool value);
	void set_skip_first_lines(const std::size_t &value);

	row_type operator[] (std::size_t i);
	const row_type operator[] (std::size_t i) const;

	std::size_t rows() const;
	std::size_t read_rows(std::size_t n = std::numeric_limits<std::size_t>::max());

	void close();

private:
	bool m_ignore_bad_row_len = false;
	bool m_skip_empty_lines = true;
	bool m_first_lines_skipped = false;

	static constexpr char m_separator = ',';
	static constexpr char m_quote = '"';
	static constexpr char m_escape = '\\';

	std::size_t m_skip_first_lines = 1u;

	std::ifstream m_ifs;
	std::vector<row_type> m_rows;

	part_parsers_type m_part_parsers = ColParsersGetter<Args...>::get();
	row_parser_type m_row_parser;
};

template <class ... Args>
using CsvReader = ParametrizedCsvReader<DefaultColParsersGetter, DefaultRowParser , Args...>;

template <
	  template <class...> 
	  class ColParsersGetter, 
	  template<class RowType, class ColParsers> 
	  class RowParser,
	  class ... Args
>
ParametrizedCsvReader<ColParsersGetter, RowParser, Args...>::ParametrizedCsvReader(const std::string &filename)
	: m_ifs(filename)
{}

template <
	  template <class...> 
	  class ColParsersGetter, 
	  template<class RowType, class ColParsers> 
	  class RowParser,
	  class ... Args
>
void ParametrizedCsvReader<ColParsersGetter, RowParser, Args...>::open(const std::string &filename) 
{
	m_ifs.open(filename);
}

template <
	  template <class...> 
	  class ColParsersGetter, 
	  template<class RowType, class ColParsers> 
	  class RowParser,
	  class ... Args
>
bool ParametrizedCsvReader<ColParsersGetter, RowParser, Args...>::is_open() const
{
	return m_ifs.is_open();
}

template <
	  template <class...> 
	  class ColParsersGetter, 
	  template<class RowType, class ColParsers> 
	  class RowParser,
	  class ... Args
>
void ParametrizedCsvReader<ColParsersGetter, RowParser, Args...>::set_ignore_bad_row_len(bool value)
{
	m_ignore_bad_row_len = value;
}

template <
	  template <class...> 
	  class ColParsersGetter, 
	  template<class RowType, class ColParsers> 
	  class RowParser,
	  class ... Args
>
void ParametrizedCsvReader<ColParsersGetter, RowParser, Args...>::set_skip_empty_lines(bool value)
{
	m_skip_empty_lines = value;
}

template <
	  template <class...> 
	  class ColParsersGetter, 
	  template<class RowType, class ColParsers> 
	  class RowParser,
	  class ... Args
>
void ParametrizedCsvReader<ColParsersGetter, RowParser, Args...>::set_skip_first_lines(const std::size_t &value)
{
	m_skip_first_lines = value;
}

template <
	  template <class...> 
	  class ColParsersGetter, 
	  template<class RowType, class ColParsers> 
	  class RowParser,
	  class ... Args
>
typename ParametrizedCsvReader<ColParsersGetter, RowParser, Args...>::row_type
	ParametrizedCsvReader<ColParsersGetter, RowParser, Args...>::operator[] (std::size_t i)
{
	return m_rows[i];
}

template <
	  template <class...> 
	  class ColParsersGetter, 
	  template<class RowType, class ColParsers> 
	  class RowParser,
	  class ... Args
>
const typename ParametrizedCsvReader<ColParsersGetter, RowParser, Args...>::row_type
	ParametrizedCsvReader<ColParsersGetter, RowParser, Args...>::operator[] (std::size_t i) const
{
	return m_rows[i];
}

template <
	  template <class...> 
	  class ColParsersGetter, 
	  template<class RowType, class ColParsers> 
	  class RowParser,
	  class ... Args
>
size_t ParametrizedCsvReader<ColParsersGetter, RowParser, Args...>::rows() const
{
	return m_rows.size();
}

template <
	  template <class...> 
	  class ColParsersGetter, 
	  template<class RowType, class ColParsers> 
	  class RowParser,
	  class ... Args
>
void ParametrizedCsvReader<ColParsersGetter, RowParser, Args...>::close() 
{
	m_ifs.close();
}

int main()
{
	CsvReader <int, double, float, std::string> reader;;
	DefaultColParsersGetter<int, double, float, std::string> gpt;

	auto prs = gpt.get();

	DefaultRowParser<CsvReader<int, double>::row_type, CsvReader<int, double>::part_parsers_type> rp;

	CsvReader<int, double>::row_type parsedRow;

	std::array<std::string,2> strArr; 

	std::string pi = "3.14";

	std::cout << "pi " << pi << std::endl;

	strArr[0] = std::string("10000");
	strArr[1] = pi;

		for ( auto str : strArr )
		{
		std::cout << str << std::endl;
		}
	rp.parse(parsedRow, DefaultColParsersGetter<int, double>::get(), strArr);


	std::cout << std::get<0>(parsedRow) << std::endl;
	std::cout << std::get<1>(parsedRow) << std::endl;
#ifdef DEBUG
	std::cout << __PRETTY_FUNCTION__ << std::endl << typeid(std::get<0>(parsedRow)).name() << " " << std::get<0>(parsedRow) << std::endl;
	std::cout << __PRETTY_FUNCTION__ << std::endl << typeid(std::get<1>(parsedRow)).name() << " " << std::get<1>(parsedRow) << std::endl;
	std::cout << __PRETTY_FUNCTION__ << std::endl << typeid(std::get<0>(prs)).name() << std::endl;
	std::cout << __PRETTY_FUNCTION__ << std::endl << typeid(std::get<1>(prs)).name() << std::endl;
	std::cout << __PRETTY_FUNCTION__ << std::endl << typeid(std::get<2>(prs)).name() << std::endl;
	std::cout << __PRETTY_FUNCTION__ << std::endl << typeid(std::get<3>(prs)).name() << std::endl;
#endif

	return 0;
}




