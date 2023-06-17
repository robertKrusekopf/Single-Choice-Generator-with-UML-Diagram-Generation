#include <iostream>
#include <cassert>
#include <vector>
#include <fstream>
#include <filesystem>
#include "converter.h"
#include "parameter.h"
#include "picture.h"
#include "question.h"

void testDiagram()
{
    std::vector<std::string> uml = {"ClassA", "ClassB"};
    Diagram d(1, uml);

    assert(d.GetDiaId() == 1);
    assert(d.GetUML() == uml);
}

void testWriteUML()
{
    std::vector<Diagram> diagrams;
    diagrams.push_back(Diagram(1, {"ClassA"}));
    diagrams.push_back(Diagram(2, {"ClassB"}));

    std::string outputPath = "output";

    int result = writeUML(diagrams, outputPath);
    assert(result == 0);

    for (const Diagram& d : diagrams)
    {
        std::string filename = outputPath + "/inputUML/input" + std::to_string(d.GetDiaId()) + ".txt";
        std::ifstream file(filename);
        assert(file.good());
        file.close();
    }
    std::filesystem::remove_all(outputPath + "/inputUML");
}

void testPlantUMLCall()
{
    std::string command = "plantuml -v";
    std::string output = plantUMLcall(command.c_str());

    assert(!output.empty());
}

void testAddUMLtoPictures()
{
    std::vector<Diagram> diagrams;
    diagrams.push_back(Diagram(1, {"ClassA"}));
    diagrams.push_back(Diagram(2, {"ClassB"}));

    std::vector<Picture> pictures;

    pictures = addUMLtoPictures(diagrams, pictures);

    assert(pictures.size() == diagrams.size());
    for (size_t i = 0; i < pictures.size(); i++)
    {
        assert(pictures[i].GetFileName() == "input" + std::to_string(i + 1) + ".png");
        assert(pictures[i].GetHeight() == "100");
        assert(pictures[i].GetWidth() == "100");
        assert(pictures[i].GetQuestionID() == i + 1);
    }
}

void testSetPNGdimensions()
{
    std::vector<Question> questions;
    questions.push_back(Question());
    questions.push_back(Question());

    std::string filename = "inputPictures";
    std::filesystem::create_directory(filename);

    std::ofstream file1(filename + "/input1.png", std::ios::binary);
    std::ofstream file2(filename + "/input2.png", std::ios::binary);
    file1.close();
    file2.close();

    setPNGdimensions(filename, questions);

    assert(questions[0].picture.GetFileName() == filename + "/input1.png");
    assert(questions[0].picture.GetHeight() == "100");
    assert(questions[0].picture.GetWidth() == "100");
    assert(questions[0].picture.GetQuestionID() == 0);

    assert(questions[1].picture.GetFileName() == filename + "/input2.png");
    assert(questions[1].picture.GetHeight() == "100");
    assert(questions[1].picture.GetWidth() == "100");
    assert(questions[1].picture.GetQuestionID() == 1);

    std::filesystem::remove_all(filename);
}

void testDeleteFolders()
{
    std::string path = "path/to/folders";
    std::string subfoldername = "subfolder";

    deleteFolders(path, subfoldername);

    assert(!std::filesystem::exists(path + "/inputPictures"));
    assert(!std::filesystem::exists(path + "/inputUML"));
    assert(!std::filesystem::exists(path + "/output/" + subfoldername));
}

int main()
{
    testDiagram();
    testWriteUML();
    testPlantUMLCall();
    testAddUMLtoPictures();
    testSetPNGdimensions();
    testDeleteFolders();

    std::cout << "All tests passed!" << std::endl;

    return 0;
}