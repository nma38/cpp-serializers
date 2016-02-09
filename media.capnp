@0xfe6ad24674c171e2;

$import "/capnp/c++.capnp".namespace("capnp_test");

enum Size {
  small @0;
  large @1;
}

enum Player {
  java @0;
  flash @1;
}

struct Pod {
  message @0 :Text;
  pod @1 :Pod;
}

struct Image {
  uri @0 :Text;
  title @1 :Text;  
  width @2 :Int32;
  height @3 :Int32;
  size @4 :Size;
}

struct Media {
  uri @0 :Text;
  title @1 :Text;
  width @2 :Int32;
  height @3 :Int32;
  format @4 :Text;
  duration @5 :Int64;
  size @6 :Int64;
  bitrate @7 :Int32;
  person @8 :List(Text);
  player @9 :Player;
  copyright @10 :Text;
  pods @11 :List(Pod);
}

struct MediaContent {
  image @0 :List(Image);
  media @1 :Media;
}
