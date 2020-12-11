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

#include "nrp_general_library/utils/python_error_handler.h"

#include <boost/python.hpp>

std::string handle_pyerror()
{
	using namespace boost::python;

	if(PyErr_Occurred())
	{

		// Fetch Exception
		PyObject *exc,*val,*tb;
		object formatted_list, formatted;

		PyErr_Fetch(&exc,&val,&tb);
		PyErr_NormalizeException(&exc,&val,&tb);

		// Manage references
		handle<> hexc(exc), hval(allow_null(val)), htb(allow_null(tb));

		// Create traceback
		object traceback(import("traceback"));
		if (!tb)
		{
			PyException_SetTraceback(val, tb);

			object format_exception_only(traceback.attr("format_exception_only"));
			formatted_list = format_exception_only(hexc,hval);
		}
		else
		{
			object format_exception(traceback.attr("format_exception"));
			formatted_list = format_exception(hexc,hval,htb);
		}

//		if (!tb)
//			PyException_SetTraceback(val, tb);

//		object format_exception(traceback.attr("format_exception"));
//		formatted_list = format_exception(hexc,hval,htb);

		formatted = str("").join(formatted_list);
		const std::string retVal = extract<std::string>(formatted);

		// Create string
		PyErr_Clear();
		return retVal;
	}

	return "";
}
