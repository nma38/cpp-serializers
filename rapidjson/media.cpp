#include "rapidjson/media.hpp"
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <queue>

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

thrift_test::MediaContent get_thrift_test_data(std::string testfile)
{
    rapidjson_test::MediaContent data = get_test_data(testfile);
    thrift_test::MediaContent thriftMediaContent;

    // convert images
    for (std::vector<rapidjson_test::Image>::iterator imgIt = data.images.begin() ; imgIt != data.images.end(); ++imgIt) {
        rapidjson_test::Image input = *imgIt;

        thrift_test::Image image;
        image.title = input.title;
        image.uri = input.uri;
        image.height = input.height;
        image.width = input.width;
        switch(input.size) {
            case rapidjson_test::LARGE: image.size = thrift_test::Size::LARGE; break;
            case rapidjson_test::SMALL: image.size = thrift_test::Size::SMALL; break;
        }

        thriftMediaContent.image.push_back(image);
    }

    // convert media
    rapidjson_test::Media media = data.media;
    thriftMediaContent.media.bitrate = media.bitrate;
    thriftMediaContent.media.title = media.title;
    thriftMediaContent.media.uri = media.uri;
    thriftMediaContent.media.width = media.width;
    thriftMediaContent.media.height = media.height;
    thriftMediaContent.media.duration = media.duration;
    thriftMediaContent.media.format = media.format;
    thriftMediaContent.media.copyright = media.copyright;
    switch(media.player) {
        case rapidjson_test::JAVA: thriftMediaContent.media.player = thrift_test::Player::JAVA; break;
        case rapidjson_test::FLASH: thriftMediaContent.media.player = thrift_test::Player::FLASH; break;
    }
    thriftMediaContent.media.person = media.persons;

    // convert media.pods
    for (std::vector<rapidjson_test::Pod*>::iterator podIt = media.pods.begin() ; podIt != media.pods.end(); ++podIt) {
        rapidjson_test::Pod* input = *podIt;

        std::queue<thrift_test::Pod> podqueue;
        thrift_test::Pod rootPod;
        rootPod.message = input->message;

        input = input->pod;
        while (input != 0) {
            thrift_test::Pod pod;
            pod.message = input->message;
            podqueue.push(pod);
            input = input->pod;
        }

        // convert pod
        thrift_test::Pod curPod = rootPod;
        while (!podqueue.empty()) {
            thrift_test::Pod nested = podqueue.front();
            curPod.pod.push_back(nested);
            curPod = nested;
            podqueue.pop();
        }

        thriftMediaContent.media.pods.push_back(rootPod);
    }

    return thriftMediaContent;
}

protobuf_test::MediaContent get_protobuf_test_data(std::string testfile)
{
    rapidjson_test::MediaContent data = get_test_data(testfile);
    protobuf_test::MediaContent protobufMediaContent;

    // convert images
    for (std::vector<rapidjson_test::Image>::iterator imgIt = data.images.begin() ; imgIt != data.images.end(); ++imgIt) {
        rapidjson_test::Image input = *imgIt;

        protobuf_test::Image image;
        image.title = input.title;
        image.uri = input.uri;
        image.height = input.height;
        image.width = input.width;
        switch(input.size) {
            case rapidjson_test::LARGE: image.size = protobuf_test::Size::LARGE; break;
            case rapidjson_test::SMALL: image.size = protobuf_test::Size::SMALL; break;
        }

        protobufMediaContent.image.push_back(image);
    }

    // convert media
    rapidjson_test::Media media = data.media;
    protobufMediaContent.media.bitrate = media.bitrate;
    protobufMediaContent.media.title = media.title;
    protobufMediaContent.media.uri = media.uri;
    protobufMediaContent.media.width = media.width;
    protobufMediaContent.media.height = media.height;
    protobufMediaContent.media.duration = media.duration;
    protobufMediaContent.media.format = media.format;
    protobufMediaContent.media.copyright = media.copyright;
    switch(media.player) {
        case rapidjson_test::JAVA: protobufMediaContent.media.player = protobuf_test::Player::JAVA; break;
        case rapidjson_test::FLASH: protobufMediaContent.media.player = protobuf_test::Player::FLASH; break;
    }
    thriftMediaContent.media.person = media.persons;

    // convert media.pods
    for (std::vector<rapidjson_test::Pod*>::iterator podIt = media.pods.begin() ; podIt != media.pods.end(); ++podIt) {
        rapidjson_test::Pod* input = *podIt;

        std::queue<protobuf_test::Pod> podqueue;
        thrift_test::Pod rootPod;
        rootPod.message = input->message;

        input = input->pod;
        while (input != 0) {
            protobuf_test::Pod pod;
            pod.message = input->message;
            podqueue.push(pod);
            input = input->pod;
        }

        // convert pod
        protobuf_test::Pod curPod = rootPod;
        while (!podqueue.empty()) {
            protobuf_test::Pod nested = podqueue.front();
            curPod.pod.push_back(nested);
            curPod = nested;
            podqueue.pop();
        }

        protobufMediaContent.media.pods.push_back(rootPod);
    }

    return protobufMediaContent;
}

} // namespace
