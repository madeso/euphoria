#include "engine/dukintegration.h"

#include <map>

#include "core/componentsystem.h"

#include "engine/duk.h"
#include "engine/components.h"

#include "dukglue/dukglue.h"

typedef unsigned long ComponentId;
typedef unsigned long EntityId;

struct Comp
{
  virtual ~Comp() = default;
  virtual bool
  has(EntityId ent, const EntReg& reg) = 0;
};

template <typename T>
struct TComp : public Comp
{
  bool
  has(EntityId ent, const EntReg& reg) override
  {
    return reg.has<T>(ent);
  }
};

class DukRegistry
{
 public:
  explicit DukRegistry(EntReg* r)
      : reg(r)
  {
    add<CPosition2>();
    add<CSprite>();
  }

  template <typename T>
  void
  add()
  {
    auto id  = reg->component<T>();
    auto ptr = std::make_shared<TComp<T>>();
    comps.insert(std::make_pair(id, ptr));
  }

  ComponentId
  getPosition2dId()
  {
    return reg->component<CPosition2>();
  }

  ComponentId
  getSpriteId()
  {
    return reg->component<CSprite>();
  }

  bool
  hasEntityComponent(ComponentId t, EntityId ent)
  {
    auto f = comps.find(t);
    if(f == comps.end())
    {
      return false;
    }
    else
    {
      return f->second->has(ent, *reg);
    }
  }

  bool
  hasEntityComponent(const std::vector<ComponentId>& types, EntityId ent)
  {
    for(auto t : types)
    {
      if(false == hasEntityComponent(t, ent))
      {
        return false;
      }
    }

    return true;
  }

  std::vector<EntityId>
  entities(const std::vector<ComponentId>& types)
  {
    std::vector<EntityId> ret;
    reg->each([this, types, &ret](auto ent) {
      if(this->hasEntityComponent(types, ent))
      {
        ret.push_back(ent);
      }
    });
    return ret;
  }

  EntReg* reg;
  std::map<ComponentId, std::shared_ptr<Comp>> comps;
};

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
  DukIntegrationPimpl(Systems* sys, Duk* duk)
      : systems(sys)
  {
  }

  void
  Integrate(Duk* duk)
  {
    dukglue_register_method(duk->ctx, &DukSystems::AddUpdate, "AddUpdate");
    dukglue_register_global(duk->ctx, &systems, "Systems");
  }

  DukSystems systems;
};

DukIntegration::DukIntegration(Systems* systems, Duk* duk)
{
  pimpl.reset(new DukIntegrationPimpl(systems, duk));
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
