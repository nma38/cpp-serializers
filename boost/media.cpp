#include "boost/media.hpp"

namespace boost_test {

void
to_string(const MediaContent &record, std::string &data)
{
    std::ostringstream stream;
    boost::archive::binary_oarchive archiver(stream);
    archiver << record;

    data = stream.str();
}

void
from_string(MediaContent &record, const std::string &data)
{
    std::stringstream stream(data);
    boost::archive::binary_iarchive archiver(stream);
    archiver >> record;
}

} // namespace
