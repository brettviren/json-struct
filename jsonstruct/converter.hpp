#pragma once

#include <string>
#include <vector>
#include <optional>   // C++17
#include <variant> // C++17

namespace jsonstruct {

    // Make sure this forward declaration has the new template parameter
    template<typename T, typename JsonLibTraits, typename Enable = void>
    struct Converter;

    // --- Basic Type Specializations (now templated on JsonLibTraits) ---
    template<typename JsonLibTraits>
    struct Converter<int, JsonLibTraits, void> {
        using JsonValueType = typename JsonLibTraits::ValueType;
        static bool fromJson(const JsonValueType& j_val, int& cpp_val) {
            if (JsonLibTraits::is_int(j_val)) { cpp_val = JsonLibTraits::get_int(j_val); return true; }
            return false;
        }
        static JsonValueType toJson(const int& cpp_val) { return JsonLibTraits::create_int(cpp_val); }
    };
    // Add specializations for std::string, bool, double, etc.
    template<typename JsonLibTraits>
    struct Converter<std::string, JsonLibTraits, void> {
        using JsonValueType = typename JsonLibTraits::ValueType;
        static bool fromJson(const JsonValueType& j_val, std::string& cpp_val) {
            if (JsonLibTraits::is_string(j_val)) { cpp_val = JsonLibTraits::get_string(j_val); return true; }
            return false;
        }
        static JsonValueType toJson(const std::string& cpp_val) { return JsonLibTraits::create_string(cpp_val); }
    };

    template<typename JsonLibTraits>
    struct Converter<bool, JsonLibTraits, void> {
        using JsonValueType = typename JsonLibTraits::ValueType;
        static bool fromJson(const JsonValueType& j_val, bool& cpp_val) {
            if (JsonLibTraits::is_bool(j_val)) { cpp_val = JsonLibTraits::get_bool(j_val); return true; }
            return false;
        }
        static JsonValueType toJson(const bool& cpp_val) { return JsonLibTraits::create_bool(cpp_val); }
    };


    template<typename JsonLibTraits>
    struct Converter<double, JsonLibTraits, void> {
        using JsonValueType = typename JsonLibTraits::ValueType;
        static bool fromJson(const JsonValueType& j_val, double& cpp_val) {
            // Nlohmann::json differentiates between integer and float numbers,
            // so checking for either might be appropriate depending on exact requirements.
            // JsonCPP's isDouble covers both for numbers.
            if (JsonLibTraits::is_double(j_val)) { cpp_val = JsonLibTraits::get_double(j_val); return true; }
            // Potentially handle integers as doubles if your policy allows:
            // if (JsonLibTraits::is_int(j_val)) { cpp_val = static_cast<double>(JsonLibTraits::get_int(j_val)); return true; }
            return false;
        }
        static JsonValueType toJson(const double& cpp_val) { return JsonLibTraits::create_double(cpp_val); }
    };


    // ... other primitive types ...

    template<typename T>
    struct has_config_fields {
    private:
        template <typename U>
        static auto test(U*) -> decltype(U::config_fields(), std::true_type{});
        static std::false_type test(...);
    public:
        static constexpr bool value = decltype(test((T*)nullptr))::value;
    };



    template<typename StructType, typename MemberType> // Removed JsonLibTraits from Field struct itself
    struct Field {
        const char* name;
        MemberType StructType::* ptr_to_member;
        std::optional<MemberType> default_value;
        bool is_required;

        Field(const char* n, MemberType StructType::* p)
            : name(n), ptr_to_member(p), is_required(true) {}

        Field(const char* n, MemberType StructType::* p, const MemberType& default_val)
            : name(n), ptr_to_member(p), default_value(default_val), is_required(false) {}

        // Template parse and serialize methods on JsonLibTraits
        template<typename JsonLibTraits>
        bool parse(StructType& obj, const typename JsonLibTraits::ValueType& parent_json) const {
            if (JsonLibTraits::has_member(parent_json, name)) {
                // Use the generic Converter with the specified JsonLibTraits
                return Converter<MemberType, JsonLibTraits, void>::fromJson(
                    JsonLibTraits::get_member(parent_json, name), obj.*ptr_to_member);
            } else {
                if (default_value) {
                    obj.*ptr_to_member = *default_value;
                    return true;
                }
                if (is_required) {
                    return false;
                }
                return true;
            }
        }

        template<typename JsonLibTraits>
        void serialize(const StructType& obj, typename JsonLibTraits::ValueType& parent_json) const {
            // Use the generic Converter with the specified JsonLibTraits
            JsonLibTraits::set_member(
                parent_json, name,
                Converter<MemberType, JsonLibTraits, void>::toJson(obj.*ptr_to_member));
        }
    };

    // Helper functions for cleaner Field creation (no change needed here)
    template<typename StructType, typename MemberType>
    Field<StructType, MemberType> make_field(const char* name, MemberType StructType::* ptr) {
        return Field<StructType, MemberType>(name, ptr);
    }

