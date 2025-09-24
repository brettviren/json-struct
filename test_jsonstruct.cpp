struct ServerConfig {
    std::string host;
    int port = 8080;
    std::optional<bool> debug_mode;
    std::variant<bool, std::string> feature_activation;

    struct DatabaseConfig {
        std::string user;
        std::string password;
        int max_connections;

        static auto config_fields() {
            return std::make_tuple(
                make_field("user", &DatabaseConfig::user),
                make_field("password", &DatabaseConfig::password, std::string("")),
                make_field("max_connections", &DatabaseConfig::max_connections, 10)
            );
        }
    };
    DatabaseConfig db_config;

    std::vector<std::string> allowed_ips;

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
