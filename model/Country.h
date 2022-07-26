#include <hazelcast/client/hazelcast_client.h>
#ifndef HAZELCASTCLOUD_COUNTRY_H
#define HAZELCASTCLOUD_COUNTRY_H
class Country{
public:
    static hazelcast::client::hazelcast_json_value asJson(std::string isoCode, std::string country);
    };
#endif //HAZELCASTCLOUD_COUNTRY_H
