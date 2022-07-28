#include <hazelcast/client/hazelcast_client.h>
#ifndef HAZELCASTCLOUD_COUNTRY_H
#define HAZELCASTCLOUD_COUNTRY_H
class country{
public:
    static hazelcast::client::hazelcast_json_value as_json(std::string isoCode, std::string country);
    };
#endif //HAZELCASTCLOUD_COUNTRY_H
