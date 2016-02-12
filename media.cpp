#include <string>
#include <set>
#include <iostream>
#include <stdexcept>
#include <memory>
#include <chrono>
#include <sstream>
#include <fstream>
#include <cstdio>

#include <boost/shared_ptr.hpp>
#include <boost/lexical_cast.hpp>

#include <thrift/transport/TBufferTransports.h>
#include <thrift/protocol/TBinaryProtocol.h>
#include <thrift/protocol/TCompactProtocol.h>

#include "thrift/gen-cpp/media_types.h"
#include "thrift/gen-cpp/media_constants.h"
#include "protobuf/media.pb.h"

/*#include <capnp/message.h>
#include <capnp/serialize.h>*/

/*#include "capnproto/media.capnp.h"*/
#include "boost/media.hpp"
#include "rapidjson/media.hpp"
#include "rapidjson/document.h"
#include "rapidjson/prettywriter.h"

/*void
boost_serialization_test(size_t iterations)
{
    using namespace boost_test;
    MediaContent r1, r2;

    rapidjson_test::MediaContent data = get_test_data("media.2.json");

    std::string serialized;
    to_string(r1, serialized);
    from_string(r2, serialized);

    if (r1 != r2) {
        throw std::logic_error("boost's case: deserialization failed");
    }

    std::cout << "boost: version = " << BOOST_VERSION << std::endl;
    std::cout << "boost: size = " << serialized.size() << " bytes" << std::endl;

    auto start = std::chrono::high_resolution_clock::now();
    for (size_t i = 0; i < iterations; i++) {
        serialized.clear();
        to_string(r1, serialized);
        from_string(r2, serialized);
    }
    auto finish = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(finish - start).count();

    std::cout << "boost: time = " << duration << " milliseconds" << std::endl << std::endl;
}*/

void
rapidjson_serialization_test(std::string testfile, size_t iterations)
{
    using namespace rapidjson_test;
    MediaContent r1, r2;
    //std::cout << "grabbing data file" << std::endl;
    r1 = get_test_data(testfile);

    //std::cout << "got data" << std::endl;
    StringBuffer buffer;
    PrettyWriter<StringBuffer> writer(buffer);
    r1.Serialize(writer);
    std::string serialized = buffer.GetString();
    //std::cout << "Serialized output to local string" << std::endl;
    //std::cout << serialized << std::endl;
    //std::cout << "converting string to document" << std::endl;

    Document d = get_document(serialized);
    //std::cout << "deserializing" << std::endl;
    r2.Deserialize(d);

    if (r1 != r2) {
        throw std::logic_error("rapidjson's case: deserialization failed");
    }

    std::cout << "rapidjson: size = " << serialized.size() << " bytes" << std::endl;

    auto start = std::chrono::high_resolution_clock::now();
    for (size_t i = 0; i < iterations; i++) {
        serialized.clear();

        rapidjson::StringBuffer buffer;
        rapidjson::Writer<StringBuffer> writer(buffer);
        r1.Serialize(writer);
        serialized = buffer.GetString();
        Document d = get_document(serialized);
        r2.Deserialize(d);
    }
    auto finish = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(finish - start).count();

    std::cout << "rapidjson: roundtrip time = " << duration << " milliseconds" << std::endl;

    auto start_deser = std::chrono::high_resolution_clock::now();
    for (size_t i = 0; i < iterations; i++) {
        Document d = get_document(serialized);
        r2.Deserialize(d);
    }
    auto finish_deser = std::chrono::high_resolution_clock::now();
    auto duration_deser = std::chrono::duration_cast<std::chrono::milliseconds>(finish_deser - start_deser).count();

    std::cout << "rapidjson: deserialize time = " << duration_deser << " milliseconds" << std::endl;

    auto start_ser = std::chrono::high_resolution_clock::now();
    for (size_t i = 0; i < iterations; i++) {
        serialized.clear();

        rapidjson::StringBuffer buffer;
        rapidjson::Writer<StringBuffer> writer(buffer);
        r1.Serialize(writer);
        std::string new_serialized = buffer.GetString();
    }
    auto finish_ser = std::chrono::high_resolution_clock::now();
    auto duration_ser = std::chrono::duration_cast<std::chrono::milliseconds>(finish_ser - start_ser).count();

    std::cout << "rapidjson: serialize time = " << duration_ser << " milliseconds" << std::endl;
    std::cout << "rapidjson: " << duration_ser << " " <<  duration_deser << " " << duration << std::endl << std::endl;
}

enum class ThriftSerializationProto {
    Binary,
    Compact
};

