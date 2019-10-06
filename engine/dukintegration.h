#ifndef EUPHORIA_DUKINTEGRATION_H
#define EUPHORIA_DUKINTEGRATION_H

#include <memory>

namespace euphoria::core
{
    class Systems;
    class World;
}  // namespace euphoria::core

namespace euphoria::duk
{
    class Duk;
}

namespace euphoria::engine
{
    class Input;
    struct DukRegistry;
    class ObjectCreator;
    struct DukIntegrationPimpl;
    class CameraData;
    struct Components;


    class DukIntegration
    {
        public:
        DukIntegration(
                core::Systems* systems,
                core::World*   reg,
                duk::Duk*      duk,
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
}  // namespace euphoria::engine

#endif  // EUPHORIA_DUKINTEGRATION_H
