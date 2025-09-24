#pragma once

namespace jsonstruct {

    // Generic JSON traits relied on for conversin 
    template<typename ValueType_>
    struct Traits {
        using ValueType = ValueType_;

        // Type checking
        static bool is_object(const ValueType& v) { /* ... */ return false; }
        static bool is_array(const ValueType& v) { /* ... */ return false; }
        static bool is_string(const ValueType& v) { /* ... */ return false; }
        static bool is_int(const ValueType& v) { /* ... */ return false; }
        static bool is_bool(const ValueType& v) { /* ... */ return false; }
        static bool is_double(const ValueType& v) { /* ... */ return false; }
        static bool is_null(const ValueType& v) { /* ... */ return false; }

        // Value retrieval
        static int get_int(const ValueType& v) { /* ... */ return 0; }
        static std::string get_string(const ValueType& v) { /* ... */ return {}; }
        static bool get_bool(const ValueType& v) { /* ... */ return false; }
        static double get_double(const ValueType& v) { /* ... */ return 0.0; }

        // Value creation
        static ValueType create_object() { /* ... */ return {}; }
        static ValueType create_array() { /* ... */ return {}; }
        static ValueType create_null() { /* ... */ return {}; }
        static ValueType create_int(int val) { /* ... */ return {}; }
        static ValueType create_string(const std::string& val) { /* ... */ return {}; }
        static ValueType create_bool(bool val) { /* ... */ return {}; }
        static ValueType create_double(double val) { /* ... */ return {}; }


        // Object operations
        static bool has_member(const ValueType& obj, const char* name) { /* ... */ return false; }
        static ValueType get_member(const ValueType& obj, const char* name) { /* ... */ return {}; }
        static void set_member(ValueType& obj, const char* name, const ValueType& val) { /* ... */ }

        // Array operations
        static void append_array_element(ValueType& arr, const ValueType& val) { /* ... */ }

        // Iteration (can be tricky to generalize perfectly, but common patterns exist)
        // For arrays: usually range-based for works if ValueType is iterable
        // For objects: range-based for works for nlohmann::json, JsonCpp has specific iterators

        // --- Unified Iteration for Objects (Callback-based) ---
        // For arrays, direct range-based for loop over ValueType (e.g., `for (const auto& item : json_array_value)`)
        // is expected to work in JsonConverter<std::vector> for both libraries.
        // For objects, this callback-based approach provides a consistent interface.
        template<typename Callback> // Callback signature: void(const std::string& key, ValueType& value)
        static void for_each_object_member(ValueType& obj, Callback&& cb) {
            // Default empty implementation, concrete traits will override
        }

    };
  
}
