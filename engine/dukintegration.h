#ifndef EUPHORIA_DUKINTEGRATION_H
#define EUPHORIA_DUKINTEGRATION_H

#include <memory>

class Systems;
class Duk;
class Input;
class World;
struct DukRegistry;
class ObjectCreator;
struct Components;

struct DukIntegrationPimpl;

class DukIntegration
{
 public:
  DukIntegration(
      Systems*       systems,
      World*         reg,
      Duk*           duk,
      ObjectCreator* creator,
      Components*    components);
  ~DukIntegration();

  void
  Clear();

  DukRegistry&
  Registry();

  void
  BindKeys(Duk* duk, const Input& input);

 private:
  std::unique_ptr<DukIntegrationPimpl> pimpl;
};


#endif  // EUPHORIA_DUKINTEGRATION_H
