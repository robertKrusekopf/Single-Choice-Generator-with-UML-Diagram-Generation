#include "converter.h"
#include <vector>
#include "parameter.h"
#include <fstream>
#include <iostream>
#include <array>
#include <memory>
#include <filesystem>
#include "picture.h"
#include "question.h"

using namespace std;

Diagram::Diagram(int Id, const vector<string> &d_uml)
{
    diaId = Id;
    uml = d_uml;
}

int Diagram::GetDiaId()
{
    return diaId;
}

vector<string> Diagram::GetUML()
{
    return uml;
}

// takes the vector of diagrams and creates a .txt file for every diagram-ID, which is then needed as input for Plantuml
int writeUML(vector<Diagram> diagrams, string path)
{
    string filename;
    // go to folder inputUML
    path += "\\inputUML";
    // no need to theck existence of directory, as it already has been deleted
    filesystem::create_directory(path);
    for (Diagram d : diagrams)
    {
        ofstream umlfile;
        // in folder inputUML go to file input1.txt etc.
        filename = path + "\\input";
        int c = d.GetDiaId();
        filename.append(to_string(c));
        filename.append(".txt");
        umlfile.open(filename);
        umlfile << "@startuml"
                << "\n";
        for (const auto &e : d.GetUML())
        {
            umlfile << e << "\n";
        }
        umlfile << "@enduml";
        umlfile.close();
    }
    return 0;
}

// opens plantuml for creation of images based on the input files created by writeUML; get cmd output from plantUML
string plantUMLcall(const char *plantUMLcommand)
{
    array<char, 128> buffer;
    string result;
    // open Plantuml cmd
    auto pipe = popen(plantUMLcommand, "r");

    if (!pipe)
        throw runtime_error("popen() fail");

    while (!feof(pipe))
    {
        // get console output
        if (fgets(buffer.data(), buffer.size(), pipe) != nullptr)
            result += buffer.data();
    }

    auto rc = pclose(pipe);

    if (rc == EXIT_SUCCESS)
    {
    }
    else if (rc == EXIT_FAILURE)
    {
        cout << "Exit Failure";
    }
    return result;
}

// adds UML pngs to Picture vector with dummy dimensions before real dimensions are set by setPNGdimensions
vector<Picture> addUMLtoPictures(vector<Diagram> diagrams, vector<Picture> pictures)
{
    Picture picture{};
    int i = 1;
    for (Diagram d : diagrams)
    {
        string filename = "input";
        filename.append(to_string(i));
        filename.append(".png");
        picture.SetAll(filename, "100", "100", i);
        i++;
        pictures.push_back(picture);
    };
    return pictures;
}

// read dimensions of png files and set correct height and width of pictures in questions
void setPNGdimensions(string filename, vector<Question> &questions)
{
    int QuestionID = 0;
    for (Question q : questions)
    {
        int height, width;
        //create filename copy, because filenames are alway appended
        string filenamecopy = filename;
        filenamecopy.append("\\input");
        filenamecopy.append(to_string(QuestionID + 1));
        filenamecopy.append(".png");

        unsigned char buf[8];
        std::ifstream in(filenamecopy);
        in.seekg(16);
        in.read(reinterpret_cast<char *>(&buf), 8);

        // get height and width and add to questions vector
        height = (buf[0] << 24) + (buf[1] << 16) + (buf[2] << 8) + (buf[3] << 0);
        width = (buf[4] << 24) + (buf[5] << 16) + (buf[6] << 8) + (buf[7] << 0);
        questions[QuestionID].picture.SetAll(filenamecopy, to_string(height), to_string(width), QuestionID);
        QuestionID++;
    }
}

// deletes all folders created by previous program runs
void deleteFolders(const string &path, string subfoldername)
{
    filesystem::remove_all(path + "\\inputPictures");
    filesystem::remove_all(path + "\\inputUML");
    // overwrite outputs of same questionpoolid, keep outputs for other questionpoolids
    filesystem::remove_all(path + "\\output\\" + subfoldername);
}