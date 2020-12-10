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

#ifndef PROPERTY_TEMPLATE_H
#define PROPERTY_TEMPLATE_H

#include "nrp_general_library/utils/fixed_string.h"
#include "nrp_general_library/utils/nrp_exceptions.h"

#include <concepts>
#include <exception>
#include <stdexcept>
#include <tuple>


/*!
 * \brief Exception that is raised when a specified property cannot be found
 */
class NRPExceptionMissingProperty
        : public NRPExceptionRecoverable
{
	public:
		template<class T>
		NRPExceptionMissingProperty(T &&msg, bool isLogged = false)
		    : NRPExceptionRecoverable(std::forward<T>(msg), isLogged)
		{}

		~NRPExceptionMissingProperty() override;
};

class ObjectDeserializerGeneral;

template<class T>
concept PROPERTY_SERIALIZER_C = std::is_base_of_v<ObjectDeserializerGeneral, std::remove_cv_t<std::decay_t<T> > >;

template<class T>
concept PROPERTY_OPTION_PROP_C = requires(T a, const T &b) {
        { a == b };		// Property requires a comparison operator
        { T(b) };		// Property must be copy constructible
};

template<class T, class U>
concept PROPERTY_CONSTRUCTIBLE_C = requires {
    {	std::constructible_from<U, T>	};
};

struct PropNamesGeneral
{};

template<class T>
concept PROP_NAMES_C = (std::is_base_of_v<PropNamesGeneral, T>);

template<FixedString ...NAMES>
struct PropNames
        : public PropNamesGeneral
{
	static constexpr unsigned NumNames()
	{	return sizeof...(NAMES);	}

	template<unsigned ID>
	static constexpr std::string_view getName()
	{
		static_assert(sizeof...(NAMES) > ID, "No name with given ID stored");
		return std::string_view(std::get<ID>(std::forward_as_tuple(NAMES...)));
	}

	template<FixedString NAME_T>
	static constexpr unsigned getID()
	{	return getID<NAME_T, 0>();	}

	private:
	    template<FixedString NAME_T, unsigned ID_T>
	    static constexpr unsigned getID()
		{
			if constexpr (getName<ID_T>().compare(std::string_view(NAME_T)) == 0)
			{
				return ID_T;
			}
			else if constexpr (ID_T+1 < NumNames())
			{
				return getID<NAME_T, ID_T+1>();
			}
			else
			{
				return NumNames();
			}
		}
};

template<class ...PROP_NAMES>
struct MultiPropNames
        : public PropNamesGeneral
{
	static constexpr unsigned NumNames()
	{	return NumNames<PROP_NAMES...>(0);	}

	template<unsigned ID>
	static constexpr std::string_view getName()
	{
		static_assert(sizeof...(PROP_NAMES) > 0, "ID not found");
		return MultiPropNames::getName<ID, 0, 0, PROP_NAMES...>();
	}

	template<FixedString NAME_T>
	static constexpr unsigned getID()
	{	return getID<NAME_T, PROP_NAMES...>(0);	}

	private:
	    template<unsigned ID, unsigned CUR_ID, unsigned PREV_PROP_NAME_IDS, PROP_NAMES_C PROP_NAME, class ...REST>
	    static constexpr std::string_view getName()
		{
			if constexpr (CUR_ID-PREV_PROP_NAME_IDS >= PROP_NAME::NumNames())
			{
				static_assert(sizeof...(REST) > 0, "ID not found");
				return MultiPropNames::template getName<ID, CUR_ID, CUR_ID, REST...>();
			}
			else if constexpr (ID == CUR_ID)
			{
				return PROP_NAME::template getName<CUR_ID-PREV_PROP_NAME_IDS>();
			}
			else
			{
				return MultiPropNames::template getName<ID, CUR_ID+1, PREV_PROP_NAME_IDS, PROP_NAME, REST...>();
			}
		}

		template<PROP_NAMES_C NAMES, class ...REST>
		static constexpr unsigned NumNames(const unsigned numNames)
		{
			if constexpr (sizeof...(REST) > 0)
			{
				return MultiPropNames::template NumNames<REST...>(numNames + NAMES::NumNames());
			}
			else
			{
				return numNames + NAMES::NumNames();
			}
		}

		template<FixedString NAME_T, class PROP_NAME, class ...REST>
		static constexpr unsigned getID(const unsigned curTotalID)
		{
			const auto curID = PROP_NAME::template getID<NAME_T>();
			if constexpr (curID < PROP_NAME::NumNames())
			{
				return curTotalID + curID;
			}
			else if constexpr (sizeof...(REST) > 0)
			{
				return getID<NAME_T, REST...>(curTotalID + PROP_NAME::NumNames());
			}
			else
			{
				return curTotalID + PROP_NAME::NumNames();
			}
		}
};

