#include <string>
#include "model/City.h"
#include "model/Country.h"

// This is boilerplate application that configures client to connect Hazelcast
// Cloud cluster.
// See: https://docs.hazelcast.com/cloud/cpp-client

void mapExample(hazelcast::client::hazelcast_client client);
void nonStopMapExample(hazelcast::client::hazelcast_client client);
int main(int argc, char **argv) {
    hazelcast::client::client_config config;
    config.set_property("hazelcast.client.statistics.enabled", "true");
    config.set_property(hazelcast::client::client_properties::CLOUD_URL_BASE, "YOUR_DISCOVERY_URL");
    config.set_cluster_name("YOUR_CLUSTER_NAME");
    auto &cloud_configuration = config.get_network_config().get_cloud_config();
    cloud_configuration.enabled = true;
    cloud_configuration.discovery_token = "YOUR_CLUSTER_DISCOVERY_TOKEN";
    auto client = hazelcast::new_client(std::move(config)).get();
    std::cout << "Connection Successful!" << std::endl;

    mapExample(client);

    //nonStopMapExample(client);

    client.shutdown();
}

// This example shows how to work with Hazelcast maps.

void mapExample(hazelcast::client::hazelcast_client client) {
    auto cities = client.get_map("cities").get();
    cities->put<std::string, hazelcast::client::hazelcast_json_value>("1", City::asJson("United Kingdom", "London", 9540576)).get();
    cities->put<std::string, hazelcast::client::hazelcast_json_value>("2", City::asJson("United Kingdom", "Manchester", 2770434)).get();
    cities->put<std::string, hazelcast::client::hazelcast_json_value>("3", City::asJson("United States", "New York", 19223191)).get();
    cities->put<std::string, hazelcast::client::hazelcast_json_value>("4", City::asJson("United States", "Los Angeles", 3985520)).get();
    cities->put<std::string, hazelcast::client::hazelcast_json_value>("5", City::asJson("Turkey", "Ankara", 5309690)).get();
    cities->put<std::string, hazelcast::client::hazelcast_json_value>("6", City::asJson("Turkey", "Istanbul", 15636243)).get();
    cities->put<std::string, hazelcast::client::hazelcast_json_value>("7", City::asJson("Brazil", "Sao Paulo", 22429800)).get();
    cities->put<std::string, hazelcast::client::hazelcast_json_value>("8", City::asJson("Brazil", "Rio de Janeiro", 13634274)).get();
    int mapSize = cities->size().get();
    std::printf("'cities' map now contains %d entries.\n", mapSize);
    std::cout << "--------------------" << std::endl;
}

// This example shows how to work with Hazelcast maps, where the map is
// updated continuously.

void nonStopMapExample(hazelcast::client::hazelcast_client client) {
    std::cout << "Now the map named 'map' will be filled with random entries." << std::endl;
    std::cout << "--------------------" << std::endl;
    auto map = client.get_map("map").get();
    int iterationCounter = 0;
    while (true) {
        int randomKey = std::rand() % 100000;
        map->put<std::string, std::string>("key-" + std::to_string(randomKey), "value-" + std::to_string(randomKey)).get();
        map->get<std::string, std::string>("key-" + std::to_string(std::rand() % 100000)).get();
        if (++iterationCounter == 10) {
            iterationCounter = 0;
            std::cout << "Current map size: " + std::to_string(map->size().get()) << std::endl;
        }
    }
}




