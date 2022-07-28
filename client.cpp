#include <string>
#include <hazelcast/client/hazelcast_client.h>

/**
 * This is boilerplate application that configures client to connect Hazelcast
 * Cloud cluster.
 * <p>
 * See: <a href="https://docs.hazelcast.com/cloud/cpp-client">https://docs.hazelcast.com/cloud/cpp-client</a>
 */

void map_example(hazelcast::client::hazelcast_client client);
void non_stop_map_example(hazelcast::client::hazelcast_client client);
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

    map_example(client);

    //non_stop_map_example(client);

    client.shutdown().wait();
}

hazelcast::client::hazelcast_json_value as_json(std::string country, std::string city, int population) {
    return hazelcast::client::hazelcast_json_value("{\"country\":\"" + country + "\",\"city\":\"" + city + "\",\"population\":" +std::to_string(population)  +"}");
}

/**
* This example shows how to work with Hazelcast maps.
*
* @param client - a Hazelcast client.
*/

void map_example(hazelcast::client::hazelcast_client client) {
    auto cities = client.get_map("cities").get();
    cities->put<std::string, hazelcast::client::hazelcast_json_value>("1", as_json("United Kingdom", "London", 9540576)).get();
    cities->put<std::string, hazelcast::client::hazelcast_json_value>("2", as_json("United Kingdom", "Manchester", 2770434)).get();
    cities->put<std::string, hazelcast::client::hazelcast_json_value>("3", as_json("United States", "New York", 19223191)).get();
    cities->put<std::string, hazelcast::client::hazelcast_json_value>("4", as_json("United States", "Los Angeles", 3985520)).get();
    cities->put<std::string, hazelcast::client::hazelcast_json_value>("5", as_json("Turkey", "Ankara", 5309690)).get();
    cities->put<std::string, hazelcast::client::hazelcast_json_value>("6", as_json("Turkey", "Istanbul", 15636243)).get();
    cities->put<std::string, hazelcast::client::hazelcast_json_value>("7", as_json("Brazil", "Sao Paulo", 22429800)).get();
    cities->put<std::string, hazelcast::client::hazelcast_json_value>("8", as_json("Brazil", "Rio de Janeiro", 13634274)).get();
    int map_size = cities->size().get();
    std::printf("'cities' map now contains %d entries.\n", map_size);
    std::cout << "--------------------" << std::endl;
}

/**
* This example shows how to work with Hazelcast SQL queries.
*
* @param client - a Hazelcast client.
*/

void non_stop_map_example(hazelcast::client::hazelcast_client client) {
    std::cout << "Now the map named 'map' will be filled with random entries." << std::endl;
    std::cout << "--------------------" << std::endl;
    auto map = client.get_map("map").get();
    int iteration_counter = 0;
    while (true) {
        int random_key = std::rand() % 100000;
        map->put<std::string, std::string>("key-" + std::to_string(random_key), "value-" + std::to_string(random_key)).get();
        map->get<std::string, std::string>("key-" + std::to_string(std::rand() % 100000)).get();
        if (++iteration_counter == 10) {
            iteration_counter = 0;
            std::cout << "Current map size: " + std::to_string(map->size().get()) << std::endl;
        }
    }
}
