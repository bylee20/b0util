#pragma once

#include "./any.hpp"
#include "./meta/type_seq.hpp"

namespace b0 {

template<class... Args>
class variant {
    B0_USING_REQ_T;
public:
    using type_seq = meta::type_seq<Args...>;
    static_assert(type_seq::exclusive, "each type for variant should be unique");
private:
    template<class T>
    using _ = b0::meta::_<T>;
    template<class U>
    static constexpr int type_index = meta::find_type_v<type_seq, std::decay_t<U>>;
    template<class U>
    static constexpr int constructible_index = meta::find_type_if_v<type_seq, _<U>::template can_construct>;
    template<class T>
    static constexpr int holdable_index = type_index<T> != -1 ? type_index<T> : constructible_index<T>;
public:
    variant() { ctor(type_seq::get<0>()); }

    variant(const variant &other) = default;

    variant(variant &&other) = default;

    template<class U, int I = holdable_index<U>, B0_REQ(I != -1)>
    variant(U &&other) { emplace<type_seq::get<I>>(std::forward<U>(other)); }

    template<class T, class... Args, B0_REQ(holdable_index<T> != -1)>
    variant(b0::in_place_type_t<T>, Args&&... args)
    {
        emplace<T>(std::forward<Args>(args)...);
    }

    auto index() const -> int { return m_index; }

    auto operator = (const variant &other) -> variant& = default;

    auto operator = (variant &&other) -> variant& = default;

    template<class U, int I = holdable_index<U>, B0_REQ(I != -1)>
    auto operator = (U &&rhs) -> variant&
    {
        emplace<type_seq::get<I>>(std::forward<U>(rhs));
        return *this;
    }

    template<class T, class... Args, B0_REQ(holdable_index<T> != -1)>
    auto emplace(Args&&... args) -> void
    {
        m_index = meta::find_type_v<type_seq, T>;
        m_data.emplace<T>(std::forward<Args>(args)...);
    }

    auto swap(variant &other) -> void
    {
        m_data.swap(other.m_data);
        std::swap(m_index, other.m_index);
    }

//    template<class T>
//    static constexpr auto can_hold() -> bool { return holdable_index<T> != -1; }

//    template<class T>
//    static constexpr auto can_hold(T&&) -> bool { return can_hold<std::decay_t<T>>(); }

    template<class T>
    auto is_holding() const -> bool { return type_index<T> == m_index; }

    auto type_id() const -> type_id_t { return m_data.type_id(); }

    auto type() const -> const std::type_info& { return m_data.type(); }

    template<class T>
    auto get() const -> const T& { return m_data.value<T>(); }

    template<class T>
    auto get() -> T& { return m_data.value<T>(); }

    template<int I>
    auto get() const -> const auto& { return get<type_seq::template get<I>>(); }

    template<int I>
    auto get() -> auto& { return get<type_seq::template get<I>>(); }

    template<class U, class T = std::decay_t<U>>
    auto get_or(U &&def) const& -> T
    {
        if (m_index == type_seq::find<T>)
            return get<T>();
        return std::forward<U>(def);
    }

    template<class U, class T = std::decay_t<U>>
    auto get_or(U &&def) && -> T
    {
        if (m_index == type_seq::template find<T>)
            return std::move(get<T>());
        return std::forward<U>(def);
    }

    template<class T>
    auto is() const -> bool { return m_data.is<T>(); }
private:
    int m_index = 0;
    b0::any m_data;
};

}
