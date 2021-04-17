#include "lighting/environments/GreenDayGrenadeEnvironment.hpp"

using namespace Chroma;

EnvironmentLightDataT GreenDayGrenadeEnvironment::getEnvironmentLights() {
    return EnvironmentLightDataT(
            {
                    {0, {
                                {1, 9},
                                {2, 10},
                                {3, 5},
                                {4, 19},
                                {5, 15},
                                {6, 17},
                                {7, 16},
                                {8, 7},
                                {9, 20},
                                {10, 14},
                                {11, 12},
                                {12, 8},
                                {13, 18},
                                {14, 11},
                                {15, 6},
                                {16, 13},
                        }},
                    {2, {
                                {1, 4},
                                {2, 2},
                                {3, 3},
                                {4, 6},
                                {5, 0},
                                {6, 5},
                        }},
                    {3, {
                                {1, 5},
                                {2, 0},
                                {3, 6},
                                {4, 3},
                                {5, 2},
                                {6, 4},
                        }},
                    {4, {
                                {1, 4},
                                {2, 11},
                                {3, 12},
                                {4, 6},
                                {5, 7},
                        }},
            }
    );

}