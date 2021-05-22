#include <cmath>
#include <cstdio>
#include <fstream>
#include <string>
#include <vector>

#include <nlohmann/json.hpp>

using json = nlohmann::json;

constexpr float PI = 3.14159265358979323846f;

float maxf(float x, float other)
{
	return (x > other) ? x : other;
}

float minf(float x, float other)
{
	return (x < other) ? x : other;
}

float clampf(float x, float minVal, float maxVal)
{
	float t = (x > minVal) ? x : minVal;

	return (t < maxVal) ? t : maxVal;
}

float radiansf(float degrees)
{
	return (PI * degrees) / 180.0f;
}

float degreesf(float radians)
{
	return 180.0f * radians / PI;
}

bool loadFile(std::string& output, const std::string& filename)
{
	std::ifstream file(filename, std::ios::ate | std::ios::binary);
	if (!file.is_open())
	{
		return false;
	}

	size_t fileSize = static_cast<size_t>(file.tellg());
	file.seekg(0);

	output.resize(fileSize);

	file.read(output.data(), fileSize);
	file.close();

	return true;
}

bool saveFile(const std::string& output, const std::string& filename)
{
	std::ofstream file(filename, std::ios::binary);
	if (!file.is_open())
	{
		return false;
	}

	file.write(output.data(), output.size());
	file.close();

	return true;
}

void applyScale(json& glTF, std::vector<float>& vertices, float scale)
{
	float minimum[3];
	float maximum[3];

	size_t counter = 0;
	for (float& f : vertices)
	{
		f *= scale;

		if (counter < 3)
		{
			minimum[counter%3] = f;
			maximum[counter%3] = f;
		}
		else
		{
			minimum[counter%3] = minf(minimum[counter%3], f);
			maximum[counter%3] = maxf(maximum[counter%3], f);
		}

		counter++;
	}

    glTF["accessors"][0]["min"][0] = minimum[0];
    glTF["accessors"][0]["min"][1] = minimum[1];
    glTF["accessors"][0]["min"][2] = minimum[2];

    glTF["accessors"][0]["max"][0] = maximum[0];
    glTF["accessors"][0]["max"][1] = maximum[1];
    glTF["accessors"][0]["max"][2] = maximum[2];
}

