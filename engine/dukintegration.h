#ifndef EUPHORIA_DUKINTEGRATION_H
#define EUPHORIA_DUKINTEGRATION_H

#include <memory>

class Systems;
class Duk;
class World;

struct DukIntegrationPimpl;

class DukIntegration
{
 public:
  DukIntegration(Systems* systems, World* reg, Duk* duk);
  ~DukIntegration();

  void
  Clear();

 private:
  std::unique_ptr<DukIntegrationPimpl> pimpl;
};


#endif  // EUPHORIA_DUKINTEGRATION_H
