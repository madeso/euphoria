#ifndef CORE_TEXTPARSER_H
#define CORE_TEXTPARSER_H

#include <string>
#include <memory>
#include <vector>
#include <sstream>

class ParsedText;

namespace textparser
{
  class Visitor;

  class Node
  {
   public:
    virtual ~Node() = default;
    virtual void
    Visit(Visitor* visitor) = 0;
  };

  class TextNode : public Node
  {
   public:
    std::string text;
    explicit TextNode(const std::string& t);
    void
    Visit(Visitor* visitor) override;
  };

  class ImageNode : public Node
  {
   public:
    std::string image;
    explicit ImageNode(const std::string& t);
    void
    Visit(Visitor* visitor) override;
  };

  class BeginEndNode : public Node
  {
   public:
    bool begin;

    explicit BeginEndNode(bool b);

    void
    Visit(Visitor* visitor) override;
  };

  class Visitor
  {
   public:
    virtual void
    OnText(TextNode* text) = 0;

    virtual void
    OnImage(ImageNode* image) = 0;

    virtual void
    OnBegin() = 0;

    virtual void
    OnEnd() = 0;
  };

  class VisitorDebugString : public Visitor
  {
   public:
    std::ostringstream ss;

    void
    OnText(TextNode* text) override;
    void
    OnImage(ImageNode* img) override;

    void
    OnBegin() override;

    void
    OnEnd() override;

    static std::string
    Visit(ParsedText* visitor);
  };
}

// @image with text and {-begin and }-end markers \ escapes
// todo add ability to specify button/keys so that we can use:
// use "[jump] to jump" and said [jump] would look up the keybind and display a
// sprite/ninepath w/ text representing the bind this could also be useful when
// displaying all the keybinds. needs to work out how (or if) the joystick
// id/number also should be displayed.
class ParsedText
{
 public:
  void
  Clear();
  void
  AddText(const std::string& str);
  void
  AddImage(const std::string& img);
  void
  AddBegin();
  void
  AddEnd();

  void
  CreateText(const std::string& str);
  bool
  CreateParse(const std::string& str);
  void
  Visit(textparser::Visitor* visitor);

 private:
  std::vector<std::shared_ptr<textparser::Node>> nodes;
};


#endif  // SPACETYPER_TEXTPARSER_H
