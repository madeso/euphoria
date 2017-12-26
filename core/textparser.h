#ifndef CORE_TEXTPARSER_H
#define CORE_TEXTPARSER_H

#include <string>
#include <memory>
#include <vector>
#include <sstream>

class TextParser;

namespace textparser
{
  class Visitor;

  class Node
  {
  public:
    virtual ~Node() = default;
    virtual void Visit(Visitor* visitor) = 0;
  };

  class TextNode : public Node
  {
  public:
    std::string text;
    explicit TextNode(const std::string& t);
    void Visit(Visitor* visitor) override;
  };

  class ImageNode : public Node
  {
  public:
    std::string image;
    explicit ImageNode(const std::string& t);
    void Visit(Visitor* visitor) override;
  };

  class VarNode : public Node
  {
  public:
    std::string var;
    explicit VarNode(const std::string& t);
    void Visit(Visitor* visitor) override;
  };

  class Visitor
  {
  public:
    virtual void OnText(TextNode* text) = 0;
    virtual void OnImage(ImageNode* image) = 0;
    virtual void OnVar(VarNode* var) = 0;
  };

  class VisitorDebugString : public Visitor
  {
  public:
    std::ostringstream ss;

    void OnText(TextNode* text) override;
    void OnImage(ImageNode* img) override;
    void OnVar(VarNode* var) override;

    static std::string Visit(TextParser* visitor);
  };

  class NodeCreator
  {
  public:
    virtual std::shared_ptr<TextNode> CreateTextNode(const std::string& text) = 0;
    virtual std::shared_ptr<ImageNode> CreateImageNode(const std::string& image) = 0;
    virtual std::shared_ptr<VarNode> CreateVarNode(const std::string& var) = 0;
  };

  class NodeCreatorBasic : public NodeCreator
  {
  public:
    std::shared_ptr<TextNode> CreateTextNode(const std::string& text) override;
    std::shared_ptr<ImageNode> CreateImageNode(const std::string& image) override;
    std::shared_ptr<VarNode> CreateVarNode(const std::string& var) override;
  };
}

// {var} and @image strings
class TextParser {
public:
  void CreateText(const std::string& str, textparser::NodeCreator* creator);
  bool CreateParse(const std::string& str, textparser::NodeCreator* creator);
  void Visit(textparser::Visitor* visitor);
private:
  std::vector<std::shared_ptr<textparser::Node>> nodes;
};


#endif //SPACETYPER_TEXTPARSER_H
