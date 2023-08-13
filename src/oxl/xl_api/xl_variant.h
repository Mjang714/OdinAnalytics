#ifndef XLVARIANT_H_
#define XLVARIANT_H_

#include <variant>
#include <string>


namespace oxl::xl_api
{
	
	using XlVariant = std::variant<double, std::string, bool>;
}
#endif //XLVARIANT_H_