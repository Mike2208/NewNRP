/* * NRP Core - Backend infrastructure to synchronize simulations
 *
 * Copyright 2020 Michael Zechmair
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * This project has received funding from the European Union’s Horizon 2020
 * Framework Programme for Research and Innovation under the Specific Grant
 * Agreement No. 945539 (Human Brain Project SGA3).
 */

#ifndef PYTHON_ARRAY_CONVERTER_H
#define PYTHON_ARRAY_CONVERTER_H

#include <boost/python.hpp>
#include <boost/python/numpy.hpp>
#include <boost/python/to_python_indirect.hpp>
#include <concepts>

#include <iostream>

template<class VALUE, size_t SIZE, class MakeHolder>
requires(std::is_scalar_v<VALUE>)
struct boost::python::to_python_indirect<const std::array<VALUE,SIZE>&, MakeHolder>
{
	public:
	    inline PyObject* operator()(const std::array<VALUE,SIZE> &ref) const
		{
			namespace python = boost::python;
			namespace np = boost::python::numpy;

			np::ndarray py_array = np::from_data(const_cast<VALUE*>(ref.data()), np::dtype::get_builtin<VALUE>(),
			                                     python::make_tuple(SIZE),
			                                     python::make_tuple(sizeof(VALUE)),
			                                     python::object());

			return python::incref(py_array.ptr());
		}

        #ifndef BOOST_PYTHON_NO_PY_SIGNATURES
		inline const PyTypeObject*get_pytype()const
		{
			namespace python = boost::python;
			namespace np = boost::python::numpy;

			return python::converter::registered_pytype<const std::array<VALUE,SIZE>&>::get_pytype();
		}
        #endif
};

template<class T>
concept PY_CONVERTIBLE_ARRAY_C =
        requires {	typename T::value_type;	} &&
        requires {	sizeof(std::tuple_size<T>);	} &&
        std::same_as<T, std::array<typename T::value_type, std::tuple_size_v<T> > >&&
        std::is_scalar_v<typename T::value_type>;


template<PY_CONVERTIBLE_ARRAY_C ARRAY>
struct array_from_python
{
	static constexpr auto Size = std::tuple_size_v<ARRAY>;
	using type_t = typename ARRAY::value_type;

	static void registerConverter()
	{
		boost::python::converter::registry::push_back(
		            &convertible,
		            &construct,
		            boost::python::type_id<ARRAY>());
	}

	static void* convertible(PyObject *obj_ptr)
	{
		namespace python = boost::python;
		namespace np = boost::python::numpy;

		auto *pType = const_cast<PyTypeObject*>(python::converter::object_manager_traits<np::ndarray>::get_pytype());
		return boost::python::pytype_check(pType, obj_ptr);
	}

	static void construct(PyObject *obj_ptr, boost::python::converter::rvalue_from_python_stage1_data *data)
	{
		namespace python = boost::python;
		namespace np = boost::python::numpy;

		python::handle pyHandle(python::borrowed(obj_ptr));
		python::object pyObj(pyHandle);

		const auto npArray = np::from_object(pyObj);
		if(npArray.get_nd() != 1
		        || *npArray.get_shape() != Size
		        || !np::equivalent(np::dtype::get_builtin<type_t>(), npArray.get_dtype()))
			python::throw_error_already_set();

		ARRAY *const pArray = reinterpret_cast<ARRAY*>(((boost::python::converter::rvalue_from_python_storage<ARRAY>*)data)->storage.bytes);
		new (pArray) ARRAY();

		memcpy(pArray->data(), npArray.get_data(), sizeof(type_t)*Size);

		data->convertible = pArray;
	}
};

template<class VALUE, class MakeHolder>
requires(std::is_scalar_v<VALUE>)
struct boost::python::to_python_indirect<const std::vector<VALUE>&, MakeHolder>
{
	public:
	    inline PyObject* operator()(const std::vector<VALUE> &ref) const
		{
			namespace python = boost::python;
			namespace np = boost::python::numpy;

			np::ndarray py_array = np::from_data(const_cast<VALUE*>(ref.data()), np::dtype::get_builtin<VALUE>(),
			                                     python::make_tuple(ref.size()),
			                                     python::make_tuple(sizeof(VALUE)),
			                                     python::object());

			return python::incref(py_array.ptr());
		}

        #ifndef BOOST_PYTHON_NO_PY_SIGNATURES
		inline const PyTypeObject*get_pytype()const
		{
			namespace python = boost::python;
			namespace np = boost::python::numpy;

			return python::converter::registered_pytype<const std::vector<VALUE>&>::get_pytype();
		}
        #endif
};

template<class T>
concept PY_CONVERTIBLE_VECTOR_C =
    requires() {	typename T::value_type;	} &&
    std::same_as<T, std::vector<typename T::value_type> > &&
    std::is_scalar_v<typename T::value_type>;

template<PY_CONVERTIBLE_VECTOR_C VECTOR>
struct vector_from_python
{
	using type_t = typename VECTOR::value_type;

	static void registerConverter()
	{
		boost::python::converter::registry::push_back(
		            &convertible,
		            &construct,
		            boost::python::type_id<VECTOR>());
	}

	static void* convertible(PyObject *obj_ptr)
	{
		namespace python = boost::python;
		namespace np = boost::python::numpy;

		auto *pType = const_cast<PyTypeObject*>(python::converter::object_manager_traits<np::ndarray>::get_pytype());
		return boost::python::pytype_check(pType, obj_ptr);
	}

	static void construct(PyObject *obj_ptr, boost::python::converter::rvalue_from_python_stage1_data *data)
	{
		namespace python = boost::python;
		namespace np = boost::python::numpy;

		python::handle pyHandle(python::borrowed(obj_ptr));
		python::object pyObj(pyHandle);

		const auto npArray = np::from_object(pyObj);
		if(npArray.get_nd() != 1 ||
		        !np::equivalent(np::dtype::get_builtin<type_t>(), npArray.get_dtype()))
			python::throw_error_already_set();

		VECTOR *const pVector = reinterpret_cast<VECTOR*>(((boost::python::converter::rvalue_from_python_storage<VECTOR>*)data)->storage.bytes);
		new (pVector) VECTOR();
		pVector->resize(*npArray.get_shape());

		memcpy(pVector->data(), npArray.get_data(), sizeof(type_t)*(*npArray.get_shape()));

		data->convertible = pVector;
	}
};


#endif // PYTHON_ARRAY_CONVERTER_H
