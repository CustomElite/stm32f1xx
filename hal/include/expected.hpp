#pragma once

#include <type_traits>
#include <utility>

template <typename E>
class unexpected {
public:
    constexpr explicit unexpected(E const& e) noexcept : mError(e) {}
    constexpr explicit unexpected(E && e) noexcept : mError(std::move(e)) {}

    constexpr E const& error() const noexcept { return mError; }
    constexpr E & error() noexcept { return mError; }

private:
    E mError;
};

template <typename E>
constexpr unexpected<typename std::decay_t<E>> MakeUnexpected(E&& e) {
    return unexpected<typename std::decay_t<E>>(std::forward<E>(e));
}

template <typename T, typename E>
class expected {
public:
    using value_type = T;
    using error_type = E;

    constexpr expected(T const& value) noexcept
        : mHasValue(true)
    {
        new (&mStorage.Value) T(value);
    }
    constexpr expected(T && value) noexcept
        : mHasValue(true)
    {
        new (&mStorage.Value) T(std::move(value));
    }
    constexpr expected(unexpected<E> const& err) noexcept
        : mHasValue(false)
    {
        new (&mStorage.Error) E(err.error());
    }
    constexpr expected(unexpected<E> && err) noexcept
        : mHasValue(false)
    {
        new (&mStorage.Error) E(std::move(err.error()));
    }
    expected(expected const& other) noexcept
        : mHasValue(other.mHasValue)
    {
        if (mHasValue)
            new (&mStorage.Value) T(other.mStorage.Value);
        else
            new (&mStorage.Error) E(other.mStorage.Error);
    }
    expected(expected && other) noexcept
        : mHasValue(other.mHasValue)
    {
        if (mHasValue)
            new (&mStorage.Value) T(std::move(other.mStorage.Value));
        else
            new (&mStorage.Error) E(std::move(other.mStorage.Error));
    }
    ~expected()
    {
        if (mHasValue)
            mStorage.Value.~T();
        else
            mStorage.Error.~E();
    }

    expected& operator=(expected const& other) noexcept
    {
        if (this != &other) {
            this->~expected();
            new (this) expected(other);
        }
        return *this;
    }
    expected& operator=(expected && other) noexcept
    {
        if (this != &other) {
            this->~expected();
            new (this) expected(std::move(other));
        }
        return *this;
    }
    [[nodiscard]] constexpr bool has_value() const { return mHasValue; }
    [[nodiscard]] constexpr explicit operator bool() const { return mHasValue; }

    [[nodiscard]] constexpr T const& value() const
    {
        return mStorage.Value;
    }
    [[nodiscard]] constexpr T& value() 
    {
        return mStorage.Value;
    }
    constexpr T const& operator*() const { return mStorage.Value; }
    constexpr T& operator*() { return mStorage.Value; }

    constexpr E const& error() const { return mStorage.Error; }
    constexpr E& error() { return mStorage.Error; }

    template <typename U>
    constexpr T value_or(U && default_value) const
    {
        return mHasValue ? mStorage.Value : static_cast<T>(std::forward<U>(default_value));
    }

private:
    union storage {
        T Value;
        E Error;

        storage() noexcept {}
        ~storage() noexcept {}
    } mStorage;

    bool mHasValue;
};

template <typename E>
class expected<void, E> {
public:
    using value_type = void;
    using error_type = E;
    
    constexpr expected(unexpected<E> const& err) noexcept
        : mHasValue(false)
    {
        new (&Error) E(err.error());
    }
    constexpr expected(unexpected<E> && err) noexcept
        : mHasValue(false)
    {
        new (&Error) E(std::move(err.error()));
    }
    ~expected() noexcept
    {
        if (not mHasValue)
            Error.~E();
    }

    constexpr bool has_value() const noexcept { return mHasValue; }
    constexpr explicit operator bool() const noexcept { return mHasValue; }
    constexpr E const& error() const noexcept { return Error; }
    constexpr E& error() noexcept { return Error; }

private:
    union {
        E Error;
    };
    bool mHasValue;
};