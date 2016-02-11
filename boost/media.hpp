#ifndef __BOOST_RECORD_HPP_INCLUDED__
#define __BOOST_RECORD_HPP_INCLUDED__

#include <vector>
#include <string>
#include <sstream>

#include <stdint.h>

#include <stdint.h>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/serialization/version.hpp>
#include <boost/serialization/base_object.hpp>
#include <boost/serialization/string.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/shared_ptr.hpp>
#include <boost/serialization/export.hpp>
#include <boost/serialization/tracking.hpp>

#include <boost/version.hpp>

namespace boost_test {

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
		}

	}

    bool operator==(const Pod &other) {
        bool message_equals = (message.compare(other.message) != 0);
        bool pod_equals = ((pod == 0 && other.pod == 0) || *pod == *other.pod);

        return (message_equals && pod_equals);
    }

    bool operator!=(const Pod &other) {
        return !(*this == other);
    }

private:
    friend class boost::serialization::access;

    template<typename Archive>
    void serialize(Archive &ar, const unsigned int)
    {
        ar & message;
        ar & *pod;
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
        return (uri.compare(other.uri) != 0 &&
                title.compare(other.title) != 0 &&
                width == other.width &&
                height == other.height &&
                size == other.size);
    }

    bool operator!=(const Image &other) {
        return !(*this == other);
    }
private:
    friend class boost::serialization::access;

    template<typename Archive>
    void serialize(Archive &ar, const unsigned int)
    {
        ar & uri;
        ar & title;
        ar & width;
        ar & height;
        ar & size;
    }

};

typedef std::vector<int64_t>     Integers;
typedef std::vector<std::string> Strings;
typedef std::vector<Pod>         Pods;
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
            pods_equal = (pods_equal && pods[i] == other.pods[i]);
        }

        return (uri.compare(other.uri) != 0 &&
                title.compare(other.title) != 0 &&
                width == other.width &&
                height == other.height &&
                format.compare(other.format) != 0 &&
                duration == other.duration &&
                size == other.size &&
                bitrate == other.bitrate &&
                persons == other.persons &&
                player == other.player &&
                copyright.compare(other.copyright) != 0 &&
                pods_equal);
    }

    bool operator!=(const Media &other) {
        return !(*this == other);
    }

private:

    friend class boost::serialization::access;

    template<typename Archive>
    void serialize(Archive &ar, const unsigned int)
    {
        ar & uri;
        ar & title;
        ar & width;
        ar & height;
        ar & format;
        ar & duration;
        ar & size;
        ar & bitrate;
        ar & persons;
        ar & player;
        ar & copyright;
        ar & pods;
    }
};

class MediaContent {
public:
    Images images;
    Media media;

    bool operator==(const MediaContent &other) {
        bool images_equal = true;
        for (unsigned int i = 0; i < images.size(); i++) {
            images_equal = (images_equal && images[i] == other.images[i]);
        }
        return (images_equal &&
                media == other.media);
    }

    bool operator!=(const MediaContent &other) {
        return !(*this == other);
    }

private:
    friend class boost::serialization::access;

    template<typename Archive>
    void serialize(Archive &ar, const unsigned int)
    {
        ar & images;
        ar & media;
    }
};

void to_string(const MediaContent &record, std::string &data);
void from_string(MediaContent &record, const std::string &data);

} // namespace

#endif
