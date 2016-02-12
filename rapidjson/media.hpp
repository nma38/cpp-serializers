#ifndef __RAPIDJSON_RECORD_HPP_INCLUDED__
#define __RAPIDJSON_RECORD_HPP_INCLUDED__

#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include <assert.h>
#include <iostream>
#include <cstdio>
#include <string>
#include <vector>

#include "thrift/gen-cpp/media_types.h"
#include "thrift/gen-cpp/media_constants.h"

namespace rapidjson_test {
using namespace rapidjson;

Document get_document(std::string);
Document parse_string(const Value& value);
Document get_nested(Document &d, std::string key);

enum Size { SMALL=1, LARGE };
enum Player { JAVA=1, FLASH };

class Pod {
public:
    Pod* pod;
    std::string message;

	~Pod() {
		if (pod != 0) {
			pod->~Pod();
            delete pod;
            pod = 0;
		}
	}

    bool operator==(const Pod &other) {
        bool message_equals = (message.compare(other.message) == 0);
        bool pod_equals = ((pod == 0 && other.pod == 0) || *pod == *other.pod);

        //std::cout << "[Pod] equal ? " << ((message_equals && pod_equals)? "true":"false") << std::endl;

        return (message_equals && pod_equals);
    }

    bool operator!=(const Pod &other) {
        return !(*this == other);
    }

    template <typename Writer>
    void Serialize(Writer& writer) const {
        writer.StartObject();

        writer.String("message");
#if RAPIDJSON_HAS_STDSTRING
        writer.String(message);
#else
        writer.String(message.c_str(), static_cast<SizeType>(message.length()));
#endif

		writer.String("pod");
        if (pod)
            pod->Serialize(writer);
        else
            writer.Null();

        writer.EndObject();
    }

	void Deserialize(Document& document) {
		assert(document.isObject());
		assert(document.HasMember("message"));
		assert(document.HasMember("pod"));

		message = document["message"].GetString();
		//std::cout << "[pod] " << "deserailize primitives" << std::endl;
		//std::cout << "[pod] inner pod null? " << document["pod"].IsNull() << std::endl;
		if (!document["pod"].IsNull()) {
			pod = new Pod();
            Document d = get_nested(document, "pod");
            //std::cout << "[pod] " << "deserailize nested pod" << std::endl;
			pod->Deserialize(d);
			//std::cout << "[pod] " << "after deserailize nested pod" << std::endl;
		} else {
			pod = 0;
		}
		//std::cout << "[pod] " << "deserailize pod" << std::endl;
	}
};

class Image {
public:
    std::string uri;
    std::string title;
    int width;
    int height;
    Size size;
    bool operator==(const Image &other) {
        bool image_equals =  (uri.compare(other.uri) == 0 &&
             title.compare(other.title) == 0 &&
             width == other.width &&
             height == other.height &&
             size == other.size);
        //std::cout << "[Image] equal ? " << (image_equals? "true":"false") << std::endl;
        return image_equals;
    }

    bool operator!=(const Image &other) {
        return !(*this == other);
    }

	template <typename Writer>
    void Serialize(Writer& writer) const {
        writer.StartObject();

        writer.String("uri");
#if RAPIDJSON_HAS_STDSTRING
        writer.String(uri);
#else
        writer.String(uri.c_str(), static_cast<SizeType>(uri.length()));
#endif

		writer.String("title");
		if (!title.empty()) {
#if RAPIDJSON_HAS_STDSTRING
        writer.String(title);
#else
        writer.String(title.c_str(), static_cast<SizeType>(title.length()));
#endif
       } else {
        writer.Null();
       }

		writer.String("width");
		writer.Int(width);

		writer.String("height");
		writer.Int(height);

		writer.String("size");
		switch(size)
		{
			case LARGE: writer.String("LARGE"); break;
			case SMALL: writer.String("SMALL"); break;
		}

        writer.EndObject();
    }

