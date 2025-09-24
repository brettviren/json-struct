#include "jsonstruct/field.hpp"
#include <fstream>

using namespace jsonstruct;

struct ServerConfig {
    std::string host = "localhost";
    int port = 8080;
#if 0
    std::optional<bool> debug_mode;
    std::variant<bool, std::string> feature_activation;
#endif
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
#if 0
            make_field("debug_mode", &ServerConfig::debug_mode),
            make_field("feature_activation", &ServerConfig::feature_activation, std::variant<bool, std::string>(true)),
#endif
            make_field("database", &ServerConfig::db_config),
            make_field("allowed_ips", &ServerConfig::allowed_ips, std::vector<std::string>{})
        );
    }
};


#if 0
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
#endif

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


int main (int argc, char* argv[])
{
    if (argc < 2) {

        nlohmann_default();

        return 0;
    }

//    auto a = jsoncpp_config(argv[1]);
    auto b = nlohmann_config(argv[1]);

    return 0;
}





