//// Stolen from https://github.com/roger-dv/cpp20-coro-generator/blob/8c425b5d853419ea43da68cfa7880fce8784840c/generator.h
//
////
//// Created by rogerv on 9/29/19.
////
//// Based on example code (but with significant cleanup) found in:
//// Rainer Grimm, Concurrency with Modern C++ (Leanpub, 2017 - 2019), 207-209.
////
//
//#pragma once
//
//#include <experimental/coroutine>
//#include <memory>
//#include <iostream>
//
//namespace std {
//
//    template<typename T>
//    class generator {
//    public:
//        struct promise_type;
//        using handle_type = std::experimental::coroutine_handle<promise_type>;
//    private:
//        handle_type coro;
//    public:
//        explicit generator(handle_type h) : coro(h) {}
//        generator(const generator &) = delete;
//        generator(generator &&oth) noexcept : coro(oth.coro) {
//            oth.coro = nullptr;
//        }
//        generator &operator=(const generator &) = delete;
//        generator &operator=(generator &&other) noexcept {
//            coro = other.coro;
//            other.coro = nullptr;
//            return *this;
//        }
//        ~generator() {
//            if (coro) {
//                coro.destroy();
//            }
//        }
//
//        bool next() {
//            coro.resume();
//            return not coro.done();
//        }
//
//        T getValue() {
//            return coro.promise().current_value;
//        }
//
//        struct promise_type {
//        private:
//            T current_value{};
//            friend class generator;
//        public:
//            promise_type() = default;
//            ~promise_type() = default;
//            promise_type(const promise_type&) = delete;
//            promise_type(promise_type&&) = delete;
//            promise_type &operator=(const promise_type&) = delete;
//            promise_type &operator=(promise_type&&) = delete;
//
//            auto initial_suspend() {
//                return std::experimental::suspend_always{};
//            }
//
//            auto final_suspend() noexcept {
//                return std::experimental::suspend_always{};
//            }
//
//            auto get_return_object() {
//                return generator{handle_type::from_promise(*this)};
//            }
//
//            auto return_void() {
//                return std::experimental::suspend_never{};
//            }
//
//            auto yield_value(T some_value) {
//                current_value = some_value;
//                return std::experimental::suspend_always{};
//            }
//
//            void unhandled_exception() {
//                std::exit(1);
//            }
//        };
//    };
//
//}

// This was stolen from https://github.com/lewissbaker/cppcoro/blob/master/include/cppcoro/generator.hpp
///////////////////////////////////////////////////////////////////////////////
// Copyright (c) Lewis Baker
// Licenced under MIT license. See LICENSE.txt for details.
///////////////////////////////////////////////////////////////////////////////
#pragma once

#include <experimental/coroutine>
#include <type_traits>
#include <utility>
#include <exception>
#include <iterator>
#include <functional>

namespace std
{
    template<typename T>
    class generator;

    namespace detail
    {
        template<typename T>
        class generator_promise
        {
        public:

            using value_type = std::remove_reference_t<T>;
            using reference_type = std::conditional_t<std::is_reference_v<T>, T, T&>;
            using pointer_type = value_type*;

            generator_promise() = default;

            generator<T> get_return_object() noexcept;

            constexpr std::experimental::suspend_always initial_suspend() const noexcept { return {}; }
            constexpr std::experimental::suspend_always final_suspend() const noexcept { return {}; }

            template<
                    typename U = T,
                    std::enable_if_t<!std::is_rvalue_reference<U>::value, int> = 0>
            std::experimental::suspend_always yield_value(std::remove_reference_t<T>& value) noexcept
            {
                m_value = std::addressof(value);
                return {};
            }

            std::experimental::suspend_always yield_value(std::remove_reference_t<T>&& value) noexcept
            {
                m_value = std::addressof(value);
                return {};
            }

            void unhandled_exception()
            {
                m_exception = std::current_exception();
            }

            void return_void()
            {
            }

            reference_type value() const noexcept
            {
                return static_cast<reference_type>(*m_value);
            }

            // Don't allow any use of 'co_await' inside the generator coroutine.
            template<typename U>
            std::experimental::suspend_never await_transform(U&& value) = delete;

