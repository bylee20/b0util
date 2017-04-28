#pragma once

//#include "./rho.hpp"
//#include <QJsonValue>
//#include <QJsonArray>
//#include <QJsonObject>
//#include <QJsonDocument>

#include "./symbol.hpp"

B0_DECLARE_SYMBOL(json_serializer)
B0_DECLARE_SYMBOL(json_deserializer)
B0_DECLARE_SYMBOL(json_name)
B0_DECLARE_SYMBOL(json_skip)

//namespace b0 {
//class JsonIOError : public std::runtime_error {
//public:
//    JsonIOError(const QJsonParseError &e)
//        : std::runtime_error(QString(u"syntax error: "_q % e.errorString()).toStdString()) { }
//    JsonIOError(const std::string &message)
//        : std::runtime_error(message) { }
//    JsonIOError(const char *message)
//        : std::runtime_error(message) { }

//    static auto typeError(QJsonValue::Type expected) -> JsonIOError
//    {
//        using namespace std::string_literals;
//        auto type = [&] () {
//            switch (expected) {
//            case QJsonValue::Null:
//                return "null"s;
//            case QJsonValue::Double:
//                return "number"s;
//            case QJsonValue::Bool:
//                return "boolean"s;
//            case QJsonValue::String:
//                return "string"s;
//            case QJsonValue::Array:
//                return "array"s;
//            case QJsonValue::Object:
//                return "object"s;
//            default:
//                return "undefined"s;
//            }
//        }();
//        return "type mismatch: "s + type + " expected"s;
//    }
//};
//}

//#include "./private/jsonio_p.hpp"

//namespace b0 {

//template<class T>
//struct JsonIO {};

//template<class T>
//using JsonSerializer = std::function<QJsonValue(const T&)>;
//template<class T>
//using JsonDeserializer = std::function<T(const QJsonValue&)>;

//template<class T>
//B0_SI auto toJson(const T &t) -> QJsonValue
//{ return detail_json::json_io<T>::toJson(t); }

//template<class T>
//B0_SI auto toJsonDocument(const T &t) -> QJsonDocument
//{ return toJsonDocument(toJson(t)); }

//template<>
//auto toJsonDocument<QJsonValue>(const QJsonValue &json) -> QJsonDocument
//{
//    if (json.isArray())
//        return QJsonDocument(json.toArray());
//    if (json.isObject())
//        return QJsonDocument(json.toObject());
//    return {};
//}

//template<class T>
//B0_SI auto toJsonData(const T &t) -> QByteArray
//{ return toJsonDocument(t).toJson(); }

//template<class T>
//B0_SI auto parse(const QJsonValue &json) -> T
//{ return detail_json::json_io<T>::parse(json); }

//template<class T>
//B0_SI auto parse(const QJsonDocument &doc) -> T
//{
//    if (doc.isArray())
//        return parse<T>(doc.array());
//    if (doc.isObject())
//        return parse<T>(doc.object());
//    return t;
//}

//template<>
//auto parse<QJsonValue>(const QJsonDocument &doc) -> QJsonValue
//{
//    if (doc.isArray())
//        return doc.array();
//    if (doc.isObject())
//        return doc.object();
//    QJsonParseError e;
//    e.error = QJsonParseError::MissingObject;
//    throw JsonIOError(e);
//}

//template<class T>
//B0_SI auto parseJsonData(const QByteArray &data) -> T
//{
//    QJsonParseError e;
//    const auto &doc = QJsonDocument::fromJson(data, &e);
//    if (e.error)
//        throw JsonIOError(e);
//    return parse<T>(doc);
//}

//}