/*!
 * \brief General Interface to PropertyTemplate
 */
class PropertyTemplateGeneral
{
	public:
		/*!
		 * \brief Parent Class of PropertyConfig. Used to check type
		 */
		struct PropertyConfigGeneral
		{};

		/*!
		 *	\brief Pass additional options to property. Can be used to adjust behavior and set a default value
		 *	\tparam PROPERTY Property type
		 *	\tparam WRITE_DEFAULT Should the default value be written?
		 */
		template<PROPERTY_OPTION_PROP_C PROPERTY, bool WRITE_DEFAULT>
		struct PropertyConfig
		    : public PropertyConfigGeneral
		{
			/*!
			 *	\brief Property Type
			 */
			using property_t = PROPERTY;

			/*!
			 * \brief Should this property be serialized if value is the same as default?
			 */
			static constexpr bool WriteDefault = WRITE_DEFAULT;
		};

		template<class CLASS, int ID, class PROPERTY, bool WRITE_DEFAULT>
		struct PropertyTemplateConfig;

		template<class CLASS, int ID, class PROPERTY>
		struct PropertyTemplateConfig<CLASS, ID, PROPERTY, true>
		{
			static constexpr bool WriteDefault = true;

			template<class T>
			PropertyTemplateConfig(T&&)
			{}

			PropertyTemplateConfig() = default;
		};

		template<class CLASS, int ID, class PROPERTY>
		struct PropertyTemplateConfig<CLASS, ID, PROPERTY, false>
		{
			static constexpr bool WriteDefault = false;

			PROPERTY DefaultValue;

			PropertyTemplateConfig(const PROPERTY &_defaultValue)
			    : DefaultValue(_defaultValue)
			{}
		};

		template<class PROPERTY_T>
		struct property_config;

		template<class PROPERTY_T>
		requires std::is_base_of_v<PropertyConfigGeneral, std::decay_t<PROPERTY_T> >
		struct property_config<PROPERTY_T>
		{
			using type = typename PROPERTY_T::property_t;
			static constexpr bool WriteDefault = PROPERTY_T::WriteDefault;
		};

		template<class PROPERTY_T>
		requires (!std::is_base_of_v<PropertyConfigGeneral, std::decay_t<PROPERTY_T> >)
		struct property_config<PROPERTY_T>
		{
			using type = PROPERTY_T;
			static constexpr bool WriteDefault = true;
		};

		template<class PROPERTY_T>
		using property_value_t = typename property_config<PROPERTY_T>::type;

		template<class PROPERTY_T>
		static constexpr bool property_write_default_v = property_config<PROPERTY_T>::WriteDefault;


		template<class CLASS, int ID, PROP_NAMES_C PROP_NAMES, class ...REMAINING_PROPERTIES>
		struct PropertyTemplateInternal
		{
			/*!
			 *	\brief Last Constructor. Only used as a last base class constructor to prevent compile errors
			 */
			template<class ...T>
			PropertyTemplateInternal(T &&...)
			{}

			PropertyTemplateInternal() = default;
		};

