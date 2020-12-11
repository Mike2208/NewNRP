//
// NRP Core - Backend infrastructure to synchronize simulations
//
// Copyright 2020 Michael Zechmair
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//   http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
// This project has received funding from the European Union’s Horizon 2020
// Framework Programme for Research and Innovation under the Specific Grant
// Agreement No. 945539 (Human Brain Project SGA3).
//

#include <gtest/gtest.h>

#include "nrp_general_library/device_interface/device.h"

using namespace testing;

TEST(DeviceIdentifierTest, Constructor)
{
	const std::string deviceName1("deviceName1");
	const std::string deviceType1("deviceType1");
	const std::string engineType1("engine1");

	DeviceIdentifier identifier1(deviceName1, engineType1, deviceType1);

	ASSERT_STREQ(identifier1.Name.data(), deviceName1.data());
	ASSERT_STREQ(identifier1.Type.data(), deviceType1.data());
	ASSERT_STREQ(identifier1.EngineName.data(), engineType1.data());
}

TEST(DeviceIdentifierTest, Operators)
{
	const std::string deviceName1("deviceName1");
	const std::string deviceType1("deviceType1");
	const std::string engineType1("engine1");

	const std::string deviceName2("deviceName2");
	const std::string deviceType2("deviceType2");
	const std::string engineType2("engine2");

	DeviceIdentifier identifier1(deviceName1, engineType1, deviceType1);
	DeviceIdentifier identifier2(deviceName2, engineType2, deviceType2);

	DeviceIdentifier identifier3 = identifier1;

	// Check equality
	ASSERT_FALSE(identifier1 == identifier2);
	ASSERT_TRUE(identifier1 == identifier3);

	// Check less than operator (for set placement)
	ASSERT_TRUE(identifier1 < identifier2);
	ASSERT_FALSE(identifier2 < identifier1);

	ASSERT_FALSE(identifier1 < identifier3);
	ASSERT_FALSE(identifier3 < identifier1);
}

TEST(DeviceInterfaceTest, Constructor)
{
	const std::string deviceName1("deviceName1");
	const std::string deviceType1("deviceType1");
	const std::string engineType1("engine1");

	const std::string deviceName2("deviceName2");
	const std::string deviceType2("deviceType2");
	const std::string engineType2("engine2");

	DeviceInterface interface(deviceName1, engineType1, deviceType1);

	ASSERT_STREQ(interface.name().data(), deviceName1.data());
	ASSERT_STREQ(interface.type().data(), deviceType1.data());
	ASSERT_STREQ(interface.engineName().data(), engineType1.data());

	interface.setName(deviceName2);
	ASSERT_STREQ(interface.name().data(), deviceName2.data());

	interface.setType(deviceType2);
	ASSERT_STREQ(interface.type().data(), deviceType2.data());

	interface.setEngineName(engineType2);
	ASSERT_STREQ(interface.engineName().data(), engineType2.data());

	const DeviceIdentifier id1(deviceName1, engineType1, deviceType1);
	interface.setID(id1);

	ASSERT_STREQ(interface.name().data(), id1.Name.data());
	ASSERT_STREQ(interface.type().data(), id1.Type.data());
	ASSERT_STREQ(interface.engineName().data(), id1.EngineName.data());

	ASSERT_EQ(interface.id(), id1);
}
