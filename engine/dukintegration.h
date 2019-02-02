#ifndef EUPHORIA_DUKINTEGRATION_H
#define EUPHORIA_DUKINTEGRATION_H

#include <memory>

#include "core/sol.h"

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
      sol::state*    duk,
      ObjectCreator* creator,
      Components*    components,
      CameraData*    camera);
  ~DukIntegration();

  void
  Clear();

  DukRegistry&
  Registry();

  void
  BindKeys(sol::state* duk, const Input& input);

 private:
  std::unique_ptr<DukIntegrationPimpl> pimpl;
};


#endif  // EUPHORIA_DUKINTEGRATION_H
