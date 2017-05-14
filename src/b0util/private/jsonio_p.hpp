#pragma once

#include "../tmp.hpp"
#include "../symbol.hpp"

namespace b0 {

template<class T>
struct JsonIO;

namespace detail_json {

B0_DECLARE_HAS_MEMBER(toJson)
B0_DECLARE_HAS_MEMBER(parse)

template<class T>
B0_SC bool to_json_exists_v = has_toJson<JsonIO<T>>::value;
template<class T>
B0_SC bool parse_exists_v = has_parse<JsonIO<T>>::value;

template<class T>
struct jtag {};

template<class T>
using jtag_t =
    b0::if_t<parse_exists_v<T> && to_json_exists_v<T>, jtag<class jcustom>,
    b0::if_t<std::is_same_v<T, bool>, jtag<class jvalue>,
    b0::if_t<std::is_same_v<T, QString>, jtag<class jvalue>,
    b0::if_t<std::is_enum_v<T> , jtag<class jenum>,
    b0::if_t<std::is_integral_v<T>, jtag<class jinteger>,
    b0::if_t<std::is_floating_point_v<T>, jtag<class jnumber>,
    b0::if_t<b0::is_iterable<T>::value, jtag<class jarray>,
    b0::if_t<b0::is_sdo_type_v<T>, jtag<class jobject>,
void>>>>>>>>;

template<class T, class Tag = jtag_t<T>>
struct json_io;

template<class T>
struct json_io<T, jtag<class jvalue>>;

template<>
struct json_io<bool, jtag<class jvalue>> {
    B0_SI auto toJson(bool t) -> QJsonValue { return t; }
    B0_SI auto parse(const QJsonValue &json) -> bool
    {
        if (!json.isBool())
            throw JsonIOError::typeError(QJsonValue::Bool);
        return json.toBool();
    }
};

template<>
struct json_io<QString, jtag<class jvalue>> {
    B0_SI auto toJson(const QString &t) -> QJsonValue { return t; }
    B0_SI auto parse(const QJsonValue &json) -> QString
    {
        if (!json.isString())
            throw JsonIOError::typeError(QJsonValue::String);
        return json.toString();
    }
};

template<class T>
struct json_io<T, jtag<class jenum>> {
    using integral_t = std::underlying_type_t<T>;
    B0_SI auto toJson(T t) -> QJsonValue
    {
        return json_io<integral_t>::toJson(static_cast<integral_t>(t));
    }
    B0_SI auto parse(const QJsonValue &json) -> T
    {
        return static_cast<T>(json_io<integral_t>::parse(json));
    }
};

template<class T>
struct json_io<T, jtag<class jinteger>> {
    B0_SI auto toJson(T t) -> QJsonValue { return static_cast<double>(t); }
    B0_SI auto parse(const QJsonValue &json) -> T
    {
        auto v = json_io<double>::parse(json);
        return static_cast<T>(v);
    }
};

template<class T>
struct json_io<T, jtag<class jnumber>> {
    B0_SI auto toJson(T t) -> QJsonValue { return static_cast<double>(t); }
    B0_SI auto parse(const QJsonValue &json) -> T
    {
        if (!json.isDouble())
            throw JsonIOError::typeError(QJsonValue::Double);
        return static_cast<T>(json.toDouble());
    }
};

template<class T>
struct json_io<T, jtag<class jcustom>> {
    B0_SI auto toJson(const T &t) -> QJsonValue { return JsonIO<T>::toJson(t); }
    B0_SI auto parse(const QJsonValue &json) -> T { return JsonIO<T>::parse(json); }
};

template<class T>
struct json_io<T, jtag<class jarray>> {
    B0_SI auto toJson(const T &t) -> QJsonValue
    {
        QJsonArray array;
        const auto end = std::end(t);
        for (auto it = std::begin(t); it != end; ++it)
            array.push_back(b0::toJson<std::decay_t<decltype(*it)>>(*it));
        return array;
    }
    B0_SI auto parse(const QJsonValue &json) -> T
    {
        if (!json.isArray())
            throw JsonIOError::typeError(QJsonValue::Array);
        return b0::mapAs<T>(json.toArray(), [&] (const QJsonValue &value)
            { return ::b0::parse<std::decay_t<decltype(*std::begin(std::declval<T>()))>>(value); });
    }
};


template<bool custom = true>
struct json_io_object {
    template<class T>
    B0_SI auto toJson(const T &var) -> QJsonValue
    { return var.f_option<s::_json_serializer_t>()(var.f_value()); }
    template<class T>
    B0_SI auto parse(const QJsonValue &json, T &var) -> void
    { var.f_value() = var.f_option<s::_json_deserializer_t>()(json); }
};

template<>
struct json_io_object<false> {
    template<class T>
    B0_SI auto toJson(const T &var) -> QJsonValue
    { return b0::toJson<std::decay_t<decltype(var.f_value())>>(var.f_value()); }
    template<class T>
    B0_SI auto parse(const QJsonValue &json, T &var) -> void
    { var.f_value() = ::b0::parse<std::decay_t<decltype(var.f_value())>>(json); }
};

template<class T>
struct json_io<T, jtag<class jobject>> {
    B0_SI auto toJson(const T &t) -> QJsonValue
    {
        QJsonObject object;
        using namespace s;
        b0::for_each(t, [&] (const auto &var) {
            if (var.f_option(_json_skip, false))
                return;
            auto &&json = json_io_object<var.f_hasOption(_json_serializer)>::toJson(var);
            object.insert(var.f_option(_json_name, QString(var.f_name())), std::move(json));
        });
        return object;
        return {};
    }
    B0_SI auto parse(const QJsonValue &json) -> T
    {
        if (!json.isObject())
            throw JsonIOError::typeError(QJsonValue::Object);
        const auto &object = json.toObject();
        using namespace s;
        T t;
        b0::for_each(t, [&] (auto &&var) {
            if (var.f_option(_json_skip, false))
                return;
            auto it = object.find(var.f_option(_json_name, QString(var.f_name())));
            if (it != object.end()) {
                try {
                    json_io_object<var.f_hasOption(_json_deserializer)>::parse(*it, var);
                } catch (JsonIOError &e) {
                    throw JsonIOError(QString(it.key() % ": " % e.what()).toStdString());
                }
            }
        });
        return t;
    }
};




//template<class T>
//B0_SI auto toJson(const T &t, jtag<class jvalue>) -> QJsonValue { return t; }

//template<class T>
//B0_SI auto toJson(const T &t, jtag<double>) -> QJsonValue { return static_cast<double>(t); }

//template<class T>
//B0_SI auto toJson(const T &t, jtag<class jcustom>) -> QJsonValue { return JsonIO<T>::toJson(t); }

//template<class T>
//B0_SI auto toJson(const T &t, jtag<class jarray>) -> QJsonValue
//{
//    QJsonArray array;
//    const auto end = std::end(t);
//    for (auto it = std::begin(t); it != end; ++it)
//        array.push_back(b0::toJson<std::decay_t<decltype(*it)>>(*it));
//    return array;
//}

//template<bool custom = true>
//struct to_json_s {
//    template<class T>
//    B0_SI auto apply(const T &var) -> QJsonValue
//    { return var.f_option<s::_json_serializer_t>()(var.f_value()); }
//};

//template<>
//struct to_json_s<false> {
//    template<class T>
//    B0_SI auto apply(const T &var) -> QJsonValue
//    { return b0::toJson<std::decay_t<decltype(var.f_value())>>(var.f_value()); }
//};


//template<class T>
//B0_SI auto toJson(const T &t, jtag<class jobject>) -> QJsonValue
//{
//    QJsonObject object;
//    using namespace s;
//    b0::for_each(t, [&] (const auto &var) {
//        if (var.f_option(_json_skip, false))
//            return;
//        auto &&json = to_json_s<var.f_hasOption(_json_serializer)>::apply(var);
//        object.insert(var.f_option(_json_name, QString(var.f_name())), std::move(json));
//    });
//    return object;
//    return {};
//}

//template<class T>
//B0_SCI auto parse(const QJsonValue &json, jtag<double>) -> T
//{
//    if (!json.isDouble())
//        throw JsonIOError::typeError(QJsonValue::Double);
//    return static_cast<T>(static_cast<b0::if_t<std::is_enum_v<T>, int, double>>(json.toDouble()));
//}

//template<class T>
//B0_SI auto parse(const QJsonValue &json, jtag<class jvalue>) -> T;

//template<>
//B0_SI auto parse<QString>(const QJsonValue &json, jtag<class jvalue>) -> QString
//{
//    if (!json.isString())
//        throw JsonIOError::typeError(QJsonValue::String);
//    return json.toString();
//}


//template<>
//B0_SI auto parse<bool>(const QJsonValue &json, jtag<class jvalue>) -> bool
//{
//    if (!json.isBool())
//        throw JsonIOError::typeError(QJsonValue::Bool);
//    return json.toBool();
//}

//template<class T>
//B0_SI auto parse(const QJsonValue &json, jtag<class jcustom>) -> T { return JsonIO<T>::parse(json); }


//template<class T>
//B0_SI auto parse(const QJsonValue &json, jtag<class jarray>) -> T
//{
//    if (!json.isArray())
//        throw JsonIOError::typeError(QJsonValue::Array);
//    return b0::mapAs<T>(json.toArray(), [&] (const QJsonValue &value)
//        { return ::b0::parse<std::decay_t<decltype(*std::begin(std::declval<T>()))>>(value); });
//}

//template<bool custom>
//struct parse_s {
//    template<class T>
//    B0_SI auto apply(const QJsonValue &json, T &var) -> void
//    { var.f_value() = var.f_option<s::_json_deserializer_t>()(json); }
//};

//template<>
//struct parse_s<false> {
//    template<class T>
//    B0_SI auto apply(const QJsonValue &json, T &var) -> void
//    { var.f_value() = ::b0::parse<std::decay_t<decltype(var.f_value())>>(json); }
//};

//template<class T>
//B0_SI auto parse(const QJsonValue &json, jtag<class jobject>) -> T
//{
//    if (!json.isObject())
//        throw JsonIOError::typeError(QJsonValue::Object);
//    const auto &object = json.toObject();
//    using namespace s;
//    T t;
//    b0::for_each(t, [&] (auto &&var) {
//        if (var.f_option(_json_skip, false))
//            return;
//        auto it = object.find(var.f_option(_json_name, QString(var.f_name())));
//        if (it != object.end()) {
//            try {
//                parse_s<var.f_hasOption(_json_deserializer)>::apply(*it, var);
//            } catch (JsonIOError &e) {
//                throw JsonIOError(QString(it.key() % ": " % e.what()).toStdString());
//            }
//        }
//    });
//    return t;
//}

}
}