void
thrift_serialization_test(std::string testfile, size_t iterations, ThriftSerializationProto proto = ThriftSerializationProto::Binary)
{
    using apache::thrift::transport::TMemoryBuffer;
    using apache::thrift::protocol::TBinaryProtocol;
    using apache::thrift::protocol::TCompactProtocol;
    using apache::thrift::protocol::TBinaryProtocolT;
    using apache::thrift::protocol::TCompactProtocolT;

    using namespace thrift_test;

    boost::shared_ptr<TMemoryBuffer> buffer1(new TMemoryBuffer());
    boost::shared_ptr<TMemoryBuffer> buffer2(new TMemoryBuffer());

    TBinaryProtocolT<TMemoryBuffer> binary_protocol1(buffer1);
    TBinaryProtocolT<TMemoryBuffer> binary_protocol2(buffer2);

    TCompactProtocolT<TMemoryBuffer> compact_protocol1(buffer1);
    TCompactProtocolT<TMemoryBuffer> compact_protocol2(buffer2);

    MediaContent r1 = rapidjson_test::get_thrift_test_data(testfile);
    std::string serialized;

    if (proto == ThriftSerializationProto::Binary) {
        r1.write(&binary_protocol1);
    } else if (proto == ThriftSerializationProto::Compact) {
        r1.write(&compact_protocol1);
    }

    serialized = buffer1->getBufferAsString();

    //std::cout << serialized << std::endl;

    // check if we can deserialize back
    MediaContent r2;

    buffer2->resetBuffer((uint8_t*)serialized.data(), serialized.length());

    if (proto == ThriftSerializationProto::Binary) {
        r2.read(&binary_protocol2);
    } else if (proto == ThriftSerializationProto::Compact) {
        r2.read(&compact_protocol2);
    }

    if (r1 != r2) {
        throw std::logic_error("thrift's case: deserialization failed");
    }

    std::string tag;

    if (proto == ThriftSerializationProto::Binary) {
        tag = "thrift-binary:";
    } else if (proto == ThriftSerializationProto::Compact) {
        tag = "thrift-compact:";
    }

    std::cout << tag << " version = " << VERSION << std::endl;
    std::cout << tag << " size = " << serialized.size() << " bytes" << std::endl;

    auto start = std::chrono::high_resolution_clock::now();
    for (size_t i = 0; i < iterations; i++) {
        buffer1->resetBuffer();

        if (proto == ThriftSerializationProto::Binary) {
            r1.write(&binary_protocol1);
        } else if (proto == ThriftSerializationProto::Compact) {
            r1.write(&compact_protocol1);
        }

        serialized = buffer1->getBufferAsString();
        buffer2->resetBuffer((uint8_t*)serialized.data(), serialized.length());

        if (proto == ThriftSerializationProto::Binary) {
            r2.read(&binary_protocol2);
        } else if (proto == ThriftSerializationProto::Compact) {
            r2.read(&compact_protocol2);
        }
    }
    auto finish = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(finish - start).count();

    auto start_deser = std::chrono::high_resolution_clock::now();
    for (size_t i = 0; i < iterations; i++) {
        buffer2->resetBuffer((uint8_t*)serialized.data(), serialized.length());

        if (proto == ThriftSerializationProto::Binary) {
            r2.read(&binary_protocol2);
        } else if (proto == ThriftSerializationProto::Compact) {
            r2.read(&compact_protocol2);
        }
    }
    auto finish_deser = std::chrono::high_resolution_clock::now();
    auto duration_deser = std::chrono::duration_cast<std::chrono::milliseconds>(finish_deser - start_deser).count();

    auto start_ser = std::chrono::high_resolution_clock::now();
    for (size_t i = 0; i < iterations; i++) {
        buffer1->resetBuffer();

        if (proto == ThriftSerializationProto::Binary) {
            r1.write(&binary_protocol1);
        } else if (proto == ThriftSerializationProto::Compact) {
            r1.write(&compact_protocol1);
        }

        serialized = buffer1->getBufferAsString();
    }
    auto finish_ser = std::chrono::high_resolution_clock::now();
    auto duration_ser = std::chrono::duration_cast<std::chrono::milliseconds>(finish_ser - start_ser).count();

    std::cout << tag << " roundtrip time = " << duration << " milliseconds" << std::endl;
    std::cout << tag << " deserialize time = " << duration_deser << " milliseconds" << std::endl;
    std::cout << tag << " serialize time = " << duration_ser << " milliseconds" << std::endl;
    std::cout << tag << " " << duration_ser << " " <<  duration_deser << " " << duration << std::endl << std::endl;

}

