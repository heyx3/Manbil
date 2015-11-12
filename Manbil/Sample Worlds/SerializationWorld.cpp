#include "SerializationWorld.h"

#include <iostream>
#include "../Rendering/Primitives/PrimitiveGenerator.h"

#include "../IO/DataSerialization.h"
#include "../IO/SerializationWrappers.h"
#include "../IO/XmlSerialization.h"


//Define a data structure that contains a single mesh vertex.
//It implements the ISerializable interface, which means it describes how to save/load its data.
struct WorldVertex : public ISerializable
{
public:

    VertexPosUVNormal Value;

    virtual void WriteData(DataWriter* writer) const override
    {
        //The most straightforward way to do this is to just write each component.
        if (false)
        {
            writer->WriteFloat(Value.Pos.x, "Pos.x");
            writer->WriteFloat(Value.Pos.y, "Pos.y");
            writer->WriteFloat(Value.Pos.z, "Pos.z");
            writer->WriteFloat(Value.UV.x, "UV.x");
            writer->WriteFloat(Value.UV.y, "UV.y");
            writer->WriteFloat(Value.Normal.x, "Normal.x");
            writer->WriteFloat(Value.Normal.y, "Normal.x");
            writer->WriteFloat(Value.Normal.z, "Normal.x");
        }
        //An easier way to do this is to let the vectors serialize themselves,
        //    but they don't implement IWritable or ISerializable.
        //Fortunately, serializers for many data structures are defined in "SerializationWrappers.h".
        else
        {
            writer->WriteDataStructure(Vector3f_Writable(Value.Pos), "Pos");
            writer->WriteDataStructure(Vector2f_Writable(Value.UV), "UV");
            writer->WriteDataStructure(Vector3f_Writable(Value.Normal), "Normal");
        }
    }
    virtual void ReadData(DataReader* reader) override
    {
        //As mentioned in WriteData() above, we can use helper classes to serialize vectors.
        reader->ReadDataStructure(Vector3f_Readable(Value.Pos));
        reader->ReadDataStructure(Vector2f_Readable(Value.UV));
        reader->ReadDataStructure(Vector3f_Readable(Value.Normal));
    }
};

//A data structure that contains world info.
//Like "WorldVertex", it is marked as serializable.
struct WorldInfo : public ISerializable
{
public:

    //The data contained in each vertex of the world geometry.
    RenderIOAttributes VertexInputs;

    //The shaders for the material used to render world geometry.
    std::string VertexShader, FragmentShader;

    //The vertices of the world geometry mesh.
    std::vector<WorldVertex> Vertices;
    //The indices of the world geometry mesh.
    std::vector<unsigned int> Indices;


    virtual void WriteData(DataWriter* writer) const override
    {
        writer->WriteDataStructure(RenderIOAttributes_Writable(VertexInputs), "Vertex inputs");

        writer->WriteString(VertexShader, "Vertex shader");
        writer->WriteString(FragmentShader, "Fragment shader");

        //Writing collections of objects is a bit more involved;
        //    you have to provide a callback describing how to write each element.
        DataWriter::ElementWriter vertWriter = [](DataWriter* writer, const void* elementToWrite,
                                                  unsigned int elIndex, void* pData)
            {
                WorldVertex& toWrite = *(WorldVertex*)elementToWrite;
                writer->WriteDataStructure(toWrite, "Vertex");
            };
        DataWriter::ElementWriter uIntWriter = [](DataWriter* writer, const void* elementToWrite,
                                                  unsigned int elIndex, void* pData)
            {
                unsigned int toWrite = *(unsigned int*)elementToWrite;
                writer->WriteUInt(toWrite, "Index");
            };
        writer->WriteCollection(vertWriter, "Vertices", sizeof(WorldVertex),
                                Vertices.data(), Vertices.size());
        writer->WriteCollection(uIntWriter, "Indices", sizeof(unsigned int),
                                Indices.data(), Indices.size());
    }
    virtual void ReadData(DataReader* reader) override
    {
        reader->ReadDataStructure(RenderIOAttributes_Readable(VertexInputs));

        reader->ReadString(VertexShader);
        reader->ReadString(FragmentShader);

        //Just like with writing a collection, we need to define callbacks when reading a collection.
        //One callback for deserializing an element, and another for resizing the collection.
        DataReader::CollectionResizer vertexResizer = [](void* pCollection, unsigned int newSize)
            {
                std::vector<WorldVertex>& vertices = *(std::vector<WorldVertex>*)pCollection;
                vertices.resize(newSize);
            };
        DataReader::ElementReader vertexReader = [](DataReader* reader, void* pCollection,
                                                    unsigned int elIndex, void* pData)
            {
                std::vector<WorldVertex>& vertices = *(std::vector<WorldVertex>*)pCollection;
                reader->ReadDataStructure(vertices[elIndex]);
            };
        DataReader::CollectionResizer indexResizer = [](void* pCollection, unsigned int newSize)
            {
                std::vector<unsigned int>& indices = *(std::vector<unsigned int>*)pCollection;
                indices.resize(newSize);
            };
        DataReader::ElementReader indexReader = [](DataReader* reader, void* pCollection,
                                                   unsigned int elIndex, void* pData)
            {
                std::vector<unsigned int>& indices = *(std::vector<unsigned int>*)pCollection;
                reader->ReadUInt(indices[elIndex]);
            };
        reader->ReadCollection(vertexReader, vertexResizer, &Vertices);
        reader->ReadCollection(indexReader, indexResizer, &Indices);
    }


