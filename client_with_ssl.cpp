// Copyright (c) 2008-2023, Hazelcast, Inc. All Rights Reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <string>
#include <hazelcast/client/hazelcast_client.h>

/**
 * A sample application that configures a client to connect to an Hazelcast
 * Viridian cluster over TLS, and to then insert and fetch data with SQL, thus
 * testing that the connection to the Hazelcast Viridian cluster is successful.
 *
 * See: https://docs.hazelcast.com/cloud/get-started
 */

void
create_mapping(hazelcast::client::hazelcast_client client);
void
insert_cities(hazelcast::client::hazelcast_client client);
void
fetch_cities(hazelcast::client::hazelcast_client client);

struct CityDTO
{
    std::string cityName;
    std::string country;
    int population;
};

// CityDTO serializer
namespace hazelcast {
namespace client {
namespace serialization {

template<>
struct hz_serializer<CityDTO> : compact::compact_serializer
{
    static void write(const CityDTO& object, compact::compact_writer& out)
    {
        out.write_int32("population", object.population);
        out.write_string("city", object.cityName);
        out.write_string("country", object.country);
    }

    static CityDTO read(compact::compact_reader& in)
    {
        CityDTO c;

        c.population = in.read_int32("population");
        boost::optional<std::string> city = in.read_string("city");

        if (city) {
            c.cityName = *city;
        }

        boost::optional<std::string> country = in.read_string("country");

        if (country) {
            c.country = *country;
        }

        return c;
    }

    static std::string type_name() { return "city"; }
};

} // namespace serialization
} // namespace client
} // namespace hazelcast

int
main(int argc, char** argv)
{
    hazelcast::client::client_config config;

    config.set_cluster_name("YOUR_CLUSTER_NAME");
    config.set_property(hazelcast::client::client_properties::CLOUD_URL_BASE,
                        "YOUR_DISCOVERY_URL");
    config.set_property("hazelcast.client.statistics.enabled", "true");

    auto& cloud_configuration = config.get_network_config().get_cloud_config();
    cloud_configuration.enabled = true;
    cloud_configuration.discovery_token = "YOUR_CLUSTER_DISCOVERY_TOKEN";

    boost::asio::ssl::context ctx(boost::asio::ssl::context::tlsv12);

    try {
        ctx.load_verify_file("ca.pem");
        ctx.use_certificate_file("cert.pem", boost::asio::ssl::context::pem);
        ctx.set_password_callback(
          [&](std::size_t max_length,
              boost::asio::ssl::context::password_purpose purpose) {
              return "YOUR_SSL_PASSWORD";
          });
        ctx.use_private_key_file("key.pem", boost::asio::ssl::context::pem);
    } catch (std::exception& e) {
        std::cerr << "You should copy ca.pem, cert.pem and key.pem files to "
                     "the working directory, exception cause "
                  << e.what() << std::endl;
        exit(EXIT_FAILURE);
    }
    config.get_network_config().get_ssl_config().set_context(std::move(ctx));
    config.get_logger_config().level(hazelcast::logger::level::info);

    auto client = hazelcast::new_client(std::move(config)).get();
    std::cout << "Connection Successful!" << std::endl;

    create_mapping(client);
    insert_cities(client);
    fetch_cities(client);

    client.shutdown().get();
}

void
create_mapping(hazelcast::client::hazelcast_client client)
{
    // Mapping is required for your distributed map to be queried over SQL.
    // See: https://docs.hazelcast.com/hazelcast/latest/sql/mapping-to-maps

    std::cout << "Creating the mapping..." << std::endl;

    auto sql = client.get_sql();

    auto result = sql
                    .execute(R"(CREATE OR REPLACE MAPPING 
                                    cities (
                                        __key INT,                                        
                                        country VARCHAR,
                                        city VARCHAR,
                                        population INT) TYPE IMAP
                                    OPTIONS ( 
                                        'keyFormat' = 'int',
                                        'valueFormat' = 'compact',
                                        'valueCompactTypeName' = 'city'))")
                    .get();

    std::cout << "OK." << std::endl;
}

void
insert_cities(hazelcast::client::hazelcast_client client)
{
    std::cout << "Inserting cities into 'cities' map..." << std::endl;

    auto sql = client.get_sql();

    try {
        // Create mapping for the integers. This needs to be done only once per
        // map.
        auto result = sql
                        .execute(R"(INSERT INTO cities 
                    (__key, city, country, population) VALUES
                    (1, 'London', 'United Kingdom', 9540576),
                    (2, 'Manchester', 'United Kingdom', 2770434),
                    (3, 'New York', 'United States', 19223191),
                    (4, 'Los Angeles', 'United States', 3985520),
                    (5, 'Istanbul', 'Turkey', 15636243),
                    (6, 'Ankara', 'Turkey', 5309690),
                    (7, 'Sao Paulo ', 'Brazil', 22429800))")
                        .get();

        std::cout << "OK." << std::endl;
    } catch (hazelcast::client::exception::iexception& e) {
        // don't panic for duplicated keys.
        std::cerr << "FAILED, duplicated keys " << e.what() << std::endl;
    }

    // Let's also add a city as object.
    std::cout << "Putting a city into 'cities' map..." << std::endl;
    auto map = client.get_map("cities").get();
    map->put(8, CityDTO{ "Rio de Janeiro", "Brazil", 13634274 });
    std::cout << "OK." << std::endl;
}

void
fetch_cities(hazelcast::client::hazelcast_client client)
{
    std::cout << "Fetching cities via SQL..." << std::endl;

    auto result =
      client.get_sql().execute("SELECT __key, this FROM cities").get();

    std::cout << "OK." << std::endl;
    std::cout << "--Results of 'SELECT __key, this FROM cities'" << std::endl;

    std::printf("| %-4s | %-20s | %-20s | %-15s |\n",
                "id",
                "country",
                "city",
                "population");

    for (auto itr = result->iterator(); itr.has_next();) {
        auto page = itr.next().get();

        for (auto const& row : page->rows()) {

            auto id = row.get_object<int32_t>("__key");
            auto city = row.get_object<CityDTO>("this");
            std::printf("| %-4d | %-20s | %-20s | %-15d |\n",
                        *id,
                        city->country.c_str(),
                        city->cityName.c_str(),
                        city->population);
        }
    }

    std::cout
      << "\n!! Hint !! You can execute your SQL queries on your Viridian "
         "cluster over the management center. \n 1. Go to 'Management Center' "
         "of your Hazelcast Viridian cluster. \n 2. Open the 'SQL Browser'. \n "
         "3. Try to execute 'SELECT * FROM cities'.\n";
}
