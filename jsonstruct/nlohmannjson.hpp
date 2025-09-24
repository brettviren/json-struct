#pragma once

#include <nlohmann/json.hpp> 

#include <jsonstruct/traits.hpp>

namespace jsonstruct::nlohmannjson {

    struct Traits : jsonstruct::Traits<nlohmann::json> {
        using Base = jsonstruct::Traits<nlohmann::json>;
        using ValueType = typename Base::ValueType;

        // Type checking
        static bool is_object(const ValueType& v) { return v.is_object(); }
        static bool is_array(const ValueType& v) { return v.is_array(); }
        static bool is_string(const ValueType& v) { return v.is_string(); }
        static bool is_int(const ValueType& v) { return v.is_number_integer(); }
        static bool is_bool(const ValueType& v) { return v.is_boolean(); }
        static bool is_double(const ValueType& v) { return v.is_number_float(); } // Nlohmann distinguishes int/float numbers
        static bool is_null(const ValueType& v) { return v.is_null(); }

        // Value retrieval
        static int get_int(const ValueType& v) { return v.get<int>(); }
        static std::string get_string(const ValueType& v) { return v.get<std::string>(); }
        static bool get_bool(const ValueType& v) { return v.get<bool>(); }
        static double get_double(const ValueType& v) { return v.get<double>(); }

        // Value creation
        static ValueType create_object() { return ValueType::object(); }
        static ValueType create_array() { return ValueType::array(); }
        static ValueType create_null() { return ValueType(); } // Default constructed nlohmann::json is null
        static ValueType create_int(int val) { return ValueType(val); }
        static ValueType create_string(const std::string& val) { return ValueType(val); }
        static ValueType create_bool(bool val) { return ValueType(val); }
        static ValueType create_double(double val) { return ValueType(val); }

        // Object operations
        static bool has_member(const ValueType& obj, const char* name) { return obj.contains(name); }
        // nlohmann::json::at(key) returns a reference (json& or const json&).
        // Returning ValueType is safe and consistent with jsonstruct::Traits.
        static ValueType get_member(const ValueType& obj, const char* name) { return obj.at(name); }
        static void set_member(ValueType& obj, const char* name, const ValueType& val) { obj[name] = val; }

        // Array operations
        static void append_array_element(ValueType& arr, const ValueType& val) { arr.push_back(val); }

        // --- Nlohmann/json-specific Object Iteration ---
        template<typename Callback> // Callback signature: void(const std::string& key, ValueType& value)
        static void for_each_object_member(ValueType& obj, Callback&& cb) {
            if (!obj.is_object()) return;
            // nlohmann::json iterators provide key() and value() methods
            for (auto it = obj.begin(); it != obj.end(); ++it) {
                cb(it.key(), it.value());
            }
        }
    };

}