    //Generates sample world information from scratch.
    void GenerateInfo(void)
    {
        VertexInputs = VertexPosUVNormal::GetVertexAttributes();

        //Generate a cube for the mesh.
        std::vector<VertexPosUVNormal> vertices;
        PrimitiveGenerator::GenerateCube(vertices, Indices, false, false);
        Vertices.resize(vertices.size());
        for (unsigned int i = 0; i < Vertices.size(); ++i)
            Vertices[i].Value = vertices[i];


        //Set up the mesh's material.

        //Vertex shader just converts vertices to screen space.
        RenderIOAttributes vertexInputs = VertexInputs;
        MaterialUsageFlags vertParams;
        vertParams.EnableFlag(MaterialUsageFlags::DNF_USES_WVP_MAT);
        std::string wvpMat = MaterialConstants::WVPMatName,
                    vIn_Pos = vertexInputs.GetAttribute(0).Name;
        VertexShader = MaterialConstants::GetVertexHeader("", vertexInputs, vertParams) +
"\n\
void main()                                                    \n\
{                                                              \n\
    gl_Position = " + wvpMat + " * vec4(" + vIn_Pos + ", 1.0); \n\
}";

        //Fragment shader oscillates between two colors.
        MaterialUsageFlags fragParams;
        fragParams.EnableFlag(MaterialUsageFlags::DNF_USES_TIME);
        std::string fragOutputs = "out vec4 fOut_SurfaceColor;";
        std::string time = MaterialConstants::ElapsedTimeName;
        FragmentShader = MaterialConstants::GetFragmentHeader("", fragOutputs, fragParams) +
"\n\
void main()                                                    \n\
{                                                              \n\
    float timeLerp = 0.5 + (0.5 * sin(" + time + "));          \n\
    fOut_SurfaceColor = mix(vec4(1.0, 0.0, 0.0, 1.0),          \n\
                            vec4(0.0, 1.0, 0.0, 1.0),          \n\
                            timeLerp);                         \n\
}";
    }

    //Converts this instance's vertex info list into actual vertices.
    void GetVertices(std::vector<VertexPosUVNormal>& outVertices)
    {
        outVertices.reserve(Vertices.size() + outVertices.size());
        for (unsigned int i = 0; i < Vertices.size(); ++i)
        {
            outVertices.push_back(Vertices[i].Value);
        }
    }
};



SerializationWorld::SerializationWorld(void)
    : windowSize(800, 600),
      SFMLOpenGLWorld(800, 600)
{
}

sf::VideoMode SerializationWorld::GetModeToUse(unsigned int windowW, unsigned int windowH)
{
    return sf::VideoMode(windowW, windowH);
}
std::string SerializationWorld::GetWindowTitle(void)
{
    return "SerializationWorld";
}
sf::Uint32 SerializationWorld::GetSFStyleFlags(void)
{
    return sf::Style::Titlebar | sf::Style::Resize | sf::Style::Close;
}
sf::ContextSettings SerializationWorld::GenerateContext(void)
{
    return sf::ContextSettings(24, 0, 0, 4, 1);
}