		/*!
		 *	\brief Internal PropertyTemplate. Contains a property value as well as a name
		 *	\tparam CLASS Final derived class of PropertyTemplateInternal
		 *	\tparam ID ID of current property
		 *	\tparam PROP_NAMES Template class of type PropNames<...>, containing all property names
		 *	\tparam PROPERTY Class of current property
		 *	\tparam REMAINING_PROPERTIES Remaining class properties
		 */
		template<class CLASS, int ID, PROP_NAMES_C PROP_NAMES, class PROPERTY, class ...REMAINING_PROPERTIES>
		struct PropertyTemplateInternal<CLASS, ID, PROP_NAMES, PROPERTY, REMAINING_PROPERTIES...>
		        : public PropertyTemplateConfig<CLASS, ID, PropertyTemplateGeneral::property_value_t<PROPERTY>, property_write_default_v<PROPERTY> >,
		          public PropertyTemplateInternal<CLASS, ID+1, PROP_NAMES, REMAINING_PROPERTIES...>
		{
				using base_property_internal_t = PropertyTemplateInternal<CLASS, ID+1, PROP_NAMES, REMAINING_PROPERTIES...>;

			protected:
				using property_config_t = PropertyTemplateConfig<CLASS, ID, PropertyTemplateGeneral::property_value_t<PROPERTY>, property_write_default_v<PROPERTY> >;
				using property_value_t = typename PropertyTemplateGeneral::property_value_t<PROPERTY>;

				/*!
				 * \brief Name of this property
				 */
				static constexpr std::string_view Name = PROP_NAMES::template getName<ID>();

				/*!
				 * \brief Property variable
				 */
				property_value_t Property;

				/*!
				 *	\brief Gets Property with ID _ID
				 *	\tparam _ID ID to retrieve
				 *	\tparam PROPERTY_T Class of property to retrieve
				 *	\return Returns Property (converted to const PROPERTY_T&)
				 */
				template<int _ID, class PROPERTY_T>
				constexpr const PROPERTY_T &getProperty() const
				{
					if constexpr (ID == _ID)
					{
						static_assert(std::is_convertible_v<property_value_t&, PROPERTY_T&>, "Could not convert property to given type");
						return this->Property;
					}
					else
					{
						static_assert(sizeof...(REMAINING_PROPERTIES) > 0, "ID not in PropertyTemplate");
						return this->base_property_internal_t::template getProperty<_ID, PROPERTY_T>();
					}
				}

				/*!
				 *	\brief Gets Property with ID _ID
				 *	\tparam _ID ID to retrieve
				 *	\tparam PROPERTY_T Class of property to retrieve
				 *	\return Returns Property (converted to PROPERTY_T&)
				 */
				template<int _ID, class PROPERTY_T>
				constexpr PROPERTY_T &getProperty()
				{
					if constexpr (ID == _ID)
					{
						static_assert(std::is_convertible_v<property_value_t&, PROPERTY_T&>, "Could not convert property to given type");
						return this->Property;
					}
					else
					{
						static_assert(sizeof...(REMAINING_PROPERTIES) > 0, "ID not in PropertyTemplate");
						return this->base_property_internal_t::template getProperty<_ID, PROPERTY_T>();
					}
				}

				/*!
				 *	\brief Gets Name of Property with ID _ID
				 *	\tparam _ID ID of name to retrieve
				 *	\return Returns Name
				 */
				template<int _ID>
				static constexpr std::string_view getName()
				{
					if constexpr (ID == _ID)
					{
						return Name;
					}
					else
					{
						static_assert(sizeof...(REMAINING_PROPERTIES) > 0, "ID not in PropertyTemplate");
						return base_property_internal_t::template getName<_ID>();
					}
				}


				/*!
				 *	\brief Returns Property stored under _name
				 *	\tparam PROPERTY_T Class to retrieve
				 *	\param _name Name of property
				 *	\return Returns Property (converted to const PROPERTY_T&)
				 */
				template<FixedString NAME, class PROPERTY_T = PROPERTY>
				constexpr const PROPERTY_T &getPropertyByName() const
				{
					if constexpr (NAME.compare(Name) == 0)
					{
						static_assert(std::is_same_v<std::decay_t<property_value_t>, PROPERTY_T>, "Property is incorrect type");
						return this->Property;
					}
					else
					{
						static_assert(sizeof...(REMAINING_PROPERTIES) > 0, "Requested Property not found");
						return this->base_property_internal_t::template getPropertyByName<NAME, PROPERTY_T>();
					}
				}

				/*!
				 *	\brief Returns Property stored under _name
				 *	\tparam PROPERTY_T Class to retrieve
				 *	\param _name Name of property
				 *	\return Returns Property (converted to PROPERTY_T&)
				 */
				template<FixedString NAME, class PROPERTY_T = PROPERTY>
				constexpr PROPERTY_T &getPropertyByName()
				{
					if constexpr (NAME.compare(Name) == 0)
					{
						static_assert(std::is_same_v<std::decay_t<property_value_t>, PROPERTY_T>, "Property is incorrect type");
						return this->Property;
					}
					else
					{
						static_assert(sizeof...(REMAINING_PROPERTIES) > 0, "Requested Property not found");
						return this->base_property_internal_t::template getPropertyByName<NAME, PROPERTY_T>();
					}
				}

				/*!
				 *	\brief Constructor. Takes the property's name as well as a default value.
				 *	If _serializer fails to find a corresponding property value, the property is initialized with _defaultValue.
				 *	Any additional parameters are passed onto the base property class
				 *	\tparam PROPERTY_SERIALIZER_T Class derived from PropertySerializerBase
				 *	\tparam PROPERTY_T Class convertible to property_value_t
				 *	\tparam T Types for base class parameters
				 *	\param _serializer Property Serializer
				 *	\param _name Property Name
				 *	\param _defaultValue Value with which to initialize Property if _serializer fails to find any
				 *	\param _other Parameters for base class
				 */
				template<PROPERTY_SERIALIZER_C PROPERTY_SERIALIZER_T, PROPERTY_CONSTRUCTIBLE_C<PROPERTY> PROPERTY_T, class ...T>
				PropertyTemplateInternal(PROPERTY_SERIALIZER_T &&_serializer, PROPERTY_T &&_defaultValue, T &&..._other)
				    : property_config_t(std::forward<PROPERTY_T>(_defaultValue)),
				      base_property_internal_t(std::forward<PROPERTY_SERIALIZER_T>(_serializer), std::forward<T>(_other)...),
				      Property(getSerializerValOrDefault(std::forward<PROPERTY_SERIALIZER_T>(_serializer), std::forward<PROPERTY_T>(_defaultValue)))
				{}

				/*!
				 *	\brief Constructor. Takes the property's name.
				 *	If _serializer fails to find a corresponding property value, an exception is thrown.
				 *	Any additional parameters are passed onto the base property class
				 *	\tparam PROPERTY_SERIALIZER_T Class derived from PropertySerializerBase
				 *	\param _serializer Property Serializer
				 */
				template<PROPERTY_SERIALIZER_C PROPERTY_SERIALIZER_T>
				requires (property_write_default_v<PROPERTY> == true)
				PropertyTemplateInternal(PROPERTY_SERIALIZER_T &&_serializer)
				    : property_config_t(),
				      base_property_internal_t(std::forward<PROPERTY_SERIALIZER_T>(_serializer)),
				      Property(std::forward<PROPERTY_SERIALIZER_T>(_serializer).template deserializeSingleProperty<property_value_t>(Name))
				{}

				/*!
				 * \brief Constructor. Takes property value for current PROPERTIES class, passes rest on
				 * \tparam STRING_T Name class. Should be convertible to std::string_view
				 * \tparam PROPERTY_T Property Class. Should be convertible to PROPERTY
				 * \param _property Property value
				 * \param _other Remaining names and property values for REMAINING_PROPERTIES
				 */
				template<PROPERTY_CONSTRUCTIBLE_C<PROPERTY> PROPERTY_T, class ...T>
				PropertyTemplateInternal(PROPERTY_T &&_property, T &&..._other)
				    : property_config_t(std::forward<PROPERTY_T>(_property)),
				      base_property_internal_t(std::forward<T>(_other)...),
				      Property(std::forward<PROPERTY_T>(_property))
				{}

				PropertyTemplateInternal() = default;

			private:
				/*!
				 *	\brief Uses a PropertySerializer to extract the property stored under name. Should this throw an exception, the defaultValue is used instead
				 *	\tparam PROPERTY_SERIALIZER_T Serializer Type
				 *	\tparam PROPERTY_T Type convertible to PROPERTY
				 *	\param serializer Object used to extract property stored under name
				 *	\param defaultValue Property initialization value that should be used if serializer extraction fails
				 *	\return Returns initialization value of Property
				 */
				template<PROPERTY_SERIALIZER_C PROPERTY_SERIALIZER_T, class PROPERTY_T>
				static property_value_t getSerializerValOrDefault(PROPERTY_SERIALIZER_T &&serializer, PROPERTY_T &&defaultValue)
				{
					try
					{
						return std::forward<PROPERTY_SERIALIZER_T>(serializer).template deserializeSingleProperty<property_value_t>(Name);
					}
					catch(NRPExceptionMissingProperty &)
					{
						return std::forward<PROPERTY_T>(defaultValue);
					}
					catch(std::exception &e)
					{
						throw NRPException::logCreate(e, std::string("Failed to get property by name \"") + Name.data() + "\"");
					}
				}

//				/*!
//				 * \brief Tries to retrieve value stored under name. If that fails, use the default value
//				 * \tparam FUNC Function callback. Should be of the form PROPERTY(const std::string_view &)
//				 * \tparam PROPERTY_T Property class. Should be convertible to PROPERTY
//				 * \param func Function callback. Tries to get value stored under name
//				 * \param name Name of property
//				 * \param defaultValue Default property that is used if func call throws an exception
//				 * \return Returns Property
//				 */
//				template<class FUNC, class PROPERTY_T>
//				static property_value_t getFuncValOrDefault(FUNC &&func, PROPERTY_T &&defaultValue)
//				{
//					if constexpr (std::is_base_of_v<PropertyTemplateGeneral::PropertyConfigGeneral, PROPERTY>)
//					{
//						static_assert(std::is_constructible_v<property_value_t, PROPERTY_T>, "Invalid value given as default");
//						property_value_t retProp(defaultValue);
//						try
//						{
//							retProp.Value = std::forward<FUNC>(func)(Name);
//						}
//						catch(std::exception &)
//						{
//							retProp.Value = retProp.defaultValue;
//						}

//						return retProp;
//					}
//					else
//					{
//						try
//						{
//							return std::forward<FUNC>(func)(Name);
//						}
//						catch(std::exception &)
//						{
//							return std::forward<PROPERTY_T>(defaultValue);
//						}
//					}
//				}
		};
};

