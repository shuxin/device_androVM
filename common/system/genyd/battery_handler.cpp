
#include <cutils/properties.h>

#include "dispatcher.hpp"
#include "libgenyd.hpp"

void Dispatcher::setBatteryStatus(const Request &request, Reply *reply)
{
    std::string value = request.parameter().value().stringvalue();

    if (value != "Charging" && value != "Discharging" &&
        value != "Not charging" && value != "Full") {

        SLOGD("Unknown \"%s\" battery status", value.c_str());

        reply->set_type(Reply::Error);
        Status *status = reply->mutable_status();
        status->set_code(Status::InvalidRequest);

        return;
    }

    int ret = property_set(BATTERY_STATUS, value.c_str());
    SLOGD("Setting [%s] to \"%s\": %d", BATTERY_STATUS, value.c_str(), ret);
}

void Dispatcher::getBatteryStatus(const Request &request, Reply *reply)
{
    // Prepare response
    reply->set_type(Reply::Value);
    Status *status = reply->mutable_status();
    status->set_code(Status::Ok);
    Value *value = reply->mutable_value();
    value->set_type(Value::String);

    char property_value[PROPERTY_VALUE_MAX];
    property_get(BATTERY_STATUS CACHE_SUFFIX, property_value, "Unknown");

    if (!LibGenyd::useRealValue(BATTERY_VALUE)) {
        property_get(BATTERY_STATUS, property_value, "Unknown");
    }

    // Set value in response
    value->set_stringvalue(property_value);
}

void Dispatcher::getBatteryValue(const Request &request, Reply *reply)
{
    // Read keys
    char property_full[PROPERTY_VALUE_MAX];
    char property_value[PROPERTY_VALUE_MAX];
    property_get(BATTERY_FULL CACHE_SUFFIX, property_full, "0");
    property_get(BATTERY_VALUE CACHE_SUFFIX, property_value, "0");

    int efull = atoi(property_full);
    int enow = atoi(property_value);

    // Compute battery level
    int batlevel = efull ? ((long long)enow)*100/efull : 0;

    // Prepare response
    reply->set_type(Reply::Value);
    Status *status = reply->mutable_status();
    status->set_code(Status::Ok);
    Value *value = reply->mutable_value();
    value->set_type(Value::Uint);

    // Set value in response
    value->set_uintvalue(batlevel);
}

void Dispatcher::isBatteryManual(const Request &request, Reply *reply)
{
    // Prepare response
    reply->set_type(Reply::Value);
    Status *status = reply->mutable_status();
    status->set_code(Status::Ok);
    Value *value = reply->mutable_value();
    value->set_type(Value::Bool);

    // Set value in response
    value->set_boolvalue(!LibGenyd::useRealValue(BATTERY_VALUE));
}