void SerializationWorld::InitializeWorld(void)
{
    SFMLOpenGLWorld::InitializeWorld();
    //If there was an error initializing the game, don’t bother with
    //    the rest of initialization.
    if (IsGameOver())
    {
        return;
    }


    //Set up the camera.
    const Vector3f camStartPos(7.0f, 7.0f, 7.0f);
    gameCam = MovingCamera(camStartPos, 8.0f, 0.1f, -camStartPos.Normalized());
    gameCam.Window = GetWindow();
    gameCam.PerspectiveInfo.SetFOVDegrees(65.0f);
    gameCam.PerspectiveInfo.Width = windowSize.x;
    gameCam.PerspectiveInfo.Height = windowSize.y;
    gameCam.PerspectiveInfo.zNear = 0.1f;
    gameCam.PerspectiveInfo.zFar = 50.0f;


    //Either generate the world info and save it to a file, or read it back from that file.
    WorldInfo info;
    const std::string filePath = "Content/Sample Worlds/Info.xml";
    const bool generateData = true;
    if (generateData)
    {
        info.GenerateInfo();

        //Write the info into "Dependencies/Include In Build/Universal/Content" using
        //    an XML file DataWriter.
        XmlWriter infoWriter;
        infoWriter.WriteDataStructure(info, "World info");
        std::string err = infoWriter.SaveData("../../Dependencies/Include In Build/Universal/" + filePath);
        if (!err.empty())
        {
            std::cout << "Error writing out world info: " << err << "\n";

            char dummy;
            std::cin >> dummy;

            EndWorld();
            return;
        }
    }
    else
    {
        //Load an XML file to read from.
        XmlReader infoReader(filePath);
        if (!infoReader.ErrorMessage.empty())
        {
            std::cout << "Error writing out world info: " << infoReader.ErrorMessage << "\n";

            char dummy;
            std::cin >> dummy;

            EndWorld();
            return;
        }

        //DataReaders use exceptions to catch errors. Error messages sit in their "ErrorMessage" field.
        try
        {
            infoReader.ReadDataStructure(info);
        }
        catch (int ex)
        {
            assert(ex == DataReader::EXCEPTION_FAILURE);
            std::cout << "Error reading in the world info: " << infoReader.ErrorMessage << "\n";

            char dummy;
            std::cin >> dummy;

            EndWorld();
            return;
        }
    }


    //Now use the world info to generate the mesh and material objects.

    mesh.SubMeshes.push_back(MeshData(false, PT_TRIANGLE_LIST));

    std::vector<VertexPosUVNormal> vertices;
    info.GetVertices(vertices);
    mesh.SubMeshes[0].SetVertexData(vertices, MeshData::BUF_STATIC,
                                    VertexPosUVNormal::GetVertexAttributes());

    mesh.SubMeshes[0].SetIndexData(info.Indices, MeshData::BUF_STATIC);

    mesh.Transform.SetScale(4.0f);

    std::string errorMsg;
    material = new Material(info.VertexShader, info.FragmentShader, params,
                            info.VertexInputs, BlendMode::GetOpaque(), errorMsg);
    if (!errorMsg.empty())
    {
        std::cout << "Error setting up material: " << errorMsg << "\n\nEnter anything to end the world.";
        char dummy;
        std::cin >> dummy;

        EndWorld();
        return;
    }
}
void SerializationWorld::OnWorldEnd(void)
{
    if (material != 0)
    {
        delete material;
    }
    mesh.SubMeshes.clear();
}

void SerializationWorld::UpdateWorld(float elapsedSeconds)
{
    gameCam.Update(elapsedSeconds);

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape))
    {
        EndWorld();
        return;
    }
}
void SerializationWorld::RenderOpenGL(float elapsedSeconds)
{
    //Set up rendering state.
    ScreenClearer(true, true, false, Vector4f(0.2, 0.2, 0.2f, 0.0f)).ClearScreen();
    RenderingState(RenderingState::C_NONE).EnableState();
    Viewport(0, 0, windowSize.x, windowSize.y).Use();

    //Set up the matrix transforms.
    Matrix4f viewMat, projMat;
    gameCam.GetViewTransform(viewMat);
    gameCam.GetPerspectiveProjection(projMat);

    //Render the geometry.
    RenderInfo info(GetTotalElapsedSeconds(), &gameCam, &viewMat, &projMat);
    material->Render(info, &mesh, params);
}


void SerializationWorld::OnInitializeError(std::string errorMsg)
{
    //Print the error message and end the game.
    SFMLOpenGLWorld::OnInitializeError(errorMsg);
    EndWorld();
}
void SerializationWorld::OnWindowResized(unsigned int newWidth, unsigned int newHeight)
{
    windowSize.x = newWidth;
    windowSize.y = newHeight;

    gameCam.PerspectiveInfo.Width = newWidth;
    gameCam.PerspectiveInfo.Height = newHeight;
}