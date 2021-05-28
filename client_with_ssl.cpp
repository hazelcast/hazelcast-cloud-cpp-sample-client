#include <hazelcast/client/hazelcast_client.h>
#include <string>

int main(int argc, char **argv) {
    hazelcast::client::client_config config;
    
    config.set_cluster_name("YOUR_CLUSTER_NAME");
    config.set_property(hazelcast::client::client_properties::CLOUD_URL_BASE, "YOUR_DISCOVERY_URL");
    config.set_property("hazelcast.client.statistics.enabled", "true");

    auto &cloud_configuration = config.get_network_config().get_cloud_config();
    cloud_configuration.enabled = true;
    cloud_configuration.discovery_token = "YOUR_CLUSTER_DISCOVERY_TOKEN";

    boost::asio::ssl::context ctx(boost::asio::ssl::context::tlsv12);
    ctx.set_verify_mode(boost::asio::ssl::verify_peer);
    ctx.load_verify_file("ca.pem");
    ctx.use_certificate_file("cert.pem", boost::asio::ssl::context::pem);
    ctx.set_password_callback([&] (std::size_t max_length, boost::asio::ssl::context::password_purpose purpose) {
        return "YOUR_SSL_PASSWORD";
    });
    ctx.use_private_key_file("key.pem", boost::asio::ssl::context::pem);
    config.get_network_config().get_ssl_config().set_context(std::move(ctx));

    auto hazelcastClient = hazelcast::new_client(std::move(config)).get();
    auto map = hazelcastClient.get_map("map").get();
    auto check = map->put<std::string, std::string>("key", "value").get();
    map->clear().get();
    std::cout << "Connection Successful!" << std::endl;
    std::cout << "Now, 'map' will be filled with random entries." << std::endl;
    
    int iterationCount = 0;
    while (true) {
        int randomKey = rand();
        std::string randomKeyString = std::to_string(randomKey);
        try {
            map->put<std::string, std::string>("key" + randomKeyString, "value" + randomKeyString).get();
        } catch (std::exception &e) {
            std::cout << "Put operation failed error:" << e.what() << std::endl;
        }
        if (++iterationCount % 10 == 0){
            std::cout << "Map size:" + std::to_string(map->size().get()) << std::endl;
        }
    }

    return 0;
}