/*void
protobuf_serialization_test(size_t iterations)
{
    using namespace protobuf_test;

    Record r1;

    for (size_t i = 0; i < kIntegers.size(); i++) {
        r1.add_ids(kIntegers[i]);
    }

    for (size_t i = 0; i < kStringsCount; i++) {
        r1.add_strings(kStringValue);
    }

    std::string serialized;

    r1.SerializeToString(&serialized);

    // check if we can deserialize back
    Record r2;
    bool ok = r2.ParseFromString(serialized);
    if (!ok) {
        throw std::logic_error("protobuf's case: deserialization failed");
    }

    std::cout << "protobuf: version = " << GOOGLE_PROTOBUF_VERSION << std::endl;
    std::cout << "protobuf: size = " << serialized.size() << " bytes" << std::endl;

    auto start = std::chrono::high_resolution_clock::now();
    for (size_t i = 0; i < iterations; i++) {
        serialized.clear();
        r1.SerializeToString(&serialized);
        r2.ParseFromString(serialized);
    }
    auto finish = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(finish - start).count();

    std::cout << "protobuf: time = " << duration << " milliseconds" << std::endl << std::endl;
}

void
capnproto_serialization_test(size_t iterations)
{
    using namespace capnp_test;

    capnp::MallocMessageBuilder message;
    Record::Builder r1 = message.getRoot<Record>();

    auto ids = r1.initIds(kIntegers.size());
    for (size_t i = 0; i < kIntegers.size(); i++) {
        ids.set(i, kIntegers[i]);
    }

    auto strings = r1.initStrings(kStringsCount);
    for (size_t i = 0; i < kStringsCount; i++) {
        strings.set(i, kStringValue);
    }

    kj::ArrayPtr<const kj::ArrayPtr<const capnp::word>> serialized =
        message.getSegmentsForOutput();

    // check if we can deserialize back
    capnp::SegmentArrayMessageReader reader(serialized);
    Record::Reader r2 = reader.getRoot<Record>();
    if (r2.getIds().size() != kIntegers.size()) {
        throw std::logic_error("capnproto's case: deserialization failed");
    }

    size_t size = 0;
    for (auto segment: serialized) {
      size += segment.asBytes().size();
    }

    std::cout << "capnproto: version = " << CAPNP_VERSION << std::endl;
    std::cout << "capnproto: size = " << size << " bytes" << std::endl;

    auto start = std::chrono::high_resolution_clock::now();
    for (size_t i = 0; i < iterations; i++) {
        serialized = message.getSegmentsForOutput();
        capnp::SegmentArrayMessageReader reader(serialized);
        reader.getRoot<Record>();
    }
    auto finish = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(finish - start).count();

    std::cout << "capnproto: time = " << duration << " milliseconds" << std::endl << std::endl;
}

void
msgpack_serialization_test(size_t iterations)
{
    using namespace msgpack_test;

    Record r1, r2;

    for (size_t i = 0; i < kIntegers.size(); i++) {
        r1.ids.push_back(kIntegers[i]);
    }

    for (size_t i = 0; i < kStringsCount; i++) {
        r1.strings.push_back(kStringValue);
    }

    msgpack::sbuffer sbuf;

    msgpack::pack(sbuf, r1);

    std::string serialized(sbuf.data(), sbuf.size());

    msgpack::unpacked msg;
    msgpack::unpack(&msg, serialized.data(), serialized.size());

    msgpack::object obj = msg.get();

    obj.convert(&r2);

    if (r1 != r2) {
        throw std::logic_error("msgpack's case: deserialization failed");
    }

    std::cout << "msgpack: version = " << msgpack_version() << std::endl;
    std::cout << "msgpack: size = " << serialized.size() << " bytes" << std::endl;

    auto start = std::chrono::high_resolution_clock::now();
    for (size_t i = 0; i < iterations; i++) {
        sbuf.clear();
        msgpack::pack(sbuf, r1);
        msgpack::unpacked msg;
        msgpack::unpack(&msg, sbuf.data(), sbuf.size());
        msgpack::object obj = msg.get();
        obj.convert(&r2);
    }
    auto finish = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(finish - start).count();

    std::cout << "msgpack: time = " << duration << " milliseconds" << std::endl << std::endl;
}

void
cereal_serialization_test(size_t iterations)
{
    using namespace cereal_test;

    Record r1, r2;

    for (size_t i = 0; i < kIntegers.size(); i++) {
        r1.ids.push_back(kIntegers[i]);
    }

    for (size_t i = 0; i < kStringsCount; i++) {
        r1.strings.push_back(kStringValue);
    }

    std::string serialized;

    to_string(r1, serialized);
    from_string(r2, serialized);
if (r1 != r2) { throw std::logic_error("cereal's case: deserialization failed");
    }

    std::cout << "cereal: size = " << serialized.size() << " bytes" << std::endl;

    auto start = std::chrono::high_resolution_clock::now();
    for (size_t i = 0; i < iterations; i++) {
        serialized.clear();
        to_string(r1, serialized);
        from_string(r2, serialized);
    }
    auto finish = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(finish - start).count();

    std::cout << "cereal: time = " << duration << " milliseconds" << std::endl << std::endl;
}

void
avro_serialization_test(size_t iterations)
{
    using namespace avro_test;

    Record r1, r2;

    for (size_t i = 0; i < kIntegers.size(); i++) {
        r1.ids.push_back(kIntegers[i]);
    }

    for (size_t i = 0; i < kStringsCount; i++) {
        r1.strings.push_back(kStringValue);
    }

    std::auto_ptr<avro::OutputStream> out = avro::memoryOutputStream();
    avro::EncoderPtr encoder = avro::binaryEncoder();

    encoder->init(*out);
    avro::encode(*encoder, r1);

    auto serialized_size = out->byteCount();

    std::auto_ptr<avro::InputStream> in = avro::memoryInputStream(*out);
    avro::DecoderPtr decoder = avro::binaryDecoder();

    decoder->init(*in);
    avro::decode(*decoder, r2);

    if (r1.ids != r2.ids || r1.strings != r2.strings ||
        r2.ids.size() != kIntegers.size() || r2.strings.size() != kStringsCount) {
        throw std::logic_error("avro's case: deserialization failed");
    }

    std::cout << "avro: size = " << serialized_size << " bytes" << std::endl;

    auto start = std::chrono::high_resolution_clock::now();
    for (size_t i = 0; i < iterations; i++) {
        auto out = avro::memoryOutputStream();
        auto encoder = avro::binaryEncoder();
        encoder->init(*out);
        avro::encode(*encoder, r1);

        auto in = avro::memoryInputStream(*out);
        auto decoder = avro::binaryDecoder();
        decoder->init(*in);
        avro::decode(*decoder, r2);
    }
    auto finish = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(finish - start).count();

    std::cout << "avro: time = " << duration << " milliseconds" << std::endl << std::endl;
}*/

