#ifndef EUPHORIA_TEMPLATE_H
#define EUPHORIA_TEMPLATE_H

#include <map>
#include <string>
#include <sstream>
#include <memory>

class Defines {
 public:
  Defines();

  bool IsDefined(const std::string& name) const;
  std::string GetValue(const std::string& name) const;

  void Undefine(const std::string& name);
  void Define(const std::string& name, const std::string& value);
 private:
  std::map<std::string, std::string> values_;
};

class TemplateNode {
 public:
  TemplateNode();
  virtual ~TemplateNode();

  virtual void Eval(const Defines& defines, std::ostringstream* out) = 0;
};

class TemplateNodeString : public TemplateNode {
 public:
  TemplateNodeString(const std::string& text);
  void Eval(const Defines& defines, std::ostringstream* out) override;

 private:
  std::string text_;
};

class TemplateNodeList : public TemplateNode {
 public:
  TemplateNodeList();

  void Eval(const Defines& defines, std::ostringstream* out) override;

  void Add(std::shared_ptr<TemplateNode> node);

 private:
  std::vector<std::shared_ptr<TemplateNode>> nodes_;
};

class TemplateNodeIfdef : public TemplateNode {
 public:
  TemplateNodeIfdef(const std::string& name, std::shared_ptr<TemplateNode> node);

  void Eval(const Defines& defines, std::ostringstream* out) override;

 private:
  std::string name_;
  std::shared_ptr<TemplateNode> node_;
};

class TemplateNodeEval : public TemplateNode {
 public:
  TemplateNodeEval(const std::string& name);

  void Eval(const Defines& defines, std::ostringstream* out) override;

 private:
  std::string name_;
};

class Template {
 public:
  explicit Template(const std::string& text);

  std::string Evaluate(const Defines& defines);
 private:
  std::shared_ptr<TemplateNodeList> nodes_;
};


#endif //EUPHORIA_TEMPLATE_H
