#pragma once

#include <json/json.h>

#include <jsonstruct/traits.hpp>

namespace jsonstruct::jsoncpp {

    /// JsonCPP traits
    struct Traits : jsonstruct::Traits<Json::Value> {
        using ValueType = Json::Value;

        // Type checking
        static bool is_object(const ValueType& v) { return v.isObject(); }
        static bool is_array(const ValueType& v) { return v.isArray(); }
        static bool is_string(const ValueType& v) { return v.isString(); }
        static bool is_int(const ValueType& v) { return v.isInt(); }
        static bool is_bool(const ValueType& v) { return v.isBool(); }
        static bool is_double(const ValueType& v) { return v.isDouble(); }
        static bool is_null(const ValueType& v) { return v.isNull(); }

        // Value retrieval
        static int get_int(const ValueType& v) { return v.asInt(); }
        static std::string get_string(const ValueType& v) { return v.asString(); }
        static bool get_bool(const ValueType& v) { return v.asBool(); }
        static double get_double(const ValueType& v) { return v.asDouble(); }

        // Value creation
        static ValueType create_object() { return Json::Value(Json::objectValue); }
        static ValueType create_array() { return Json::Value(Json::arrayValue); }
        static ValueType create_null() { return Json::Value(); } // JsonCpp default constructor makes null
        static ValueType create_int(int val) { return Json::Value(val); }
        static ValueType create_string(const std::string& val) { return Json::Value(val); }
        static ValueType create_bool(bool val) { return Json::Value(val); }
        static ValueType create_double(double val) { return Json::Value(val); }

        // Object operations
        static bool has_member(const ValueType& obj, const char* name) { return obj.isMember(name); }
        static ValueType get_member(const ValueType& obj, const char* name) { return obj[name]; }
        static void set_member(ValueType& obj, const char* name, const ValueType& val) { obj[name] = val; }

        // Array operations
        static void append_array_element(ValueType& arr, const ValueType& val) { arr.append(val); }

        // JsonCpp array iteration is typically: for (const auto& item : arr) { ... }
        // JsonCpp object iteration: for (auto it = obj.begin(); it != obj.end(); ++it) { /* it.key(), *it */ }
        // For the purpose of =JsonConverter= array handling, range-based for loop over =Json::Value= works directly.

        // --- JsonCpp-specific Object Iteration ---
        template<typename Callback> // Callback signature: void(const std::string& key, ValueType& value)
        static void for_each_object_member(ValueType& obj, Callback&& cb) {
            if (!obj.isObject()) return;
            // JsonCpp's iterators require manual key access and dereference to value
            for (auto key : obj.getMemberNames()) {
                cb(key, obj[key]);
            }
        }
    };
}
