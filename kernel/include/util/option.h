#pragma once

/*
Option<T> - represents an item that be present or not.

NOTE:
Access methods do not check whether the value is present.
You have to check manually using has_value().
*/

template <typename T>
class Option {
public:
    constexpr Option() : exists(false) {}
    constexpr Option(T value) : value(value), exists(true) {}

    constexpr bool has_value() const {
        return exists;
    }

    constexpr T get_value() const {
        return value;
    }

    constexpr T* operator->() {
        return &value;
    }

    constexpr const T* operator->() const {
        return &value;
    }

private:
    T value;
    bool exists;
};

template <typename T>
class Option<const T&> {
public:
    constexpr Option() : ptr(0) {}
    constexpr Option(const T& value) : ptr(&value) {}

    constexpr bool has_value() const {
        return ptr != 0;
    }

    constexpr const T& get_value() const {
        return *ptr;
    }

    constexpr const T* operator->() const {
        return ptr;
    }

protected:
    const T* ptr;
};