int main(int argc, char *argv[])
{
	std::string primitive = "cube";

	float scale = 1.0f;

	//

	for (int i = 0; i < argc; i++)
	{
		if (strcmp(argv[i], "-p") == 0 && (i + 1 < argc))
		{
			if (strcmp(argv[i + 1], "cube") == 0)
			{
				primitive = "cube";
			}
			else if (strcmp(argv[i + 1], "plane") == 0)
			{
				primitive = "plane";
			}
			else
			{
				printf("Error: Unknown primitive '%s'\n", argv[i + 1]);

				return -1;
			}
		}
		else if (strcmp(argv[i], "-s") == 0 && (i + 1 < argc))
		{
			scale = std::stof(argv[i + 1]);
		}
	}

	//

	std::string generatorname = primitive;

	generatorname += "_s" + std::to_string(scale);

	std::string loadname = "template.gltf";
    std::string savename = generatorname + ".gltf";
    std::string binaryname = generatorname + ".bin";

    std::string templateContent;
	if (!loadFile(templateContent, loadname))
	{
		printf("Error: Could not load template glTF file '%s'\n", loadname.c_str());

		return -1;
	}

	//

    json glTF = json::parse(templateContent);
    glTF["buffers"][0]["uri"] = binaryname;

    //

    std::vector<float> floatData;
    std::vector<uint16_t> shortData;

	size_t numberAttributes = 0;
	size_t numberIndices = 0;

    size_t byteOffset = 0;
    size_t byteLength = 0;

    //

    std::vector<float> vertices;

    if (primitive == "cube")
    {
    	numberAttributes = 24;
    	numberIndices = 36;

        vertices = {
        		-1.0f, -1.0f, -1.0f,
				-1.0f, -1.0f, +1.0f,
				+1.0f, -1.0f, +1.0f,
				+1.0f, -1.0f, -1.0f,

				-1.0f, +1.0f, -1.0f,
				-1.0f, +1.0f, +1.0f,
				+1.0f, +1.0f, +1.0f,
				+1.0f, +1.0f, -1.0f,

                -1.0f, -1.0f, -1.0f,
				-1.0f, +1.0f, -1.0f,
				+1.0f, +1.0f, -1.0f,
				+1.0f, -1.0f, -1.0f,

                -1.0f, -1.0f, +1.0f,
				-1.0f, +1.0f, +1.0f,
				+1.0f, +1.0f, +1.0f,
				+1.0f, -1.0f, +1.0f,

                -1.0f, -1.0f, -1.0f,
				-1.0f, -1.0f, +1.0f,
				-1.0f, +1.0f, +1.0f,
				-1.0f, +1.0f, -1.0f,

                +1.0f, -1.0f, -1.0f,
				+1.0f, -1.0f, +1.0f,
				+1.0f, +1.0f, +1.0f,
				+1.0f, +1.0f, -1.0f
        };
    }
    else if (primitive == "plane")
    {
    	numberAttributes = 4;
    	numberIndices = 6;

        vertices = {
        		-1.0f, 0.0f, -1.0f,
				-1.0f, 0.0f, +1.0f,
				+1.0f, 0.0f, -1.0f,
				+1.0f, 0.0f, +1.0f
        };
    }

	applyScale(glTF, vertices, scale);

	floatData.insert(floatData.end(), vertices.begin(), vertices.end());

	glTF["accessors"][0]["count"] = numberAttributes;

	byteLength = numberAttributes * 3 * sizeof(float);

	glTF["bufferViews"][0]["byteLength"] = byteLength;
	glTF["bufferViews"][0]["byteOffset"] = byteOffset;

	byteOffset += byteLength;

	//

	std::vector<float> normals;

    if (primitive == "cube")
    {
        normals = {
        		0.0f, -1.0f, 0.0f,
				0.0f, -1.0f, 0.0f,
				0.0f, -1.0f, 0.0f,
				0.0f, -1.0f, 0.0f,

        	    0.0f, +1.0f, 0.0f,
				0.0f, +1.0f, 0.0f,
				0.0f, +1.0f, 0.0f,
				0.0f, +1.0f, 0.0f,

        	    0.0f, 0.0f, -1.0f,
				0.0f, 0.0f, -1.0f,
				0.0f, 0.0f, -1.0f,
				0.0f, 0.0f, -1.0f,

        	    0.0f, 0.0f, +1.0f,
				0.0f, 0.0f, +1.0f,
				0.0f, 0.0f, +1.0f,
				0.0f, 0.0f, +1.0f,

        	    -1.0f, 0.0f, 0.0f,
				-1.0f, 0.0f, 0.0f,
				-1.0f, 0.0f, 0.0f,
				-1.0f, 0.0f, 0.0f,

        	    +1.0f, 0.0f, 0.0f,
				+1.0f, 0.0f, 0.0f,
				+1.0f, 0.0f, 0.0f,
				+1.0f, 0.0f, 0.0f
        };
    }
    else if (primitive == "plane")
    {
        normals = {
        		0.0f, +1.0f, 0.0f,
				0.0f, +1.0f, 0.0f,
				0.0f, +1.0f, 0.0f,
				0.0f, +1.0f, 0.0f
        };
    }

	floatData.insert(floatData.end(), normals.begin(), normals.end());

	glTF["accessors"][1]["count"] = numberAttributes;

	byteLength = numberAttributes * 3 * sizeof(float);

	glTF["bufferViews"][1]["byteLength"] = byteLength;
	glTF["bufferViews"][1]["byteOffset"] = byteOffset;

	byteOffset += byteLength;

    //

	std::vector<float> texCoords;

    if (primitive == "cube")
    {
        texCoords = {
        		0.0f, 0.0f,
				0.0f, 1.0f,
				1.0f, 1.0f,
				1.0f, 0.0f,

        		0.0f, 1.0f,
				0.0f, 0.0f,
				1.0f, 0.0f,
				1.0f, 1.0f,

        		1.0f, 0.0f,
				1.0f, 1.0f,
				0.0f, 1.0f,
				0.0f, 0.0f,

        		0.0f, 0.0f,
				0.0f, 1.0f,
				1.0f, 1.0f,
				1.0f, 0.0f,

        		0.0f, 0.0f,
				1.0f, 0.0f,
				1.0f, 1.0f,
				0.0f, 1.0f,

        		1.0f, 0.0f,
				0.0f, 0.0f,
				0.0f, 1.0f,
				1.0f, 1.0f
        };
    }
    else if (primitive == "plane")
    {
        texCoords = {
        		0.0f, 1.0f,
				0.0f, 0.0f,
				1.0f, 1.0f,
				1.0f, 0.0f
        };
    }

	floatData.insert(floatData.end(), texCoords.begin(), texCoords.end());

	glTF["accessors"][2]["count"] = numberAttributes;

	byteLength = numberAttributes * 2 * sizeof(float);

	glTF["bufferViews"][2]["byteLength"] = byteLength;
	glTF["bufferViews"][2]["byteOffset"] = byteOffset;

	byteOffset += byteLength;

    //

	std::vector<uint16_t> indices;

    if (primitive == "cube")
    {
        indices = {
        		0, 2, 1, 0, 3, 2, 4, 5, 6, 4, 6, 7, 8, 9, 10, 8, 10, 11, 12, 15, 14, 12, 14, 13, 16, 17, 18, 16, 18, 19, 20, 23, 22, 20, 22, 21
        };

        // No changes to the material
    }
    else if (primitive == "plane")
    {
        indices = {
        		0, 1, 2, 3, 2, 1
        };

        glTF["materials"][0]["doubleSided"] = true;
    }

	shortData.insert(shortData.end(), indices.begin(), indices.end());

	glTF["accessors"][3]["count"] = numberIndices;

	byteLength = numberIndices * 1 * sizeof(uint16_t);

	glTF["bufferViews"][3]["byteLength"] = byteLength;
	glTF["bufferViews"][3]["byteOffset"] = byteOffset;

	byteOffset += byteLength;

	//

    std::string data;
    data.resize(floatData.size() * sizeof(float) + shortData.size() * sizeof(uint16_t));
    memcpy(data.data(), floatData.data(), floatData.size() * sizeof(float));
    memcpy(&data.data()[floatData.size() * sizeof(float)], shortData.data(), shortData.size() * sizeof(uint16_t));

    glTF["buffers"][0]["byteLength"] = data.size();

    glTF["materials"][0]["name"] = generatorname;
    glTF["meshes"][0]["name"] = generatorname;

	if (!saveFile(data, binaryname))
	{
		printf("Error: Could not save generated binary file '%s' \n", binaryname.c_str());

		return -1;
	}

    //

	if (!saveFile(glTF.dump(3), savename))
	{
		printf("Error: Could not save generated glTF file '%s' \n", savename.c_str());

		return -1;
	}

	return 0;
}
