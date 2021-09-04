# Converts the JSON environments to C++ unordered_map code

# pip install PyGithub
from github import Github

import json
import os


def indent(i, amount=4):
    return ' ' * (i * amount)


def json_to_cpp(dict_json):
    json_data_e = "{\n"

    for key in dict_json:
        json_data_e += indent(1) + "{" + str(key) + ", {" + '\n'
        for key2 in dict_json[key]:
            json_data_e += indent(2) + "{" + str(key2) + ", " + str(dict_json[key][key2]) + " }," + '\n'
        json_data_e += indent(1) + "}}," + '\n'

    json_data_e += "}"

    return json_data_e


def hppEnvironment(environmentName):
    return f"""
#pragma once

#include "lighting/LightIDTableManager.hpp"


namespace Chroma {{
    class {environmentName} : public EnvironmentData {{
    public:
        EnvironmentLightDataT getEnvironmentLights() override;
    }};
}}
"""


def cppEnvironment(environmentName, lightGroup):
    return f"""

#include "lighting/environments/{environmentName}.hpp"

using namespace Chroma;

EnvironmentLightDataT {environmentName}::getEnvironmentLights() {{
    return EnvironmentLightDataT(
        {lightGroup}
    );
}}

ChromaInstallEnvironment({environmentName})
"""


def allEnvironmentsHpp(environmentListParam):
    header = "#pragma once\n"

    for environment in environmentListParam:
        header += f"#include \"lighting/environments/{environment}.hpp\"\n"

    return header


repository = Github().get_repo('AeroLuna/Chroma')

light_tables = repository.get_contents('Chroma/LightIDTables', 'v2.5.0')

environmentFolder = "./environments_converted"

environmentHppFolder = "./include/lighting/environments"
environmentCppFolder = "./src/lighting/environments"

environmentList = []

os.makedirs(environmentFolder, exist_ok=True)
for file in light_tables:
    environmentName = os.path.splitext(file.name)[0]
    print(f"Doing {environmentName}")
    json_data = json_to_cpp(json.loads(file.decoded_content))

    with open(f"{environmentFolder}/{environmentName}.txt", "w+") as file_converted:
        file_converted.write(json_data)

    with open(f"{environmentHppFolder}/{environmentName}.hpp", "w+") as file_converted:
        file_converted.write(hppEnvironment(environmentName))

    with open(f"{environmentCppFolder}/{environmentName}.cpp", "w+") as file_converted:
        file_converted.write(cppEnvironment(environmentName, json_data))

    environmentList.append(environmentName)

with open(f"{environmentHppFolder}/AllEnvironments.hpp", "w+") as file_converted:
    file_converted.write(allEnvironmentsHpp(environmentList))