            void rethrow_if_exception()
            {
                if (m_exception)
                {
                    std::rethrow_exception(m_exception);
                }
            }

        private:

            pointer_type m_value;
            std::exception_ptr m_exception;

        };

        struct generator_sentinel {};

        template<typename T>
        class generator_iterator
        {
            using coroutine_handle = std::experimental::coroutine_handle<generator_promise<T>>;

        public:

            using iterator_category = std::input_iterator_tag;
            // What type should we use for counting elements of a potentially infinite sequence?
            using difference_type = std::ptrdiff_t;
            using value_type = typename generator_promise<T>::value_type;
            using reference = typename generator_promise<T>::reference_type;
            using pointer = typename generator_promise<T>::pointer_type;

            // Iterator needs to be default-constructible to satisfy the Range concept.
            generator_iterator() noexcept
                    : m_coroutine(nullptr)
            {}

            explicit generator_iterator(coroutine_handle coroutine) noexcept
                    : m_coroutine(coroutine)
            {}

            friend bool operator==(const generator_iterator& it, generator_sentinel) noexcept
            {
                return !it.m_coroutine || it.m_coroutine.done();
            }

            friend bool operator!=(const generator_iterator& it, generator_sentinel s) noexcept
            {
                return !(it == s);
            }

            friend bool operator==(generator_sentinel s, const generator_iterator& it) noexcept
            {
                return (it == s);
            }

            friend bool operator!=(generator_sentinel s, const generator_iterator& it) noexcept
            {
                return it != s;
            }

            generator_iterator& operator++()
            {
                m_coroutine.resume();
                if (m_coroutine.done())
                {
                    m_coroutine.promise().rethrow_if_exception();
                }

                return *this;
            }

            // Need to provide post-increment operator to implement the 'Range' concept.
            void operator++(int)
            {
                (void)operator++();
            }

            reference operator*() const noexcept
            {
                return m_coroutine.promise().value();
            }

            pointer operator->() const noexcept
            {
                return std::addressof(operator*());
            }

        private:

            coroutine_handle m_coroutine;
        };
    }

    template<typename T>
    class [[nodiscard]] generator
    {
    public:

        using promise_type = detail::generator_promise<T>;
        using iterator = detail::generator_iterator<T>;

        generator() noexcept
                : m_coroutine(nullptr)
        {}

        generator(generator&& other) noexcept
                : m_coroutine(other.m_coroutine)
        {
            other.m_coroutine = nullptr;
        }

//         TODO: DO WE NEED THIS? With it, it's impossible to use Corountine Runner
                generator(const generator& other) : m_coroutine(other.m_coroutine) {} //= delete;

        ~generator()
        {
            if (m_coroutine)
            {
                m_coroutine.destroy();
            }
        }

        generator& operator=(generator other) noexcept
        {
            swap(other);
            return *this;
        }

        iterator begin()
        {
            if (m_coroutine)
            {
                m_coroutine.resume();
                if (m_coroutine.done())
                {
                    m_coroutine.promise().rethrow_if_exception();
                }
            }

            return iterator{ m_coroutine };
        }

        detail::generator_sentinel end() noexcept
        {
            return detail::generator_sentinel{};
        }

        void swap(generator& other) noexcept
        {
            std::swap(m_coroutine, other.m_coroutine);
        }

    private:

        friend class detail::generator_promise<T>;

        explicit generator(std::experimental::coroutine_handle<promise_type> coroutine) noexcept
                : m_coroutine(coroutine)
        {}

        std::experimental::coroutine_handle<promise_type> m_coroutine;

    };

    template<typename T>
    void swap(generator<T>& a, generator<T>& b)
    {
        a.swap(b);
    }

    namespace detail
    {
        template<typename T>
        generator<T> generator_promise<T>::get_return_object() noexcept
        {
            using coroutine_handle = std::experimental::coroutine_handle<generator_promise<T>>;
            return generator<T>{ coroutine_handle::from_promise(*this) };
        }
    }

    template<typename FUNC, typename T>
    generator<std::invoke_result_t<FUNC&, typename generator<T>::iterator::reference>> fmap(FUNC func, generator<T> source)
    {
        for (auto&& value : source)
        {
            co_yield std::invoke(func, static_cast<decltype(value)>(value));
        }
    }
}


