namespace cpp thrift_test

enum Size {
  SMALL = 0,
  LARGE = 1,
}

enum Player {
  JAVA = 0,
  FLASH = 1,
}

struct Pod {
  1: string message,
  2: optional Pod pod,
}

/**
 * Some comment...
 */
struct Image {
  1: string uri,              //url to the images
  2: optional string title,  
  3: required i32 width,
  4: required i32 height,
  5: required Size size,
}

struct Media {
  1: string uri,             //url to the thumbnail
  2: optional string title,
  3: required i32 width,
  4: required i32 height,
  5: required string format,
  6: required i64 duration,
  7: required i64 size,
  8: optional i32 bitrate,
  9: required list<string> person,
  10: required Player player,
  11: optional string copyright,
  12: required list<Pod> pods,
}

struct MediaContent {
  1: required list<Image> image,
  2: required Media media,
}
