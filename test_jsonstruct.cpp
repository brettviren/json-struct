#include "jsonstruct/converter.hpp"
#include <fstream>
#include <iostream> 

using namespace jsonstruct;

struct ServerConfig {
    std::string host = "localhost";
    int port = 8080;
    std::optional<bool> debug_mode;
    std::variant<bool, std::string> feature_activation;
    struct DatabaseConfig {
        std::string user = "user";
        std::string password = "changme";
        int max_connections = 42;

        static auto config_fields() {
            return std::make_tuple(
                make_field("user", &DatabaseConfig::user),
                make_field("password", &DatabaseConfig::password, std::string("")),
                make_field("max_connections", &DatabaseConfig::max_connections, 10)
            );
        }
    };
    DatabaseConfig db_config;

    std::vector<std::string> allowed_ips = { "127.0.0.1" };

    static auto config_fields() {
        return std::make_tuple(
            make_field("host", &ServerConfig::host, std::string("localhost")),
            make_field("port", &ServerConfig::port, 8080),
            make_field("debug_mode", &ServerConfig::debug_mode),
            make_field("feature_activation", &ServerConfig::feature_activation, std::variant<bool, std::string>(true)),
            make_field("database", &ServerConfig::db_config),
            make_field("allowed_ips", &ServerConfig::allowed_ips, std::vector<std::string>{})
        );
    }
};


#include <jsonstruct/jsoncpp.hpp>

ServerConfig jsoncpp_config(const std::string& filename)
{
    std::ifstream fp(filename);
    Json::Value input;
    fp >> input;

    ServerConfig config;
    if (Converter<ServerConfig, jsoncpp::Traits>::fromJson(input, config)) {
        std::cout << "JsonCPP config loaded successfully." << std::endl;
        // ... use config_cpp ...
    } else {
        std::cerr << "Failed to configure ServerConfig with JsonCPP." << std::endl;
    }

    Json::Value output = Converter<ServerConfig, jsoncpp::Traits>::toJson(config);

    return config;
}

void jsoncpp_default()
{
    ServerConfig config;
    auto output = Converter<ServerConfig, jsoncpp::Traits>::toJson(config);
    std::cout << output << "\n";
}

#include <jsonstruct/nlohmannjson.hpp>

ServerConfig nlohmann_config(const std::string& filename)
{
    std::ifstream ifs(filename);
    auto input = nlohmann::json::parse(ifs);

    ServerConfig config;
    if (Converter<ServerConfig, nlohmannjson::Traits>::fromJson(input, config)) {
        std::cout << "Nlohmann/json config loaded successfully." << std::endl;
        // ... use config ...
    } else {
        std::cerr << "Failed to configure ServerConfig with Nlohmann/json." << std::endl;
    }

    nlohmann::json output = Converter<ServerConfig, nlohmannjson::Traits>::toJson(config);
    return config;
}

void nlohmann_default()
{
    ServerConfig config;
    auto output = Converter<ServerConfig, nlohmannjson::Traits>::toJson(config);
    std::cout << output << "\n";
}


void demo_iteration() {
    // JsonCPP example
    Json::Value jsoncpp_obj;
    jsoncpp_obj["name"] = "Alice";
    jsoncpp_obj["age"] = 30;
    jsoncpp_obj["city"] = "New York";

    std::cout << "--- JsonCPP Object Iteration ---" << std::endl;
    jsoncpp::Traits::for_each_object_member(jsoncpp_obj, [](const std::string& key, Json::Value& value) {
        std::cout << "Key: " << key << ", Value: " << value.toStyledString();
    });

    // nlohmann/json example
    nlohmann::json nlohmann_obj;
    nlohmann_obj["name"] = "Bob";
    nlohmann_obj["age"] = 25;
    nlohmann_obj["country"] = "Canada";

    std::cout << "\n--- Nlohmann/json Object Iteration ---" << std::endl;
    nlohmannjson::Traits::for_each_object_member(nlohmann_obj, [](const std::string& key, nlohmann::json& value) {
        std::cout << "Key: " << key << ", Value: " << value.dump() << std::endl;
    });

    // Array iteration (direct range-based for loop, which works for both)
    Json::Value jsoncpp_arr;
    jsoncpp_arr.append(10);
    jsoncpp_arr.append("hello");
    std::cout << "\n--- JsonCPP Array Iteration (direct) ---" << std::endl;
    for (const auto& item : jsoncpp_arr) {
        std::cout << "Array Item: " << item.toStyledString();
    }
}

int main (int argc, char* argv[])
{
    if (argc < 2) {

        nlohmann_default();
        jsoncpp_default();
        demo_iteration();

        return 0;
    }

    auto a = jsoncpp_config(argv[1]);
    auto b = nlohmann_config(argv[1]);

    return 0;
}





