#ifndef PTR_TEMPLATES_H
#define PTR_TEMPLATES_H

#include <memory>

template<class T>
class PtrTemplates
{
	public:
		using shared_ptr = std::shared_ptr<T>;
		using const_shared_ptr = std::shared_ptr<const T>;

		using unique_ptr = std::unique_ptr<T>;
		using const_unique_ptr = std::unique_ptr<const T>;

		constexpr auto operator<=>(const PtrTemplates<T>&) const = default;
};

#endif