template<class T>
concept PROPERTY_TEMPLATE_C = std::is_base_of_v<PropertyTemplateGeneral, std::decay_t<T> >;

template<class PROPERTY, bool WRITE_DEFAULT = false>
using PropCfg = PropertyTemplateGeneral::PropertyConfig<PROPERTY, WRITE_DEFAULT>;

/*!
 *	\brief Struct containing a series of property variables
 *	\tparam CLASS Final class derived from this PropertyTemplate
 *	\tparam PROP_NAMES Template class of type PropNames<...>, containing all property names
 *	\tparam PROPERTIES List of saved property variable classes
 */
template<class CLASS, PROP_NAMES_C PROP_NAMES = PropNames<>, class ...PROPERTIES>
class PropertyTemplate
        : private PropertyTemplateGeneral,
          public PROP_NAMES,
          private PropertyTemplateGeneral::PropertyTemplateInternal<CLASS, 0, PROP_NAMES, PROPERTIES...>
{
		static_assert(PROP_NAMES::NumNames() == sizeof...(PROPERTIES), "Number of names does not match number of property types");

		template<int ID, int CUR_ID, class ...REMAINING_PROPERTIES>
		struct property_template_internal_type
		{	using  type = PropertyTemplateInternal<CLASS, 0, PROP_NAMES, PROPERTIES...>;	};

		template<int ID, int CUR_ID, class PROPERTY, class ...REMAINING_PROPERTIES>
		struct property_template_internal_type<ID, CUR_ID, PROPERTY, REMAINING_PROPERTIES...>
		{
			using type = std::conditional_t<CUR_ID == ID, PropertyTemplateGeneral::PropertyTemplateInternal<CLASS, CUR_ID, PROP_NAMES, PROPERTY, REMAINING_PROPERTIES...>,
				                                          typename property_template_internal_type<ID, CUR_ID+1, REMAINING_PROPERTIES...>::type>;
		};

		template<int ID_T>
		using property_internal_t = typename property_template_internal_type<ID_T, 0, PROPERTIES...>::type;

	public:
		static constexpr auto NumProperties = sizeof...(PROPERTIES);

		using property_template_t = PropertyTemplate<CLASS, PROP_NAMES, PROPERTIES...>;

		using class_t = CLASS;

		template<int ID>
		using property_t = typename property_internal_t<ID>::property_value_t;

		template<FixedString NAME>
		using property_name_t = typename property_internal_t<PROP_NAMES::template getID<NAME>()>::property_value_t;

		/*!
		 * \brief Constructor. Takes name and property value for each PROPERTIES class
		 * \tparam STRINGS_PROPERTIES_T String and Property Class. Should be a set of classes, convertible to string_view and PROPERTY
		 * \param properties Set of names and default values for REMAINING_PROPERTIES
		 */
		template<class ...STRINGS_PROPERTIES_T>
		PropertyTemplate(STRINGS_PROPERTIES_T &&...properties)
		    : property_internal_t<0>(std::forward<STRINGS_PROPERTIES_T>(properties)...)
		{}

		PropertyTemplate() = default;

		/*!
		 *	\brief Gets Property with ID _ID
		 *	\tparam _ID ID to retrieve
		 *	\tparam PROPERTY_T Class of property to retrieve
		 *	\return Returns Property (converted to const PROPERTY_T&)
		 */
		template<int ID_T, class PROPERTY_T = property_t<ID_T> >
		constexpr const PROPERTY_T &getProperty() const
		{
			static_assert(ID_T < sizeof...(PROPERTIES), "ID not in PropertyTemplate");
			return this->property_internal_t<0>::template getProperty<ID_T, PROPERTY_T>();
		}

		/*!
		 *	\brief Gets Property with ID _ID
		 *	\tparam _ID ID to retrieve
		 *	\tparam PROPERTY_T Class of property to retrieve
		 *	\return Returns Property (converted to PROPERTY_T&)
		 */
		template<int ID_T, class PROPERTY_T = property_t<ID_T>>
		constexpr PROPERTY_T &getProperty()
		{
			static_assert(ID_T < sizeof...(PROPERTIES), "ID not in PropertyTemplate");
			return this->property_internal_t<0>::template getProperty<ID_T, PROPERTY_T>();
		}

		/*!
		 *	\brief Gets Property with ID _ID
		 *	\tparam ID_T ID to retrieve
		 *	\return Returns Name
		 */
		template<int ID_T>
		static constexpr const std::string_view getName()
		{
			static_assert(ID_T < sizeof...(PROPERTIES), "ID not in PropertyTemplate");
			return property_internal_t<0>::template getName<ID_T>();
		}


		/*!
		 *	\brief Returns Property stored under _name
		 *	\tparam PROPERTY_T Class to retrieve
		 *	\param _name Name of property
		 *	\return Returns Property (converted to const PROPERTY_T&)
		 */
		template<FixedString NAME, class PROPERTY_T = property_name_t<NAME> >
		constexpr const PROPERTY_T &getPropertyByName() const
		{
			static_assert(sizeof...(PROPERTIES) > 0, "Could not find property with requested name");
			return this->property_internal_t<0>::template getPropertyByName<NAME, PROPERTY_T>();
		}

		/*!
		 *	\brief Returns Property stored under _name
		 *	\tparam PROPERTY_T Class to retrieve
		 *	\param _name Name of property
		 *	\return Returns Property (converted to PROPERTY_T&)
		 */
		template<FixedString NAME, class PROPERTY_T = property_name_t<NAME> >
		constexpr PROPERTY_T &getPropertyByName()
		{
			static_assert(sizeof...(PROPERTIES) > 0, "Could not find property with requested name");
			return this->property_internal_t<0>::template getPropertyByName<NAME, PROPERTY_T>();
		}

		/*!
		 *	\brief Check whether a property's default value will be serialized
		 *	\tparam ID_T ID of property
		 */
		template<int ID_T>
		static constexpr bool isDefaultWritable()
		{	return property_internal_t<ID_T>::property_config_t::WriteDefault;	}

		/*!
		 *	\brief Check whether a property's default value will be serialized
		 *	\tparam NAME Property Name
		 */
		template<FixedString NAME, int ID_T = PROP_NAMES::template getID<NAME>()>
		static constexpr bool isDefaultWritable()
		{
			static_assert (ID_T < PROP_NAMES::NumNames(), "No property with given name stored");
			return isDefaultWritable<PROP_NAMES::template getID<NAME>() >();
		}

		template<int ID_T>
		const property_t<ID_T> &getDefaultValue() const
		{
			static_assert(isDefaultWritable<ID_T>() == false, "Property does not contain a default value");
			return this->property_internal_t<ID_T>::property_config_t::DefaultValue;
		}

		template<FixedString NAME, int ID_T = PROP_NAMES::template getID<NAME>()>
		const property_t<ID_T> &getDefaultValue() const
		{
			static_assert (ID_T < PROP_NAMES::NumNames(), "No property with given name stored");
			return this->getDefaultValue<ID_T>();
		}
};

#endif // PROPERTY_TEMPLATE_H
