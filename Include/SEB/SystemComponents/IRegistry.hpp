#ifndef SEB_IREGISTRY_HPP
#define SEB_IREGISTRY_HPP

/*
 * This one will be a funny one because Linux by default does not have a Registry...
 * oh well, workarounds exist for that very exact matter
 */

#include <string>
#include <vector>

namespace SEB::System
{
    class IRegistry
    {
    public:
        virtual ~IRegistry() = default;

        virtual bool TryRead(const std::string& key, const std::string& name, std::string& value) = 0;
        virtual bool TryGetNames(const std::string& key, std::vector<std::string>& names) = 0;
        virtual bool TryGetSubKeys(const std::string& key, std::vector<std::string>& subKeys) = 0;
    };
}

#endif //SEB_IREGISTRY_HPP