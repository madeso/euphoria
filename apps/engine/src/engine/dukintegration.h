#ifndef EUPHORIA_DUKINTEGRATION_H
#define EUPHORIA_DUKINTEGRATION_H

#include <memory>

#include "core/sol.h"

namespace euphoria::core::ecs
{
    struct Systems;
    struct World;
}  // namespace euphoria::core::ecs

namespace euphoria::engine
{
    struct Input;
    struct DukRegistry;
    struct ObjectCreator;
    struct DukIntegrationPimpl;
    struct CameraData;
    struct Components;


    struct DukIntegration
    {
    public:
        DukIntegration(
                core::ecs::Systems* systems,
                core::ecs::World*   reg,
                Sol*           duk,
                ObjectCreator*      creator,
                Components*         components,
                CameraData*         camera);
        ~DukIntegration();

        void
        Clear();

        DukRegistry&
        Registry();

        void
  BindKeys(Sol* duk, const Input& input);

    private:
        std::unique_ptr<DukIntegrationPimpl> pimpl;
    };
}  // namespace euphoria::engine

#endif  // EUPHORIA_DUKINTEGRATION_H
