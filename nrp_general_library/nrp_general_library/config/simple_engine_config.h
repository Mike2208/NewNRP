#ifndef SIMPLE_ENGINE_CONFIG_H
#define SIMPLE_ENGINE_CONFIG_H

#include "nrp_general_library/config/engine_config.h"

template<FixedString ENGINE_TYPE>
class SimpleEngineConfig
        : public EngineConfig<SimpleEngineConfig<ENGINE_TYPE>, PropNames<> >
{
	public:
		static constexpr auto DefEngineType = ENGINE_TYPE;

		template<class ...T>
		SimpleEngineConfig(T &&...params)
		    : EngineConfig<SimpleEngineConfig<ENGINE_TYPE>, PropNames<> >(std::forward<T>(params)...)
		{}
};

#endif // SIMPLE_ENGINE_CONFIG_H