int
main(int argc, char **argv)
{
    //GOOGLE_PROTOBUF_VERIFY_VERSION;
    if (argc < 3) {
        std::cout << "usage: " << argv[0] << " N file [thrift-binary thrift-compact rapidjson protobuf boost capnproto]";
        std::cout << std::endl << std::endl;
        std::cout << "arguments: " << std::endl;
        std::cout << " N  -- number of iterations" << std::endl << std::endl;
        return EXIT_SUCCESS;
    }

    size_t iterations;

    try {
        iterations = boost::lexical_cast<size_t>(argv[1]);
    } catch (std::exception &exc) {
        std::cerr << "Error: " << exc.what() << std::endl;
        std::cerr << "First positional argument must be an integer." << std::endl;
        return EXIT_FAILURE;
    }

    std::string testfile;
    testfile = argv[2];

    std::set<std::string> names;

    if (argc > 3) {
        for (int i = 3; i < argc; i++) {
            names.insert(argv[i]);
        }
    }

    std::cout << "grabbing test data " << testfile << std::endl << std::endl;
    std::cout << "performing " << iterations << " iterations" << std::endl << std::endl;

    try {
        // MediaContent testContent = get_test_data(testfile);
        if (names.empty() || names.find("rapidjson") != names.end()) {
            rapidjson_serialization_test(testfile, iterations);
        }

        /*if (names.empty() || names.find("boost") != names.end()) {
            boost_serialization_test(iterations);
        }*/

        if (names.empty() || names.find("thrift-binary") != names.end()) {
            thrift_serialization_test(testfile, iterations, ThriftSerializationProto::Binary);
        }

        if (names.empty() || names.find("thrift-compact") != names.end()) {
            thrift_serialization_test(testfile, iterations, ThriftSerializationProto::Compact);
        }

        /*if (names.empty() || names.find("protobuf") != names.end()) {
            protobuf_serialization_test(iterations);
        }

        if (names.empty() || names.find("capnproto") != names.end()) {
            capnproto_serialization_test(iterations);
        }


        if (names.empty() || names.find("msgpack") != names.end()) {
            msgpack_serialization_test(iterations);
        }

        if (names.empty() || names.find("cereal") != names.end()) {
            cereal_serialization_test(iterations);
        }

        if (names.empty() || names.find("avro") != names.end()) {
            avro_serialization_test(iterations);
         }*/
    } catch (std::exception &exc) {
        std::cerr << "Error: " << exc.what() << std::endl;
        return EXIT_FAILURE;
    }

    //google::protobuf::ShutdownProtobufLibrary();

    return EXIT_SUCCESS;
}
