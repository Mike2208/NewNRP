#ifndef WCHAR_T_CONVERTER_H
#define WCHAR_T_CONVERTER_H

#include <vector>
#include <string>

/*!
 * \brief Converts and stores an char*[] to wchar*[].
 * Used mainly to convert argv from char*[] to wchar*[] for Python Initialization
 */
class WCharTConverter
{
	public:
		WCharTConverter(int argc, const char *const *argv);

		wchar_t **getWCharTPointers();
		int getWCharSize() const;

	private:
		std::vector<std::wstring> _wCharTContainer;
		std::vector<wchar_t*> _wCharTPointer;
};

#endif // WCHAR_T_CONVERTER_H
