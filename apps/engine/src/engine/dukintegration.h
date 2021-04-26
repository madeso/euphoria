#ifndef EUPHORIA_DUKINTEGRATION_H
#define EUPHORIA_DUKINTEGRATION_H

#include <memory>

#include "core/sol.h"

namespace euphoria::core::ecs
{
    struct systems;
    struct world;
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
                core::ecs::systems* systems,
                core::ecs::world*   reg,
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
