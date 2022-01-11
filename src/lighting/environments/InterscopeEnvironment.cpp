

#include "lighting/environments/InterscopeEnvironment.hpp"

using namespace Chroma;

EnvironmentLightDataT InterscopeEnvironment::getEnvironmentLights() {
    return EnvironmentLightDataT(
        {
    {0, {
        {1, 1 },
        {2, 0 },
        {3, 7 },
    }},
    {1, {
        {1, 7 },
        {2, 6 },
        {3, 5 },
    }},
    {2, {
        {1, 1 },
        {2, 7 },
        {3, 0 },
    }},
    {3, {
        {1, 1 },
        {2, 7 },
        {3, 0 },
    }},
    {4, {
        {1, 4 },
        {2, 10 },
        {3, 3 },
    }},
    {5, {
    }},
    {6, {
        {1, 0 },
        {2, 1 },
        {3, 2 },
        {4, 3 },
        {5, 4 },
        {6, 5 },
        {7, 6 },
    }},
    {7, {
        {1, 0 },
        {2, 1 },
        {3, 2 },
        {4, 3 },
        {5, 4 },
        {6, 5 },
        {7, 6 },
    }},
}
    );
}

ChromaInstallEnvironment(InterscopeEnvironment)
