#ifndef NRP_CONCEPTS_H
#define NRP_CONCEPTS_H

#include <concepts>

template<class T, class U>
concept SAME_AS_C = requires()
{	std::same_as<std::remove_cvref_t<T>, U>;	};

#endif // NRP_CONCEPTS_H
