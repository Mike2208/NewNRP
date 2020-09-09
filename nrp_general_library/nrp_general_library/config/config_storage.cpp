#include "nrp_general_library/config/config_storage.h"

void from_json(const nlohmann::json &j, ConfigStorage<nlohmann::json> &eng)
{
	eng.Data = j;
}
