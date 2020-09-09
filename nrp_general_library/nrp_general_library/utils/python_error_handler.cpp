#include "nrp_general_library/utils/python_error_handler.h"

#include <boost/python.hpp>

std::string handle_pyerror()
{
	using namespace boost::python;

	// Fetch Exception
	PyObject *exc,*val,*tb;
	object formatted_list, formatted;

	PyErr_Fetch(&exc,&val,&tb);
	PyErr_NormalizeException(&exc,&val,&tb);

	// Manage references
	handle<> hexc(exc), hval(allow_null(val)), htb(allow_null(tb));

	// Create traceback
	object traceback(import("traceback"));
//	if (!tb)
//	{
//		//PyException_SetTraceback(val, tb);

//		object format_exception_only(traceback.attr("format_exception_only"));
//		formatted_list = format_exception_only(hexc,hval);
//	}
//	else
//	{
//		object format_exception(traceback.attr("format_exception"));
//		formatted_list = format_exception(hexc,hval,htb);
//	}

	if (!tb)
		PyException_SetTraceback(val, tb);

	object format_exception(traceback.attr("format_exception"));
	formatted_list = format_exception(hexc,hval,htb);

	formatted = str("").join(formatted_list);

	// Create string
	return extract<std::string>(formatted);
}
