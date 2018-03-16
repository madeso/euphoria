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

class CScript
{
 public:
  std::map<ComponentId, DukValue> properties;
};

constexpr ComponentId ComponentIdStart = 100;

class DukRegistry
{
 public:
  explicit DukRegistry(EntReg* r)
      : reg(r)
      , last_script_id(ComponentIdStart + 1)
  {
    add<CPosition2>();
    add<CSprite>();
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

  ComponentId
  CreateNewId()
  {
    ComponentId id = last_script_id;
    last_script_id += 1;
    return id;
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

  DukValue
  GetProperty(EntityId ent, ComponentId comp)
  {
    CScript& s = reg->accomodate<CScript>(ent);
    return s.properties[comp];
  }

  void
  SetProperty(EntityId ent, ComponentId comp, DukValue value)
  {
    CScript& s         = reg->accomodate<CScript>(ent);
    s.properties[comp] = value;
  }

  template <typename T>
  T*
  GetComponentOrNull(EntityId id)
  {
    if(reg->has<T>(id))
    {
      T& t = reg->get<T>(id);
      return &t;
    }
    else
    {
      return nullptr;
    }
  }

 private:
  template <typename T>
  void
  add()
  {
    auto id  = reg->component<T>();
    auto ptr = std::make_shared<TComp<T>>();
    comps.insert(std::make_pair(id, ptr));
  }

  bool
  hasEntityComponent(ComponentId t, EntityId ent)
  {
    if(t > ComponentIdStart)
    {
      if(!reg->has<CScript>(ent))
      {
        return false;
      }
      CScript& comp = reg->get<CScript>(ent);
      auto     f    = comp.properties.find(t);
      return f != comp.properties.end();
    }
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


  EntReg* reg;
  std::map<ComponentId, std::shared_ptr<Comp>> comps;
  ComponentId last_script_id;
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
  DukIntegrationPimpl(Systems* sys, World* world, Duk* duk)
      : systems(sys)
      , registry(&world->reg)
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
    dukglue_register_property(
        duk->ctx, &CPosition2::GetPosition, &CPosition2::SetPosition, "vec");
  }

  DukSystems  systems;
  DukRegistry registry;
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
