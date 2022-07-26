#include <hazelcast/client/hazelcast_client.h>
#ifndef HAZELCASTCLOUD_CITY_H
#define HAZELCASTCLOUD_CITY_H
class City{
public:
    static hazelcast::client::hazelcast_json_value asJson(std::string, std::string, int);
    };
#endif //HAZELCASTCLOUD_CITY_H
