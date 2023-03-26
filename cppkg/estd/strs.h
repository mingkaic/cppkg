
#ifndef CPPKG_ESTD_STR_H
#define CPPKG_ESTD_STR_H

#include <string>

namespace estd
{

bool has_prefix (const std::string& str, const std::string& prefix);

bool has_affix (const std::string& str, const std::string& affix);

}

#endif // CPPKG_ESTD_STR_H
