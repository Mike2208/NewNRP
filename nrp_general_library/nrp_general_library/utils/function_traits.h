#ifndef FUNCTION_TRAITS_H
#define FUNCTION_TRAITS_H

#include <functional>

/*!
 *  \brief Struct to derive function input parameter types.
 *  \tparam T Function
 *
 *  From https://stackoverflow.com/questions/9065081/how-do-i-get-the-argument-types-of-a-function-pointer-in-a-variadic-template-cla
 */
template<typename T>
struct function_traits;

template<typename R, typename ...Args>
struct function_traits<std::function<R(Args...)> >
{
	static const size_t nargs = sizeof...(Args);

	using result_t = R;

	template<size_t i>
	using arg_t = typename std::tuple_element<i, std::tuple<Args...>>::type;
};

#endif // FUNCTION_TRAITS