    void Deserialize(Document& document) {
		assert(document.isObject());
		assert(document.HasMember("uri"));
		assert(document.HasMember("title"));
		assert(document.HasMember("width"));
		assert(document.HasMember("length"));
		assert(document.HasMember("size"));

		uri = document["uri"].GetString();
        if (!document["title"].IsNull()) {
            title = document["title"].GetString();
		}
        width = document["width"].GetInt();
        height = document["height"].GetInt();

        std::string stringer = document["size"].GetString();
        if (stringer.compare("SMALL") == 0) {
            size = SMALL;
        } else if (stringer.compare("LARGE") == 0) {
            size = LARGE;
        }
	}
};

typedef std::vector<std::string> Strings;
typedef std::vector<Pod*>         Pods;
typedef std::vector<Image>       Images;

class Media
{
public:

    std::string uri;
    std::string title;
    int width;
    int height;
    std::string format;
    long duration;
    long size;
    int bitrate;
    Strings persons;
    Player player;
    std::string copyright;
    Pods pods;

    bool operator==(const Media &other) {

        bool pods_equal = true;
        for (unsigned int i = 0; i < pods.size(); i++) {
            pods_equal = (pods_equal && ((pods[i] == 0 && other.pods[i] == 0) || *pods[i] == *other.pods[i]));
        }

        bool media_equals = (uri.compare(other.uri) == 0 &&
                title.compare(other.title) == 0 &&
                width == other.width &&
                height == other.height &&
                format.compare(other.format) == 0 &&
                duration == other.duration &&
                size == other.size &&
                bitrate == other.bitrate &&
                persons == other.persons &&
                player == other.player &&
                copyright.compare(other.copyright) == 0 &&
                pods_equal);
        //std::cout << "[Media] equal ? " << (media_equals? "true":"false") << std::endl;
        return media_equals;
    }

    bool operator!=(const Media &other) {
        return !(*this == other);
    }

	template <typename Writer>
    void Serialize(Writer& writer) const {
        writer.StartObject();

        writer.String("uri");
#if RAPIDJSON_HAS_STDSTRING
        writer.String(uri);
#else
        writer.String(uri.c_str(), static_cast<SizeType>(uri.length()));
#endif

		writer.String("title");
		if (!title.empty()) {
#if RAPIDJSON_HAS_STDSTRING
            writer.String(title);
#else
            writer.String(title.c_str(), static_cast<SizeType>(title.length()));
#endif
        } else {
            writer.Null();
        }

		writer.String("width");
		writer.Int(width);

		writer.String("height");
		writer.Int(height);

		writer.String("format");
#if RAPIDJSON_HAS_STDSTRING
        writer.String(format);
#else
        writer.String(format.c_str(), static_cast<SizeType>(format.length()));
#endif

		writer.String("duration");
		writer.Int64(duration);

		writer.String("size");
		writer.Int64(size);

		writer.String("bitrate");
		writer.Int(bitrate);

		writer.String("persons");
		writer.StartArray();
		for (int i = 0; i < persons.size(); i++) {
			writer.String(persons[i].c_str());
		}
		writer.EndArray();

		writer.String("player");
		switch(player)
		{
			case FLASH: writer.String("FLASH"); break;
			case JAVA: writer.String("JAVA"); break;
		}

        writer.String("copyright");
        if (!copyright.empty()) {
#if RAPIDJSON_HAS_STDSTRING
        writer.String(copyright);
#else
        writer.String(copyright.c_str(), static_cast<SizeType>(copyright.length()));
#endif
        } else {
        writer.Null();
        }


		writer.String("pods");
		writer.StartArray();
		for (std::vector<Pod*>::const_iterator podItr = pods.begin(); podItr != pods.end(); ++podItr) {
			Pod* pod = *podItr;
			pod->Serialize(writer);
        }
		writer.EndArray();

		writer.EndObject();
	}

