#ifndef EUPHORIA_DUKINTEGRATION_H
#define EUPHORIA_DUKINTEGRATION_H

#include <memory>

class Systems;
class Duk;

struct DukIntegrationPimpl;

class DukIntegration
{
 public:
  DukIntegration(Systems* systems, Duk* duk);
  ~DukIntegration();

  void
  Clear();

 private:
  std::unique_ptr<DukIntegrationPimpl> pimpl;
};


#endif  // EUPHORIA_DUKINTEGRATION_H
