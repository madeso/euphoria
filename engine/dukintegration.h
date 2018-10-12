#ifndef EUPHORIA_DUKINTEGRATION_H
#define EUPHORIA_DUKINTEGRATION_H

#include <memory>

namespace duk
{
  class Duk;
}

class Systems;
class Input;
class World;
struct DukRegistry;
class ObjectCreator;

struct Components;

struct DukIntegrationPimpl;

class CameraData;

class DukIntegration
{
 public:
  DukIntegration(
      Systems*       systems,
      World*         reg,
      duk::Duk*           duk,
      ObjectCreator* creator,
      Components*    components,
      CameraData*    camera);
  ~DukIntegration();

  void
  Clear();

  DukRegistry&
  Registry();

  void
  BindKeys(duk::Duk* duk, const Input& input);

 private:
  std::unique_ptr<DukIntegrationPimpl> pimpl;
};


#endif  // EUPHORIA_DUKINTEGRATION_H
