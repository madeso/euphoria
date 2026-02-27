#pragma once

#include <algorithm>
#include <sstream>
#include <string>

#include "tests/false_string.h"

namespace eu::tests
{

constexpr float machine_epsilon = 0.001f;

// mit from https://github.com/guilhermeagostinelli/levenshtein/blob/master/levenshtein.cpp
int levenshtein(const std::string& foo, const std::string& bar);

std::size_t find_index_of_first_diff(const std::string& first, const std::string& second);

template<typename T>
struct Equaler
{
	using Self = Equaler<T>;

	T lhs;
	T rhs;

	std::vector<std::string> failures;

	Equaler(T l, T r)
		: lhs(std::move(l))
		, rhs(std::move(r))
	{}

	template<typename F, typename C>
	Self& impl(
		const std::string& name,
		const F& convert,
		const C& compare
	)
	{
		const auto l = convert(lhs);
		const auto r = convert(rhs);
		const FalseString c = compare(l, r);
		if (c.IsTrue() == false)
		{
			std::ostringstream ss;
			ss << name << ": " << l << " != " << r;
			// hack: since empty string is true, let's use a single space as <no reason>
			if(c.reason != " ")
			{
				ss << " (" << *c.reason << ")";
			}
			failures.emplace_back(ss.str());
		}
		return *this;
	}

	template<typename C>
	Self& add(const std::string& name, const C& convert){
		return impl(name, convert, [](const auto& l, const auto& r) {
			const auto same = l == r;
			if (same) return FalseString::True();
			return FalseString::False(" ");
		});
	}

	Self& add(const std::string& name, std::string (*convert)(const T&))
	{
		return impl(name, convert, [](const std::string& l, const std::string& r)
		{
			const auto same = l == r;
			if (same) return FalseString::True();
			std::ostringstream ss;
			const auto edit = levenshtein(l, r);
			const auto diff_index = find_index_of_first_diff(l, r);
			ss << "length diff " << l.length() << " vs " << r.length() << ", changes=" << edit << ", first diff at "
			   << diff_index << " (" << l[diff_index] << " vs " << r[diff_index] << ")";
			return FalseString::False(ss.str());
		});
	}

	template<typename C, typename F>
	Self& add(const std::string& name, const C& convert, F min)
	{
		return impl(name, convert, [min](const auto& l, const auto& r) {
			const auto diff = std::abs(l - r);
			const auto close = diff < min;
			if (close) return FalseString::True();
			std::ostringstream ss;
			ss << "difference " << diff << " is larger than " << min;
			return FalseString::False(ss.str());
		});
	}

	[[nodiscard]] FalseString resolve() const
	{
		if (failures.empty())
		{
			return FalseString::True();
		}

		std::ostringstream ss;

		for (const auto& fa: failures)
		{
			ss << "  " << fa << '\n';
		}

		return FalseString::False(ss.str());
	}
};

}
