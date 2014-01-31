#include "SFML/Graphics.hpp"
#include "NoiseTest.h"
#include "TerrainWorld.h"
#include "OpenGLTestWorld.h"
#include "TwoTrianglesWorld.h"

#include "IO/IOTest.h"
using namespace tinyxml2;

#include <iostream>
using namespace std;


void PauseConsole(void)
{
    char dummy;
    cout << "Enter any character to continue.\n";
    cin >> dummy;
}

void RunIOTest(void)
{
    IOTester tester;


    //Try to open the document.
    XMLDocument doc;
    XMLError error = doc.LoadFile("Test Files/TestIOData.xml");
    if (error != 0)
    {
        cout << "Error reading xml document: " << error << "\n";
        PauseConsole();
        return;
    }
    else
    {
        cout << "Document opened successfully.\n";
    }


    //Try to create the serializer.
    DataSerializer rootSer(doc.FirstChild()->ToElement(), doc);
    if (rootSer.HasError())
    {
        cout << "Error initializing rootSer: " << rootSer.GetErrorMsg().c_str() << "\n";
        PauseConsole();
        return;
    }
    else
    {
        cout << "rootSer created successfully.\n";
    }


    //Try to read the class data.
    if (!rootSer.ReadClass("MyIOData", &tester))
    {
        cout << "Error reading tester: " << rootSer.GetErrorMsg().c_str() << "\n";
        PauseConsole();
        return;
    }
    else
    {
        cout << "tester read in successfully.\n";
    }


    //Read in the class data.
    cout << "\n\n---------------------------------\n\n" <<
        "tester.Float = " << tester.Float << "\n" <<
        "tester.Int = " << tester.Int << "\n" <<
        "tester.Bool = " << tester.Bool << "\n" <<
        "tester.String = " << tester.String.c_str() << "\n" <<
        "tester.MiniMe = \n\t";
    for (int i = 0; i < (sizeof(tester.MiniMe.Floats) / sizeof(float)); ++i)
    {
        if (i > 0) cout << ", ";
        cout << tester.MiniMe.Floats[i];
    }
    cout << "\n\n----------------------------\n\n";

    PauseConsole();


    //Output the class data to another file.
    cout << "\n\nOutputting class...\n\n";
    doc.Clear();
    doc.InsertEndChild(doc.NewElement("rootNode"));
    DataSerializer newRootSer = DataSerializer(doc.FirstChild()->ToElement(), doc);
    if (newRootSer.HasError())
    {
        cout << "Error clearing document and creating newRootSer: \"" << newRootSer.GetErrorMsg() << "\"\n";
        PauseConsole();
        return;
    }
    if (!newRootSer.WriteClass("MyIOData", tester))
    {
        cout << "Error writing values to document: \"" << newRootSer.GetErrorMsg() << "\"\n";
        PauseConsole();
        return;
    }
    error = doc.SaveFile("Test Files/TestIOData2.xml");
    if (error != 0)
    {
        cout << "Error saving output XML: " << error << "\n";
        PauseConsole();
        return;
    }
    else
    {
        cout << "Done!\n\n";
        PauseConsole();
    }


    //Try to overwrite the data this time.
    cout << "Trying to overwrite document...\n";
    tester.Float += 5.0f;
    tester.Int += 44444;
    tester.Bool = !tester.Bool;
    tester.String += " WAAAAAAT";
    tester.MiniMe.Floats[0] = 3.14159f;
    if (!newRootSer.WriteClass("MyIOData", tester))
    {
        cout << "Error overwriting values to document: \"" << newRootSer.GetErrorMsg() << "\"\n";
        PauseConsole();
        return;
    }
    
    //Try to add a second class.
    tester.String = "HAHAHAHAHAHAHAHAHAHAH";
    if (!newRootSer.WriteClass("MySecondIOData", tester))
    {
        cout << "Error adding second value to document: \"" << newRootSer.GetErrorMsg() << "\"\n";
        PauseConsole();
        return;
    }
    error = doc.SaveFile("Test Files/TestIOData2.xml");
    if (error != 0)
    {
        cout << "Error saving output XML: " << error << "\n";
        PauseConsole();
        return;
    }


    //Done!
    cout << "\n\nAll done!\n";
    PauseConsole();
}


int main()
{
    //NoiseTest().RunWorld();
    //TerrainWorld().RunWorld();
    //OpenGLTestWorld().RunWorld();

    //TwoTrianglesWorld().RunWorld();

    RunIOTest();
}