    void Deserialize(Document& document) {
		assert(document.isObject());
		assert(document.HasMember("uri"));
		assert(document.HasMember("title"));
		assert(document.HasMember("width"));
		assert(document.HasMember("height"));
		assert(document.HasMember("duration"));
        assert(document.HasMember("format"));
		assert(document.HasMember("bitrate"));
		assert(document.HasMember("persons"));
		assert(document.HasMember("player"));
		assert(document.HasMember("size"));
        assert(document.HasMemver("copyright"));
		assert(document.HasMember("pods"));

        //std::cout << "[media] " << "deserailize uri" << std::endl;
		uri = document["uri"].GetString();
		if (!document["title"].IsNull()) {
            title = document["title"].GetString();
		}
        width = document["width"].GetInt();
        height = document["height"].GetInt();
        duration = document["duration"].GetInt64();
        size = document["size"].GetInt64();
        format = document["format"].GetString();
        bitrate = document["bitrate"].GetInt();
        if (!document["copyright"].IsNull()) {
            copyright = document["copyright"].GetString();
        }

        //std::cout << "[media] " << "deserailize primitives" << std::endl;
      {
        const Value& p = document["persons"]; // Using a reference for consecutive access is handy and faster.
        assert(p.IsArray());

        // Iterating array with iterators
        for (Value::ConstValueIterator itr = p.Begin(); itr != p.End(); ++itr)
       		persons.push_back(itr->GetString());
      }
       //std::cout << "[media] " << "deserailize persons" << std::endl;

        std::string stringer = document["player"].GetString();
        if (stringer.compare("JAVA") == 0) {
            player = JAVA;
        } else if (stringer.compare("FLASH") == 0) {
            player = FLASH;
        }
         //std::cout << "[media] " << "deserailize player" << std::endl;

        {
        const Value& p = document["pods"]; // Using a reference for consecutive access is handy and faster.
        for (Value::ConstValueIterator itr = p.Begin(); itr != p.End(); ++itr) {
		    Document d = parse_string(*itr);
		    Pod* pod = new Pod();
			pod->Deserialize(d);
       		pods.push_back(pod);
       		//std::cout << "[media] " << "add a pod into pods" << std::endl;
		 }
       }
	   //std::cout << "[media] " << "deserailize pods" << std::endl;

	}
};

class MediaContent {
public:
    Media media;
    Images images;

    bool operator==(const MediaContent &other) {
        bool images_equal = true;
        for (unsigned int i = 0; i < images.size(); i++) {
            images_equal = (images_equal && (images[i] == other.images[i]));
        }
        return (images_equal &&
                media == other.media);
    }

    bool operator!=(const MediaContent &other) {
        return !(*this == other);
    }

	template <typename Writer>
    void Serialize(Writer& writer) const {
        writer.StartObject();

		writer.String("media");
		media.Serialize(writer);

		writer.String("images");
		writer.StartArray();
		for (std::vector<Image>::const_iterator imageItr = images.begin(); imageItr != images.end(); ++imageItr)
			imageItr->Serialize(writer);
		writer.EndArray();

        writer.EndObject();
	}

	void Deserialize(Document& document) {
		assert(document.isObject());
		assert(document.HasMember("media"));
		assert(document.HasMember("images"));
		assert(document["images"].IsArray());

		//std::cout << "[MediaContent] attempting to deserialize" << std::endl;
	   {
        const Value& imgs = document["images"]; // Using a reference for consecutive access is handy and faster.
        for (Value::ConstValueIterator itr = imgs.Begin(); itr != imgs.End(); ++itr) {
            Document d = parse_string(*itr);
		    Image image;
			image.Deserialize(d);
       		images.push_back(image);
		 }
	   }
	    //std::cout<< "[MediaContent] images all pushed in; count:" <<  images.size() << std::endl;

        Document d = get_nested(document, "media");
        //std::cout << "[MediaContent] got nested document" << std::endl;
		media.Deserialize(d);
        //std::cout << "[MediaContent] media deserialized" << std::endl;
	}
};

MediaContent get_test_data(std::string testfile);
thrift_test::MediaContent get_thrift_test_data(std::string testfile);

} // namespace
#endif
