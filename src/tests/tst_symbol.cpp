#include <catch.hpp>
#include <b0util/symbol/symbol.hpp>
#include <b0util/symbol/object.hpp>
#include <b0util/symbol/factory.hpp>

b0_define_symbol(name)
b0_define_symbol(value)
b0_define_symbol(toggle);
b0_define_symbol(unknown);

TEST_CASE("symbol") {
    using namespace s;

    struct {
        std::string name = "test";
        int value = 1234;
        auto toggle(bool in) const -> bool { return !in; }
    } obj;

    META_ASSERT(_name.exists(obj));
    META_ASSERT(_value.exists(obj));
    META_ASSERT(_toggle.exists(obj));

    REQUIRE(_name.name() == std::string("name"));
    REQUIRE(_value.name() == std::string("value"));
    REQUIRE(_toggle.name() == std::string("toggle"));

    REQUIRE(_name.equals(_name));
    REQUIRE(!_name.equals(_value));

    REQUIRE(_name.get(obj) == "test");
    REQUIRE(_value.get(obj) == 1234);
    REQUIRE(_toggle.invoke(obj, false) == true);
    META_ASSERT(!_unknown.exists<decltype(obj)>());
    REQUIRE(_unknown.get_or(obj, -1234) == -1234);

    struct { std::string name = "test2"; } obj2;
    REQUIRE(_name.try_compare(obj, obj2) == false);
    REQUIRE(_value.try_compare(obj, obj2) == false);
    REQUIRE(_name.try_assign(obj, obj2) == true);
    REQUIRE(_value.try_assign(obj, obj2) == false);
    REQUIRE(_name.try_compare(obj, obj2) == true);
    REQUIRE(_value.try_compare(obj, obj2) == false);
    REQUIRE(obj.name == "test2");

    SECTION("object") {
        auto object = b0::symbol::make_object(_name = std::string("object-name"), _value = 100, _toggle = true);
        REQUIRE(_name.get_or(object, "empty") == std::string("object-name"));
        REQUIRE(_unknown.get_or(object, std::string("empty")) == std::string("empty"));
        REQUIRE(object.value == 100);
        REQUIRE(object.name == "object-name");
        b0::symbol::assign(object, _toggle = false, 200, _name = std::string("another-name"));
        REQUIRE(object.value == 200);
        REQUIRE(object.name == "another-name");
        REQUIRE(object.toggle == false);
    }
    SECTION("factory") {
        auto factory = b0::symbol::make_factory(_value(_name = std::string("value")) = -1);
        auto o1 = factory(), o2 = factory();
        REQUIRE(o1.value == -1);
        REQUIRE(o2.value == -1);
        REQUIRE(o1 == o2);
        o2.value = 1;
        REQUIRE(o1 != o2);
        REQUIRE(factory(_value).option(_name, "empty") == "value");
    }
}
