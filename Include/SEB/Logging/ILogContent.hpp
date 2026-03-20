#ifndef SEB_ILOGCONTENT_HPP
#define SEB_ILOGCONTENT_HPP

namespace SEB::Logging {

    class ILogContent
    {
    public:
        virtual ~ILogContent() = default;
        virtual ILogContent* clone() const = 0;
    };

}

#endif //SEB_ILOGCONTENT_HPP