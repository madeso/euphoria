#include "engine/dukintegration.h"

#include <map>

#include "core/componentsystem.h"

#include "core/duk.h"
#include "engine/components.h"
#include "engine/input.h"
#include "engine/dukregistry.h"

class DukUpdateSystem : public ComponentSystem, public ComponentSystemUpdate
{
  DukValue func;

 public:
  DukUpdateSystem(const std::string& name, DukValue f)
      : ComponentSystem(name)
      , func(f)
  {
  }

  void
  Update(EntReg* reg, float dt) const override
  {
    dukglue_pcall<void>(func.context(), func, dt);
  }

  void
  RegisterCallbacks(Systems* systems) override
  {
    systems->update.Add(this);
  }
};

class DukSystems
{
 public:
  Systems* systems;
  explicit DukSystems(Systems* s)
      : systems(s)
  {
  }

  void
  AddUpdate(const std::string& name, DukValue func)
  {
    systems->AddAndRegister(std::make_shared<DukUpdateSystem>(name, func));
  }
};

struct DukIntegrationPimpl
{
  DukIntegrationPimpl(Systems* sys, World* world, Duk* duk)
      : systems(sys)
      , registry(&world->reg)
      , input(dukglue_peval<DukValue>(
            duk->ctx, "var Input = new Object(); Input"))
  {
  }

  void
  Integrate(Duk* duk)
  {
    dukglue_register_global(duk->ctx, &systems, "Systems");
    dukglue_register_method(duk->ctx, &DukSystems::AddUpdate, "AddUpdate");

    dukglue_register_global(duk->ctx, &registry, "Registry");
    dukglue_register_method(duk->ctx, &DukRegistry::entities, "Entities");
    dukglue_register_method(duk->ctx, &DukRegistry::getSpriteId, "GetSpriteId");
    dukglue_register_method(
        duk->ctx, &DukRegistry::getPosition2dId, "GetPosition2Id");

    dukglue_register_method(duk->ctx, &DukRegistry::CreateNewId, "New");
    dukglue_register_method(duk->ctx, &DukRegistry::GetProperty, "Get");
    dukglue_register_method(duk->ctx, &DukRegistry::SetProperty, "Set");

    dukglue_register_method(
        duk->ctx, &DukRegistry::GetComponentOrNull<CPosition2>, "GetPosition2");
    dukglue_register_method(duk->ctx, &CPosition2::GetPosition, "GetPos");
    dukglue_register_method(duk->ctx, &CPosition2::SetPosition, "SetPos");
    dukglue_register_method(duk->ctx, &CPosition2::GetPositionRef, "GetPosRef");
    dukglue_register_property(
        duk->ctx, &CPosition2::GetPosition, &CPosition2::SetPosition, "vec");
  }

  DukSystems  systems;
  DukRegistry registry;
  DukValue    input;
};

DukIntegration::DukIntegration(Systems* systems, World* reg, Duk* duk)
{
  pimpl.reset(new DukIntegrationPimpl(systems, reg, duk));
  pimpl->Integrate(duk);
}

DukIntegration::~DukIntegration()
{
  Clear();
}

void
DukIntegration::Clear()
{
  pimpl.reset();
}

DukRegistry&
DukIntegration::Registry()
{
  ASSERT(pimpl);
  return pimpl->registry;
}

void
DukIntegration::BindKeys(Duk* duk, const Input& input)
{
  input.Set(duk, pimpl->input);
}
