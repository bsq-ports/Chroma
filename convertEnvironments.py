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


repository = Github().get_repo('AeroLuna/Chroma')

light_tables = repository.get_contents('Chroma/LightIDTables', 'v2.4.1')

environmentFolder = "./environments_converted"

os.makedirs(environmentFolder, exist_ok=True)
for file in light_tables:
    fileName = os.path.splitext(file.name)[0]
    print(f"Doing {fileName}")
    json_data = json_to_cpp(json.loads(file.decoded_content))

    with open(f"{environmentFolder}/{fileName}.txt", "w+") as file_converted:
        file_converted.write(json_data)
