//#include <catch/catch.hpp>
//#include <b0util/symbol/object.hpp>

//#include <rapidjson/document.h>

//namespace b0 {

//namespace json {

//struct value {
//    value(double value): m_value(value) {}
//    value(bool value): m_value(value) {}
//    value(const char *)
//    value(const value&) = delete;
//    value(value &&other): m_value(std::move(other.m_value)) {}
//    auto operator = (const value&) = delete;
//    auto operator = (value &&rhs) -> value& { m_value = std::move(rhs.m_value); return *this; }
//private:
//    rapidjson::Value m_value;
//};

//struct document {
//    auto make(double n) const -> value
//    {

//    }
//private:
//    rapidjson::Document m_doc;
//};


//}

//}

//TEST_CASE("Json") {
//    parse<decltype(s_obj)>()
//}

////#include "jsonio.hpp"
////#include <QDebug>
////#include <complex>

////B0_DECLARE_SYMBOL(name)
////B0_DECLARE_SYMBOL(value)
////B0_DECLARE_SYMBOL(array)
////B0_DECLARE_SYMBOL(object)
////B0_DECLARE_SYMBOL(skipped)

////TEST_CASE("JSONIO") {
////    using namespace s;
////    B0_DECLARE_SDO_TYPE(
////        Item,
////        _name = QString("item"),
////        _value = true
////    );
////    B0_DECLARE_SDO_TYPE(
////        Document,
////        _name(_json_name = QString("jname")) = QString("document"),
////        _value(_json_serializer = [] (const std::complex<double> &v) {
////                return QJsonObject({{"real", v.real()}, {"imag", v.imag()}});
////            }, _json_deserializer = [] (const QJsonValue &json) {
////                const auto &o = json.toObject();
////                return std::complex<double>(o.value("real").toDouble(), o.value("imag").toDouble());
////            }) = std::complex<double>(1, 2),
////        _object = b0::define<class _object>(
////            _name = QString("object"),
////            _array = QVector<Item>(),
////            _skipped(_json_skip = true) = 123
////        )
////    );

////    auto doc = Document();
////    doc.object.skipped = 321;
////    doc.object.array.push_back(Item(u"item1"_q, false));
////    doc.object.array.push_back(Item(u"item2"_q, true));
////    static_assert(b0::is_sdo_type_v<Document>, "!!!");
////    const auto &json = b0::toJson(doc).toObject();
////    REQUIRE(json["jname"].toString() == doc.name);
////    REQUIRE(json["value"].toObject()["real"].toDouble() == doc.value.real());
////    REQUIRE(json["value"].toObject()["imag"].toDouble() == doc.value.imag());
////    REQUIRE(json["object"].toObject()["name"].toString() == doc.object.name);
////    REQUIRE(!json["object"].toObject().contains("skipped"));
////    REQUIRE(json["object"].toObject()["array"].toArray().size() == 2);
////    REQUIRE(json["object"].toObject()["array"].toArray()[0].toObject()["name"].toString() == QString("item1"));
////    REQUIRE(json["object"].toObject()["array"].toArray()[0].toObject()["value"].toBool() == false);
////    auto parsed = b0::parse<Document>(json);
////    REQUIRE(parsed.object.skipped == 123);
////    REQUIRE(parsed != doc);
////    parsed.object.skipped = 321;
////    REQUIRE(parsed == doc);
////}
