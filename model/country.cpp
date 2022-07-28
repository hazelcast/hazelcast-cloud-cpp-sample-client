#include "country.h"
hazelcast::client::hazelcast_json_value country::as_json(std::string isoCode, std::string country) {
    return hazelcast::client::hazelcast_json_value("{\"isocode\":\"" + isoCode + "\",\"country\":\"" + country + "\"}");
}
