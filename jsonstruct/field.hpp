#pragma once

#include <jsonstruct/converter.hpp>

namespace jsonstruct {

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
                return Converter<MemberType, JsonLibTraits>::fromJson(
                    JsonLibTraits::get_member(parent_json, name), obj.*ptr_to_member);
            } else {
                if (default_value) {
                    obj.*ptr_to_member = *default_value;
                    return true;
                }
                if (is_required) {
                    std::cerr << "Error: Required field '" << name << "' missing in JSON." << std::endl;
                    return false;
                }
                return true;
            }
        }

        template<typename JsonLibTraits>
        void serialize(const StructType& obj, typename JsonLibTraits::ValueType& parent_json) const {
            // Use the generic Converter with the specified JsonLibTraits
            JsonLibTraits::set_member(parent_json, name, Converter<MemberType, JsonLibTraits>::toJson(obj.*ptr_to_member));
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
}