    template<typename StructType, typename MemberType>
    Field<StructType, MemberType> make_field(const char* name, MemberType StructType::* ptr, const MemberType& default_val) {
        return Field<StructType, MemberType>(name, ptr, default_val);
    }



    // --- Container Type Specializations (e.g., std::vector) ---
    template<typename T_elem, typename JsonLibTraits>
    struct Converter<std::vector<T_elem>, JsonLibTraits, void> {
        using JsonValueType = typename JsonLibTraits::ValueType;
        static bool fromJson(const JsonValueType& j_val, std::vector<T_elem>& cpp_val) {
            if (!JsonLibTraits::is_array(j_val)) return false;
            cpp_val.clear();
            bool success = true;
            // This loop works for JsonCpp and nlohmann::json if j_val is iterable
            for (const auto& item : j_val) {
                T_elem elem;
                if (!Converter<T_elem, JsonLibTraits, void>::fromJson(item, elem)) { success = false; break; }
                cpp_val.push_back(elem);
            }
            return success;
        }
        static JsonValueType toJson(const std::vector<T_elem>& cpp_val) {
            JsonValueType arr = JsonLibTraits::create_array();
            for (const auto& elem : cpp_val) {
                JsonLibTraits::append_array_element(arr, Converter<T_elem, JsonLibTraits, void>::toJson(elem));
            }
            return arr;
        }
    };

    // --- std::optional Specialization ---
    template<typename T_val, typename JsonLibTraits>
    struct Converter<std::optional<T_val>, JsonLibTraits, void> {
        using JsonValueType = typename JsonLibTraits::ValueType;
        static bool fromJson(const JsonValueType& j_val, std::optional<T_val>& cpp_val) {
            if (JsonLibTraits::is_null(j_val) || (JsonLibTraits::is_object(j_val) && j_val.empty()) || (JsonLibTraits::is_array(j_val) && j_val.empty())) {
                cpp_val.reset();
                return true;
            }
            T_val temp_val;
            if (Converter<T_val, JsonLibTraits, void>::fromJson(j_val, temp_val)) {
                cpp_val = std::move(temp_val);
                return true;
            }
            cpp_val.reset();
            return false;
        }
        static JsonValueType toJson(const std::optional<T_val>& cpp_val) {
            if (cpp_val) {
                return Converter<T_val, JsonLibTraits, void>::toJson(*cpp_val);
            }
            return JsonLibTraits::create_null();
        }
    };

    // --- std::variant Specialization ---
    template<typename... Types, typename JsonLibTraits>
    struct Converter<std::variant<Types...>, JsonLibTraits, void> {
        using JsonValueType = typename JsonLibTraits::ValueType;

        static bool fromJson(const JsonValueType& j_val, std::variant<Types...>& cpp_val) {
            bool success = false;

            auto try_parse_single_type = [&](auto dummy_instance) {
                using CurrentType = decltype(dummy_instance);

                if (!success) {
                    CurrentType temp_value;
                    if (Converter<CurrentType, JsonLibTraits, void>::fromJson(j_val, temp_value)) {
                        cpp_val = std::move(temp_value);
                        success = true;
                    }
                }
            };

            // FIX: Use an initializer list to correctly expand the pack
            int dummy_array[] = {
                (try_parse_single_type(Types{}), 0)... // The '...' applies to the whole (expression, 0) part
            };
            (void)dummy_array; // Suppress unused variable warning

            return success;
        }

        static JsonValueType toJson(const std::variant<Types...>& cpp_val) {
            return std::visit([&](auto&& arg) {
                using T_current = std::decay_t<decltype(arg)>;
                // Pass JsonLibTraits in the recursive call
                return Converter<T_current, JsonLibTraits, void>::toJson(std::forward<decltype(arg)>(arg));
            }, cpp_val);
        }
    };



    // --- Generic Converter for Custom Structs (SFINAE) ---
    template<typename T, typename JsonLibTraits>
    struct Converter<T, JsonLibTraits, std::enable_if_t<has_config_fields<T>::value>> {
        using JsonValueType = typename JsonLibTraits::ValueType;
        static bool fromJson(const JsonValueType& json_config, T& out_obj) {
            if (!JsonLibTraits::is_object(json_config)) return false;
            bool success = true;
            std::apply([&](const auto&... field_descriptor){
                ((success &= field_descriptor.template parse<JsonLibTraits>(out_obj, json_config)), ...); // Pass traits
            }, T::config_fields());
            return success;
        }

        static JsonValueType toJson(const T& obj) {
            JsonValueType json_val = JsonLibTraits::create_object();
            std::apply([&](const auto&... field_descriptor){
                (field_descriptor.template serialize<JsonLibTraits>(obj, json_val), ...); 
            }, T::config_fields());
            return json_val;
        }
    };

}
