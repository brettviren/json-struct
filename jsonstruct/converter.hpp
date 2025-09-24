#pragam once

namespace jsonstruct {

    // Make sure this forward declaration has the new template parameter
    template<typename T, typename JsonLibTraits>
    struct Converter;

    // --- Basic Type Specializations (now templated on JsonLibTraits) ---
    template<typename JsonLibTraits>
    struct Converter<int, JsonLibTraits> {
        using JsonValueType = typename JsonLibTraits::ValueType;
        static bool fromJson(const JsonValueType& j_val, int& cpp_val) {
            if (JsonLibTraits::is_int(j_val)) { cpp_val = JsonLibTraits::get_int(j_val); return true; }
            return false;
        }
        static JsonValueType toJson(const int& cpp_val) { return JsonLibTraits::create_int(cpp_val); }
    };
    // Add specializations for std::string, bool, double, etc.
    template<typename JsonLibTraits>
    struct Converter<std::string, JsonLibTraits> {
        using JsonValueType = typename JsonLibTraits::ValueType;
        static bool fromJson(const JsonValueType& j_val, std::string& cpp_val) {
            if (JsonLibTraits::is_string(j_val)) { cpp_val = JsonLibTraits::get_string(j_val); return true; }
            return false;
        }
        static JsonValueType toJson(const std::string& cpp_val) { return JsonLibTraits::create_string(cpp_val); }
    };
    // ... other primitive types ...

    // --- Container Type Specializations (e.g., std::vector) ---
    template<typename T_elem, typename JsonLibTraits>
    struct Converter<std::vector<T_elem>, JsonLibTraits> {
        using JsonValueType = typename JsonLibTraits::ValueType;
        static bool fromJson(const JsonValueType& j_val, std::vector<T_elem>& cpp_val) {
            if (!JsonLibTraits::is_array(j_val)) return false;
            cpp_val.clear();
            bool success = true;
            // This loop works for JsonCpp and nlohmann::json if j_val is iterable
            for (const auto& item : j_val) {
                T_elem elem;
                if (!Converter<T_elem, JsonLibTraits>::fromJson(item, elem)) { success = false; break; }
                cpp_val.push_back(elem);
            }
            return success;
        }
        static JsonValueType toJson(const std::vector<T_elem>& cpp_val) {
            JsonValueType arr = JsonLibTraits::create_array();
            for (const auto& elem : cpp_val) {
                JsonLibTraits::append_array_element(arr, Converter<T_elem, JsonLibTraits>::toJson(elem));
            }
            return arr;
        }
    };

    // --- std::optional Specialization ---
    template<typename T_val, typename JsonLibTraits>
    struct Converter<std::optional<T_val>, JsonLibTraits> {
        using JsonValueType = typename JsonLibTraits::ValueType;
        static bool fromJson(const JsonValueType& j_val, std::optional<T_val>& cpp_val) {
            if (JsonLibTraits::is_null(j_val) || (JsonLibTraits::is_object(j_val) && j_val.empty()) || (JsonLibTraits::is_array(j_val) && j_val.empty())) {
                cpp_val.reset();
                return true;
            }
            T_val temp_val;
            if (Converter<T_val, JsonLibTraits>::fromJson(j_val, temp_val)) {
                cpp_val = std::move(temp_val);
                return true;
            }
            cpp_val.reset();
            return false;
        }
        static JsonValueType toJson(const std::optional<T_val>& cpp_val) {
            if (cpp_val) {
                return Converter<T_val, JsonLibTraits>::toJson(*cpp_val);
            }
            return JsonLibTraits::create_null();
        }
    };

    // --- std::variant Specialization ---
    template<typename... Types, typename JsonLibTraits>
    struct Converter<std::variant<Types...>, JsonLibTraits> {
        using JsonValueType = typename JsonLibTraits::ValueType;
        static bool fromJson(const JsonValueType& j_val, std::variant<Types...>& cpp_val) {
            bool success = false;
            ([&](auto type_tag){
                using CurrentType = decltype(type_tag);
                if (!success) {
                    CurrentType temp_value;
                    if (Converter<CurrentType, JsonLibTraits>::fromJson(j_val, temp_value)) {
                        cpp_val = std::move(temp_value);
                        success = true;
                    }
                }
            }((Types)(), ...));
            return success;
        }
        static JsonValueType toJson(const std::variant<Types...>& cpp_val) {
            return std::visit([](auto&& arg) {
                using T_current = std::decay_t<decltype(arg)>;
                return Converter<T_current, JsonLibTraits>::toJson(std::forward<decltype(arg)>(arg));
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
                field_descriptor.template serialize<JsonLibTraits>(obj, json_val); // Pass traits
            }, T::config_fields());
            return json_val;
        }
    };

}
