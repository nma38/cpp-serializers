#ifndef __RAPIDJSON_RECORD_HPP_INCLUDED__
#define __RAPIDJSON_RECORD_HPP_INCLUDED__

#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include <iostream>
#include <cstdio>
#include <string>
#include <vector>

namespace rapidjson_test {
using namespace rapidjson;

enum Size { SMALL=1, LARGE };
enum Player { JAVA=1, FLASH }; 

Document parse_string(const Value& value) {
	rapidjson::StringBuffer buffer;
	rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
	value.Accept(writer);
	
	rapidjson::Document result;
	rapidjson::StringStream s(buffer.GetString());
	result.ParseStream(s);
	return result;
}

Document get_nested(Document &d, std::string key){
	const char *key_ctr = key.c_str();
	assert(d[key_ctr].IsObject());
	return parse_string(d[key_ctr])
}

class Pod {
public:
    Pod* pod;
    std::string message;

	~Pod() {
		if (pod != 0) {
			pod->~Pod();
		}
		delete pod;		
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
            pod->serialize(writer);
        else
            writer.Null(); 


        writer.EndObject();
    }

	void Deserialize(Document& document) const {
		assert(document.isObject());
		assert(document.HasMember("message"));
		assert(document.HasMember("pod"));

		message = document["message"].GetString();
		if (!document["pod"].IsNull()) {		
			pod->pod = new Pod();
			pod->pod.Deserialize(document["pod"]);
		} else {
			pod = 0;
		}
	}	 
};

class Image {
public:
    std::string uri;
    std::string title;
    int width;
    int height;
    Size size;
    
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
#if RAPIDJSON_HAS_STDSTRING
        writer.String(title);
#else
        writer.String(title.c_str(), static_cast<SizeType>(title.length()));
#endif

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
	
    void Deserialize(Document& document) const {
		assert(document.isObject());
		assert(document.HasMember("uri"));
		assert(document.HasMember("title"));
		assert(document.HasMember("width"));
		assert(document.HasMember("length"));
		assert(document.HasMember("size"));

		uri = document["uri"].GetString();
		title = document["title"].GetString();
        width = document["width"].GetInt();
        height = document["height"].GetInt();
        
        std::string stringer = document["size"].GetString();
        if (stringer.compare("SMALL") == 0) {
            size = Size.SMALL;
        } else if (stinger.compare("LARGE") == 0) {
            size = Size.LARGE;
        }
	}	 
};

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
#if RAPIDJSON_HAS_STDSTRING
        writer.String(title);
#else
        writer.String(title.c_str(), static_cast<SizeType>(title.length()));
#endif
		
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
			writer.String(persons[i]);
		}
		writer.EndArray();	

		writer.String("player");
		switch(size)
		{
			case FLASH: writer.String("FLASH"); break;
			case JAVA: writer.String("JAVA"); break;
		}
		
        writer.String("copyright");
#if RAPIDJSON_HAS_STDSTRING
        writer.String(copyright);
#else
        writer.String(copyright.c_str(), static_cast<SizeType>(copyright.length()));
#endif
		
		writer.String("pods");
		writer.StartArray();
		for (std::vector<Pod>::const_iterator podItr = pods.begin(); podItr != pods.end(); ++podIter)
			podIter->Serialize(writer);
		writer.EndArray();	
		
		writer.EndObject();
	}
    
    void Deserialize(Document& document) const {
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
        assert(document.HasMemver("copyright"));
		assert(document.HasMember("pods"));

		uri = document["uri"].GetString();
		title = document["title"].GetString();
        width = document["width"].GetInt();
        height = document["height"].GetInt();
        duration = document["duration"].GetInt64();
        format = document["format"].GetString();
        bitrate = document["bitrate"].GetInt();

      {
        const Value& p = document["persons"]; // Using a reference for consecutive access is handy and faster.
        assert(p.IsArray());
        
        // Iterating array with iterators
        for (Value::ConstValueIterator itr = p.Begin(); itr != p.End(); ++itr) 
       		persons.push_back(itr->GetString());
      }  

        std::string stringer = document["player"].GetString();
        if (stringer.compare("JAVA") == 0) {
            player = Player.JAVA;
        } else if (stinger.compare("FLASH") == 0) {
            player = Player.FLASH;
        }

        copyright = document["copyright"].GetString();

	   {
        const Value& p = document["pods"]; // Using a reference for consecutive access is handy and faster.
        for (Value::ConstValueIterator itr = p.Begin(); itr != p.End(); ++itr) {
		    Pod pod;	
			pod.Deserialize(parse_string(*itr));
       		pods.push_back(pod);
		 }
	   }
		
	}	 
};

class MediaContent {
public:
    Media media;
    Images images;
	
	template <typename Writer>
    void Serialize(Writer& writer) const {
        writer.StartObject();

		writer.String("media");
		media.Serialize(writer);

		writer.String("images");
		writer.StartArray();
		for (std::vector<Image>::const_iterator imageItr = images.begin(); imageItr != images.end(); ++imageIter)
			imageIter->Serialize(writer);
		writer.EndArray();	
		
        writer.EndObject();
	}
    
	void Deserialize(Document& document) const {
		assert(document.isObject());
		assert(document.HasMember("media"));
		assert(document.HasMember("images"));
		assert(document["images"].IsArray());

		media.Deserialize(get_nested(document, "media")); 	
	   {
        const Value& imgs = document["images"]; // Using a reference for consecutive access is handy and faster.
        for (Value::ConstValueIterator itr = imgs.Begin(); itr != imgs.End(); ++itr) {
		    Image image;	
			image.Deserialize(parse_string(*itr));
       		images.push_back(image);
		 }
	   }
	}
};
    
} // namespace
#endif
