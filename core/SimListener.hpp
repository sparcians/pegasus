#pragma once

namespace simdb
{
    class AppManager;
    class AppRegistrations;
} // namespace simdb

namespace pegasus
{

    // Helper class to send Simulation build/configure/etc. calls to listeners
    class SimListener
    {
      public:
        virtual ~SimListener() = default;

        virtual void onBuildTreeEarly() {}

        virtual void onBuildTreeLate() {}

        virtual void onConfigureTreeEarly() {}

        virtual void onConfigureTreeLate() {}

        virtual void onBindTreeEarly() {}

        virtual void onBindTreeLate() {}

        virtual void onRegisterAppsRequest(simdb::AppRegistrations*) {}

        virtual void onParameterizeAppsRequest(simdb::AppManager*) {}

        virtual void onFrameworkFinalized() {}
    };

} // namespace pegasus
