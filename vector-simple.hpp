#pragma once

#include <cassert>
#include <utility>
#include <algorithm>

#include <memory>
#include <new>

namespace nonstd {
    namespace Detail {

        template <typename T>
        T* allocate_uninit(size_t count) {
            return reinterpret_cast<T*>(::operator new[](sizeof(T)* count, std::align_val_t{ alignof(T) }));
        }
        template <typename T>
        void deallocate_no_destroy(T* ptr, size_t num_elements) {
            ::operator delete[](static_cast<void*>(ptr), num_elements * sizeof(T), std::align_val_t{ alignof(T) });
        }

        template <typename T>
        void destroy_range_reverse(T* start, T* end) {
            // Note: Compiler should be able to figure this out its own,
            //       but better make sure.
            if constexpr (!std::is_trivially_destructible_v<T>) {
                while (end != start) {
                    --end;
                    std::destroy_at(end);
                }
            }
        }

        template <typename T, typename... Args>
        inline void construct_at(T* const ptr, Args&&... args) {
            ::new (static_cast<void*>(ptr)) T(std::forward<Args>(args)...);
        }

    } // end namespace Detail

    template <typename T>
    class vector {
        T* m_first = nullptr;
        T* m_next = nullptr;
        T* m_end = nullptr;

        size_t calculate_new_cap(const size_t new_cap) {
            const auto old_cap = capacity();
            const auto geometric_cap = old_cap + old_cap / 2;
            return std::max(new_cap, geometric_cap);
        }

        void adopt_new_memory(T* new_memory, size_t new_capacity) {
            const auto old_size = size();
            m_next = std::uninitialized_move(m_first, m_next, new_memory);
            Detail::deallocate_no_destroy(m_first, old_size);
            m_first = new_memory;
            m_end = m_first + new_capacity;
        }

        static_assert(std::is_nothrow_move_constructible_v<T>, "No.");

    public:
        using iterator = T*;
        using const_iterator = T const*;
        vector() = default;
        ~vector() {
            Detail::destroy_range_reverse(m_first, m_next);
            Detail::deallocate_no_destroy(m_first, size());
        }

        // We don't need these for benchmarks/tests
        vector(vector const& rhs) : vector() {
            reserve(rhs.size());
            std::uninitialized_copy(rhs.begin(), rhs.end(), m_first);
        }
        vector& operator=(vector const& rhs) {
            Detail::destroy_range_reverse(m_first, m_next);
            if (rhs.size() > capacity()) {
                Detail::deallocate_no_destroy(m_first, size());
                m_first = Detail::allocate_uninit<T>(rhs.size());
                m_end = m_first + rhs.size();
            }
            m_next = std::uninitialized_copy(rhs.m_first, rhs.m_next, m_first);
            return *this;
        }
        vector(vector&& rhs) noexcept :
            m_first(std::exchange(rhs.m_first, nullptr)),
            m_next(std::exchange(rhs.m_next, nullptr)),
            m_end(std::exchange(rhs.m_end, nullptr))
        {}

        vector& operator=(vector&& rhs) noexcept {
            auto temp(std::move(rhs));
            swap(*this, temp);
            return *this;
        }

        void push_back(T const& elem) {
            // We duplicate the construction code so that the fast path
            // is easier to optimize for the compiler.
            if (m_next != m_end) {
                Detail::construct_at(m_next, elem);
                ++m_next;
                return;
            }
            const auto current_size = size();
            const auto new_cap = calculate_new_cap(current_size + 1);
            auto* new_memory = Detail::allocate_uninit<T>(new_cap);
            // Constructing new element has to happen before we move
            // old elements, in case someone is doing `v.push_back(v[0])`
            try {
                Detail::construct_at(new_memory + current_size, elem);
            }
            catch (...) {
                Detail::deallocate_no_destroy(new_memory, new_cap);
                // rethrow after fixup, so user knows it happened
                throw;
            }
            adopt_new_memory(new_memory, new_cap);
            // Account for the inserted element
            ++m_next;
        }
        void push_back(T&& elem) {
            // We duplicate the construction code so that the fast path
            // is easier to optimize for the compiler.
            if (m_next != m_end) {
                Detail::construct_at(m_next, std::move(elem));
                ++m_next;
                return;
            }
            const auto current_size = size();
            const auto new_cap = calculate_new_cap(current_size + 1);
            auto* new_memory = Detail::allocate_uninit<T>(new_cap);
            // Constructing new element has to happen before we move
            // old elements, in case someone is doing `v.push_back(v[0])`
            // We do not have to catch exception here, as we static assert
            // nothrow move constructibility on our elements.
            Detail::construct_at(new_memory + current_size, std::move(elem));
            adopt_new_memory(new_memory, new_cap);
            // Account for the element inserted ahead of time
            ++m_next;
        }
        void push_back_unchecked(T const& elem) noexcept(std::is_nothrow_copy_constructible_v<T>) {
            assert(m_next != m_end);
            Detail::construct_at(m_next, elem);
            ++m_next;
        }
        void push_back_unchecked(T&& elem) noexcept {
            assert(m_next != m_end);
            Detail::construct_at(m_next, std::move(elem));
            ++m_next;
        }

        void reserve(size_t target_capacity) {
            if (target_capacity <= capacity()) { return; }
            const auto new_cap = calculate_new_cap(target_capacity);
            auto new_memory = Detail::allocate_uninit<T>(new_cap);
            adopt_new_memory(new_memory, new_cap);
        }

        friend void swap(vector& lhs, vector& rhs) noexcept {
            std::swap(lhs.m_first, rhs.m_first);
            std::swap(lhs.m_next, rhs.m_next);
            std::swap(lhs.m_end, rhs.m_end);
        }

        T& operator[](size_t idx) {
            return m_first[idx];
        }
        T const& operator[](size_t idx) const {
            return m_first[idx];
        }

        size_t size() const {
            return m_next - m_first;
        }
        size_t capacity() const {
            return m_end - m_first;
        }

        iterator begin() { return m_first; }
        const_iterator begin() const { return m_first; }
        iterator end() { return m_next; }
        const_iterator end() const { return m_next; }
    };

} // end nonstd namespace
