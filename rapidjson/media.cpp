#include "rapidjson/media.hpp"
#include <fstream>
#include <sstream>
#include <stdexcept>

namespace rapidjson_test {
using namespace rapidjson;

Document
get_document(std::string json_str) {
    Document d;
    //std::cout << "[get_document] preparing to parse json_str " << std::endl;
    if (d.Parse(json_str.c_str()).HasParseError())  {
        throw std::runtime_error("Can't parse json string");
    }

    //std::cout << "[parse_string] value accepted writer " <<  std::endl;

    return d;
}

Document
parse_string(const Value& value) {
    //std::cout << "value entered" << std::endl;
	rapidjson::StringBuffer buffer;
	rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
	value.Accept(writer);
    //std::cout << "[parse_string] value accepted writer" << std::endl;
    //std::cout << "[parse_string] buffer string: " << buffer.GetString() << std::endl;
	rapidjson::Document result;
	rapidjson::StringStream s(buffer.GetString());
	result.ParseStream(s);
	//std::cout << "[parse_string] parsed node into document" << std::endl;
	return result;
}

Document
get_nested(Document &d, std::string key){
    //std::cout << "[get_nested] got key " << key << std::endl;
	const char *key_ctr = key.c_str();
	assert(d[key_ctr].IsObject());
	//std::cout << "[get_nested] getting nested document " << key_ctr << std::endl;

	return parse_string(d[key_ctr]);
}

MediaContent
get_test_data(std::string testfile)
{
    using namespace rapidjson;
    MediaContent data;

    //std::cout << "[get_test_data] files coming in? " << testfile << std::endl;

    std::ifstream ifs(testfile.c_str());
    std::stringstream buffer;
    buffer << ifs.rdbuf();

    //std::cout << "[get_test_data] file read?" << std::endl;

    Document document = get_document(buffer.str());
    //std::cout << "[get_test_data] Document created" << std::endl;
    data.Deserialize(document);

    return data;
}

} // namespace
