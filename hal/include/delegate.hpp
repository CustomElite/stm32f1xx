#pragma once

#include <type_traits>
#include <optional>

template <typename T, typename... tArgs>
concept cValidLambda = 
    std::is_class_v<T> and 
    std::is_invocable_v<T, tArgs...>;

template <typename T>
concept cValidReturn =
    std::is_void_v<T> or
    (std::is_trivial_v<T> and std::is_default_constructible_v<T>);

template <typename T>
class delegate;

template <cValidReturn tReturn, typename... tArgs>
class delegate<tReturn(tArgs...)> {
public:
    template <typename T, tReturn(T::*tMethod)(tArgs...)> 
    static constexpr delegate Create(T&& instance) = delete;

    template <typename T, tReturn(T::*tMethod)(tArgs...) const> 
    static constexpr delegate Create(T&& instance) = delete;
    
    template <tReturn(*tMethod)(tArgs...)>
    [[nodiscard]] static constexpr auto Create() noexcept { return delegate{ nullptr, function_stub<tMethod> }; }

    template <cValidLambda tLambda>
    requires (not std::is_same_v<tLambda, delegate<tReturn(tArgs...)>>)
    [[nodiscard]] static constexpr auto Create(tLambda& lambda) noexcept { return delegate{ (void*)(&lambda), lambda_stub<tLambda> }; }

    template <cValidLambda tLambda>
    requires (not std::is_same_v<tLambda, delegate<tReturn(tArgs...)>>)
    [[nodiscard]] static constexpr auto Create(tLambda const& lambda) noexcept { return delegate{ (void*)(&lambda), const_lambda_stub<tLambda> }; }

    template <typename T, tReturn(T::*tMethod)(tArgs...)> 
    [[nodiscard]] static constexpr delegate Create(T& instance) noexcept { return delegate{ (void*)(&instance), method_stub<T, tMethod> }; }

    template <typename T, tReturn(T::*tMethod)(tArgs...) const> 
    [[nodiscard]] static constexpr auto Create(T const& instance) noexcept { return delegate{ (void*)(&instance), const_method_stub<T, tMethod> }; }

    template <typename T, T& tInstance, tReturn(T::*tMethod)(tArgs...)> 
    [[nodiscard]] static constexpr auto Create() noexcept { return delegate{ instance_method_stub<T, tInstance, tMethod> }; }

    template <typename T, T& tInstance, tReturn(T::*tMethod)(tArgs...) const> 
    [[nodiscard]] static constexpr auto Create() noexcept { return delegate{ const_instance_method_stub<T, tInstance, tMethod> }; }

public:
    constexpr delegate() = default;
    constexpr delegate(delegate const&) = default;

    template <cValidLambda tLambda>
    requires (not std::is_same_v<tLambda, delegate<tReturn(tArgs...)>>)
    constexpr delegate(tLambda& lambda) noexcept { set_callable((void*)(&lambda), lambda_stub<tLambda>); }
    
    template <cValidLambda tLambda>
    requires (not std::is_same_v<tLambda, delegate<tReturn(tArgs...)>>)
    constexpr delegate(tLambda const& lambda) noexcept { set_callable((void*)(&lambda), const_lambda_stub<tLambda>); }

    delegate& operator =(delegate const&) = default;
    
    template <cValidLambda tLambda>
    requires (not std::is_same_v<tLambda, delegate<tReturn(tArgs...)>>)
    delegate& operator =(tLambda& lambda) noexcept { set_callable((void*)(&lambda), lambda_stub<tLambda>); return *this; }
    
    template <cValidLambda tLambda>
    requires (not std::is_same_v<tLambda, delegate<tReturn(tArgs...)>>)
    delegate& operator =(tLambda const& lambda) noexcept { set_callable((void*)(&lambda), const_lambda_stub<tLambda>); return *this; }

    template <tReturn(*tMethod)(tArgs...)> 
    constexpr void Set() noexcept { set_callable(nullptr, function_stub<tMethod>); }
    
    template <cValidLambda tLambda>
    requires (not std::is_same_v<tLambda, delegate<tReturn(tArgs...)>>)
    constexpr void Set(tLambda& lambda) noexcept { set_callable((void*)(&lambda), lambda_stub<tLambda>); }
    
    template <cValidLambda tLambda>
    requires (not std::is_same_v<tLambda, delegate<tReturn(tArgs...)>>)
    constexpr void Set(tLambda const& lambda) noexcept { set_callable((void*)(&lambda), const_lambda_stub<tLambda>); }
    
    template <typename T, tReturn(T::*tMethod)(tArgs...)>
    constexpr void Set(T& instance) noexcept { set_callable((void*)(&instance), method_stub<T, tMethod>); }
    
    template <typename T, tReturn(T::*tMethod)(tArgs...) const>
    constexpr void Set(T const& instance) noexcept { set_callable((void*)(&instance), const_method_stub<T, tMethod>); }
    
