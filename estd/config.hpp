#ifndef ESTD_CONFIG_HPP
#define ESTD_CONFIG_HPP

#include <string>
#include <vector>

namespace estd
{

struct iConfig
{
    virtual ~iConfig (void) = default;

    virtual std::vector<std::string> get_names (void) const = 0;

    virtual void* get_obj (std::string cfg_name) = 0;
};

}

#endif // ESTD_CONFIG_HPP
