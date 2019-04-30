#include "core/argparse.h"

#include "core/image.h"
#include "core/imagefilters.h"

#include "core/io.h"

#include <iostream>
#include <sstream>
#include <iomanip>

// debug::MemoryChunkToFile(drawer.image.Write(ImageWriteFormat::PNG), output.NextFile());

int main(int argc, char* argv[])
{
  auto parser = argparse::Parser{"Apply filters to images"};

  std::string input;
  std::string output = "ret.png";

  Image image;

  auto load_image = [&] {
    auto ret = LoadImage(io::FileToChunk(input), input, AlphaLoad::Keep);
    if(!ret.error.empty())
    {
      std::cerr << ret.error << "\n";
      return false;
    }
    else
    {
      image = ret.image;
      return true;
    }
  };
  auto write_image = [&] {
    io::ChunkToFile(image.Write(ImageWriteFormat::PNG), output);
  };

  parser.AddSimple("input", &input).Help("The image to apply filters to");
  parser.AddSimple("-o, --output", &output).Help("Where to write the resulting image");

  parser.AddSubParser("nop",
      "Don't do anything",
      [&]{
      if(!load_image()) { return; }
      write_image();
  });

  Grayscale grayscale = Grayscale::Average;
  auto pgrayscale = parser.AddSubParser("grayscale",
      "Apply grayscale",
      [&]{
      if(!load_image()) { return; }
      MakeGrayscale(&image, grayscale);
      write_image();
  });
  pgrayscale->AddEnum("-g,--grayscale", &grayscale);

  const auto status = parser.Parse(argc, argv);
  if(status != argparse::ParseResult::Ok)
  {
    return -1;
  }


  return 0;
}
