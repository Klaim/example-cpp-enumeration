#include <vector>
#include <format>
#include <iostream>

#include <concepts>
#include <ranges>
#include <optional>
#include <algorithm>

namespace mine
{

    template< std::input_or_output_iterator I, std::sentinel_for<I> S >
    struct enumerating_iterator
    {
        using this_type = enumerating_iterator<I, S>;
        using difference_type	= std::ptrdiff_t;
        using value_type	= std::pair< std::size_t, typename I::value_type>;
        using pointer	= value_type*;
        using reference	= value_type&;
        using iterator_category	= std::input_iterator_tag;


        std::size_t idx = 0;
        std::optional< std::ranges::subrange<I,S> > range;


        // end sentinel
        enumerating_iterator() = default;

        // copy and move
        enumerating_iterator(const enumerating_iterator&) = default;
        enumerating_iterator(enumerating_iterator&&) = default;
        enumerating_iterator& operator=(const enumerating_iterator&) = default;
        enumerating_iterator& operator=(enumerating_iterator&&) = default;

        // enumerate a given range
        enumerating_iterator(I begin, S end)
            : range({ begin,  end })
        {

        }

        auto operator*() const
        {
            return std::make_pair(idx, *range->begin());
        }

        this_type& operator++()
        {
            if(range)
            {
                ++idx;
                range->advance(1);
            }
            return *this;
        }

        this_type& operator++(int)
        {
            if(range)
            {
                idx++;
                range->advance(1);
            }
            return *this;
        }

        this_type& operator--()
        {
            if(range)
            {
                --idx;
                range->advance(-1);
            }
            return *this;
        }

        this_type& operator--(int)
        {
            if(range)
            {
                idx--;
                range->advance(-1);
            }
            return *this;
        }

        bool operator==(const this_type& other) const
        {
            if(not other.range) // other is a sentinel
            {
                if(this->range)
                    return this->range->begin() == this->range->end(); // see if we reached the end already
                else
                    return true; // both iterators are sentinels
            }

            // otherwise just make sure we contain the same exact range and enumeration
            return  this->range
                and other.idx == this->idx
                and other.range->begin() == this->range->begin()
                and other.range->end() == this->range->end()
            ;
        }

        bool operator!=(const this_type& other) const
        {
            return not (*this == other);
        }

    };


    template<std::ranges::range R>
    auto enumerate(R&& r)
    {
        using std::begin;
        using std::end;
        using I = std::remove_cvref_t<decltype(begin(r))>;
        using S = std::remove_cvref_t<decltype(end(r))>;
        using enumerator = enumerating_iterator<I, S>;
        static_assert(std::input_or_output_iterator<enumerator>);

        return std::ranges::subrange(enumerator{ begin(r), end(r) }, enumerator{});
    }

    template<std::input_or_output_iterator I, std::sentinel_for<I> S>
    auto enumerate(I begin, S end)
    {
        using enumerator = enumerating_iterator<I, S>;
        static_assert(std::input_or_output_iterator<enumerator>);

        return std::ranges::subrange(enumerator{ begin, end }, enumerator{});
    }

}


int main()
{
    const std::vector values { 123, 0, 1, 2, 3, 4, 5, 42 };
    for(const auto& [idx, value] : mine::enumerate(values))
        std::cout << std::format("{} -> {}\n", idx, value);

    std::cout << "\nreversed:\n";
    for(const auto& [idx, value] : mine::enumerate(values.rbegin(), values.rend()))
        std::cout << std::format("{} -> {}\n", idx, value);

    std::cout << "\nnot the boundaries:\n";
    for(const auto& [idx, value] : mine::enumerate(next(values.begin()), prev(values.end())))
        std::cout << std::format("{} -> {}\n", idx, value);

    std::cout << "\ntemporary container:\n";
    for(const auto& [idx, value] : mine::enumerate(std::vector{ -1, -2, -3, -4 }))
        std::cout << std::format("{} -> {}\n", idx, value);


}