    template <typename T, T& tInstance, tReturn(T::*tMethod)(tArgs...)>
    constexpr void Set() noexcept { set_callable(nullptr, instance_method_stub<T, tInstance, tMethod>); }
    
    template <typename T, T const& tInstance, tReturn(T::*tMethod)(tArgs...) const>
    constexpr void Set() noexcept { set_callable(nullptr, const_instance_method_stub<T, tInstance, tMethod>); }

    constexpr void Clear() noexcept { mCallableObj.Clear(); }

    [[nodiscard]] constexpr bool IsValid() const noexcept { return mCallableObj; }
    [[nodiscard]] constexpr operator bool() const noexcept { return IsValid(); }

    tReturn operator()(tArgs... args) const noexcept
    {
        if (IsValid())
            return (*mCallableObj.Stub)(mCallableObj.Object, std::forward<tArgs>(args)...);

        return tReturn{};
    }
    bool CallIf(tArgs... args) const noexcept
    requires (std::is_void_v<tReturn>)
    {
        if (!IsValid())
            return false;

        (*mCallableObj.Stub)(mCallableObj.Object, std::forward<tArgs>(args)...); 
        return true;
    }
    std::optional<tReturn> CallIf(tArgs... args) const noexcept
    {
        std::optional<tReturn> result;

        if (mCallableObj)
            result = (*mCallableObj.Stub)(mCallableObj.Object, std::forward<tArgs>(args)...);

        return result;
    }
    template <typename tAlternate>
    tReturn CallOr(tAlternate alternate, tArgs... args) const noexcept
    {
        if (!mCallableObj)
            return alternate(std::forward<tArgs>(args)...);

        return (*mCallableObj.Stub)(mCallableObj.Object, std::forward<tArgs>(args)...); 
    }
    template <tReturn(*tAlternate)(tArgs...)> 
    tReturn CallOr(tArgs... args) const noexcept
    {
        if (!mCallableObj)
            return (tAlternate)(std::forward<tArgs>(args)...);

        return (*mCallableObj.Stub)(mCallableObj.Object, std::forward<tArgs>(args)...); 
    }

private:
    using stub_type = tReturn(*)(void* obj, tArgs...);

    struct callable {
        constexpr callable() = default;
        constexpr callable(void* obj, stub_type stub) noexcept
            :Object{ obj }, Stub{ stub }
        {}

        constexpr bool operator ==(callable const& rhs) const noexcept { return Object == rhs.Object and Stub == rhs.Stub; }
        constexpr bool operator !=(callable const& rhs) const noexcept { return Object != rhs.Object or Stub != rhs.Stub; }

        constexpr operator bool() const noexcept { return Stub != nullptr; }
        constexpr void Clear() noexcept { Object = nullptr; Stub = nullptr; }

        void* Object{ nullptr };
        stub_type Stub{ nullptr };
    };

    constexpr delegate(void* obj, stub_type stub) noexcept : mCallableObj{ obj, stub } {}
    constexpr delegate(stub_type stub) noexcept : mCallableObj{ nullptr, stub } {}

    constexpr void set_callable(void* obj, stub_type stub) noexcept { mCallableObj = { obj, stub }; }

    template <typename T, tReturn(T::*tMethod)(tArgs...)>
    static constexpr tReturn method_stub(void* obj, tArgs... args) noexcept
    {
        T* ptr = static_cast<T*>(obj);
        return (ptr->*tMethod)(std::forward<tArgs>(args)...);
    }
    template <typename T, tReturn(T::*tMethod)(tArgs...) const>
    static constexpr tReturn const_method_stub(void* obj, tArgs... args) noexcept
    {
        T* const ptr = static_cast<T*>(obj);
        return (ptr->*tMethod)(std::forward<tArgs>(args)...);
    }
    template <typename T, T& tInstance, tReturn(T::*tMethod)(tArgs...)>
    static constexpr tReturn instance_method_stub(void*, tArgs... args) noexcept
    {
        return (tInstance.*tMethod)(std::forward<tArgs>(args)...);
    }
    template <typename T, T& tInstance, tReturn(T::*tMethod)(tArgs...) const>
    static constexpr tReturn const_instance_method_stub(void*, tArgs... args) noexcept
    {
        return (tInstance.*tMethod)(std::forward<tArgs>(args)...);
    }
    template <tReturn(*tMethod)(tArgs...)>
    static constexpr tReturn function_stub(void*, tArgs... args) noexcept
    {
        return tMethod(std::forward<tArgs>(args)...);
    }
    template <cValidLambda tLambda>
    static constexpr tReturn lambda_stub(void* obj, tArgs... args) noexcept
    {
        tLambda* ptr = static_cast<tLambda*>(obj);
        return (ptr->operator())(std::forward<tArgs>(args)...);
    }
    template <cValidLambda tLambda>
    static constexpr tReturn const_lambda_stub(void* obj, tArgs... args) noexcept
    {
        const tLambda* ptr = static_cast<const tLambda*>(obj);
        return (ptr->operator())(std::forward<tArgs>(args)...);
    }

    callable mCallableObj;
};