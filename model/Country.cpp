#include "Country.h"
hazelcast::client::hazelcast_json_value Country::asJson(std::string isoCode, std::string country) {
    return hazelcast::client::hazelcast_json_value("{\"isocode\":\"" + isoCode + "\",\"country\":\"" + country + "\"}");
}
