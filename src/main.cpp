/*
Template-based generator for single choice question with XML output for Ilias
Created By Nico Willert, April 2022
*/

#include <cstdlib>
#include <iostream>
#include <string>
#include <limits>
#include <vector>
#include <sstream>
#include <numeric>
#include <ctime>
#include <cmath>
#include <fstream>
#include <filesystem>
#include <algorithm>
#include <random>
#include <iterator>
#include <regex>

// Header Files of the different classes and pulled out Functions
#include "picture.h"
#include "parameter.h"
#include "randomNumberFunction.h"
#include "question.h"
#include "vectorHelperFunctions.h"
#include "converter.h"

using namespace std;
namespace fs = std::filesystem;

const string qpl = "qpl_";
const string qti = "qti_";
// gets replaced by argv later
string ILIASfolderMarking = "1644584918__0__";
int ILIASquestionID = 482009;
int ILIASpictureID = 1;

// replaces all occurances of a given string search with a string replace in another bigger string
void replaceAll(string &s, const string &search, const string &replace)
{
    for (size_t pos = 0;; pos += replace.length())
    {
        // Locate the substring to replace
        pos = s.find(search, pos);
        if (pos == string::npos)
            break;
        // Replace by erasing and inserting
        s.erase(pos, search.length());
        s.insert(pos, replace);
    }
}

// Create all Permutations of ParameterValues, each Parameter and value exists a pair, for each complete Permuation a vector with pairs is created.
void createAllParameters(int currentPlace, vector<vector<pair<string, string>>> allParameterPairs, vector<vector<pair<string, string>>> &validParameterPairs, vector<pair<string, string>> &vecForRecursion)
{

    for (int i = 0; i < allParameterPairs.at(currentPlace).size(); ++i)
    {
        vecForRecursion.push_back(allParameterPairs.at(currentPlace).at(i));
        if (allParameterPairs.size() == currentPlace + 1)
        {
            validParameterPairs.push_back(vecForRecursion);
        }
        else
        {
            createAllParameters(currentPlace + 1, allParameterPairs, validParameterPairs, vecForRecursion);
        }
        vecForRecursion.pop_back();
    }
}

// check for all pairs in a vector, if there exists a an interaction so that it is not a valid permutation
bool checkValidParameter(vector<pair<string, string>> v, vector<tuple<string, string, string, string>> interactions)
{
    for (pair p : v)
    {
        if (isPairInVectorTupleZeroOne(p, interactions))
        {
            vector<pair<string, string>> allInfluenced;
            vector<string> influencedParameter;
            for (tuple t : interactions)
            {
                if (get<0>(p) == get<0>(t) && get<1>(p) == get<1>(t))
                {
                    allInfluenced.push_back(make_pair(get<2>(t), get<3>(t)));
                    if (!isInVector(influencedParameter, get<2>(t)))
                    {
                        influencedParameter.push_back(get<2>(t));
                    }
                }
            }
            for (string s : influencedParameter)
            {
                bool hasOneTrueInfluence = false;
                for (pair p_parameterpair : allInfluenced)
                {
                    if (s == get<0>(p_parameterpair))
                    {
                        for (pair p_Setpair : v)
                        {
                            if (get<0>(p_Setpair) == get<0>(p_parameterpair) && get<1>(p_Setpair) == get<1>(p_parameterpair))
                            {
                                hasOneTrueInfluence = true;
                            }
                        }
                    }
                }
                if (hasOneTrueInfluence == false)
                {
                    return false;
                }
            }
        }
    }
    return true;
}

bool questionExists(Question q, vector<Question> questions)
{
    for (Question t : questions)
    {
        if ((q.GetName() == t.GetName()) && (q.GetCode() == t.GetCode()) && (q.GetAdditionalText() == t.GetAdditionalText()) &&
            (q.GetTask() == t.GetTask()) && (q.GetCorrectAnswer() == t.GetCorrectAnswer()) && (q.GetWrongAnswerOne() == t.GetWrongAnswerOne()) &&
            (q.GetWrongAnswerTwo() == t.GetWrongAnswerTwo()) && (q.GetWrongAnswerThree() == t.GetWrongAnswerThree()) &&
            (q.GetPicture().GetName() == t.GetPicture().GetName()))
        {
            cout << "\tExactly Duplicated question was ignored: " << q.GetTask() << endl;
            return true;
        }
    }
    return false;
}

bool stillParametersInTexts(vector<Parameter> parametersVector, string correctAnswer, string wrongAnswerOne, string wrongAnswerTwo, string wrongAnswerThree, string codeToSet, string taskToSet, string additionalTextToSet)
{
    /*
    parametersVector,string correctAnswer, string wrongAnswerOne,
    string wrongAnswerTwo, string wrongAnswerThree, string codeToSet,
    string taskToSet, string additionalTextToSet
    */
    for (Parameter p : parametersVector)
    {
        if ((correctAnswer.find(p.GetName()) != std::string::npos) || (wrongAnswerOne.find(p.GetName()) != std::string::npos) || (wrongAnswerTwo.find(p.GetName()) != std::string::npos) ||
            (wrongAnswerThree.find(p.GetName()) != std::string::npos) || (codeToSet.find(p.GetName()) != std::string::npos) || (taskToSet.find(p.GetName()) != std::string::npos) || (additionalTextToSet.find(p.GetName()) != std::string::npos))
        {
            cout << "Recursive Use of Parameter: " << p.GetName() << endl;
            return true;
        }
    }
    return false;
}

int main(int argc, char **argv)
{

    // Replacements for xml generations
    vector<pair<string, string>> replacementsForXmlIlias{};
    replacementsForXmlIlias.push_back(make_pair("&", "&amp;"));
    replacementsForXmlIlias.push_back(make_pair("<", "&lt;"));
    replacementsForXmlIlias.push_back(make_pair(">", "&gt;"));
    replacementsForXmlIlias.push_back(make_pair("\"", "&quot;"));
    replacementsForXmlIlias.push_back(make_pair("\'", "&apos;"));

    replacementsForXmlIlias.push_back(make_pair("$nl", "</p><p>"));
    replacementsForXmlIlias.push_back(make_pair("$b", "<strong>"));
    replacementsForXmlIlias.push_back(make_pair("$/b", "</strong>"));
    replacementsForXmlIlias.push_back(make_pair("$i", "<em>"));
    replacementsForXmlIlias.push_back(make_pair("$/i", "</em>"));
    replacementsForXmlIlias.push_back(make_pair("$u", "<u>;"));
    replacementsForXmlIlias.push_back(make_pair("$/u", "</u>"));
    replacementsForXmlIlias.push_back(make_pair("$c", "<code>"));
    replacementsForXmlIlias.push_back(make_pair("$/c", "</code>"));
    replacementsForXmlIlias.push_back(make_pair("$p", "<pre style=\"display: inline-block; \">"));
    replacementsForXmlIlias.push_back(make_pair("$/p", "</pre>"));
    replacementsForXmlIlias.push_back(make_pair("&lt;br/&gt;", "<br/>"));
    replacementsForXmlIlias.push_back(make_pair("$lc", "<br/>"));
    replacementsForXmlIlias.push_back(make_pair("$w", ""));

    vector<pair<string, string>> replacementsForXmlIliasSpecialCode{};
    replacementsForXmlIliasSpecialCode.push_back(make_pair("&", "&amp;amp;"));
    replacementsForXmlIliasSpecialCode.push_back(make_pair("<", "&amp;lt;"));
    replacementsForXmlIliasSpecialCode.push_back(make_pair(">", "&amp;gt;"));
    replacementsForXmlIliasSpecialCode.push_back(make_pair("\"", "&amp;quot;"));
    replacementsForXmlIliasSpecialCode.push_back(make_pair("\'", "&amp;apos;"));
    replacementsForXmlIliasSpecialCode.push_back(make_pair("&amp;lt;br/&amp;gt;", "<br/>"));
    replacementsForXmlIliasSpecialCode.push_back(make_pair("$w", ""));

    vector<pair<string, string>> replacementsForXmlIliasSpecialAdditionalText{};
    replacementsForXmlIliasSpecialAdditionalText.push_back(make_pair("&", "&amp;amp;"));
    replacementsForXmlIliasSpecialAdditionalText.push_back(make_pair("<", "&amp;lt;"));
    replacementsForXmlIliasSpecialAdditionalText.push_back(make_pair(">", "&amp;gt;"));
    replacementsForXmlIliasSpecialAdditionalText.push_back(make_pair("$nl", "<br/>"));
    replacementsForXmlIliasSpecialAdditionalText.push_back(make_pair("$b", "<Strong>"));
    replacementsForXmlIliasSpecialAdditionalText.push_back(make_pair("$/b", "</Strong>"));
    replacementsForXmlIliasSpecialAdditionalText.push_back(make_pair("$i", "<Emph>"));
    replacementsForXmlIliasSpecialAdditionalText.push_back(make_pair("$/i", "</Emph>"));
    replacementsForXmlIliasSpecialAdditionalText.push_back(make_pair("$u", "<Important>"));
    replacementsForXmlIliasSpecialAdditionalText.push_back(make_pair("$/u", "</Important>"));
    replacementsForXmlIliasSpecialAdditionalText.push_back(make_pair("$c", "<Code>"));
    replacementsForXmlIliasSpecialAdditionalText.push_back(make_pair("$/c", "</Code>"));
    replacementsForXmlIliasSpecialAdditionalText.push_back(make_pair("$w", ""));

    vector<pair<string, string>> replacementsForXmlMoodle{};
    replacementsForXmlMoodle.push_back(make_pair("$nl", "</p><p>"));
    replacementsForXmlMoodle.push_back(make_pair("$b", "<strong>"));
    replacementsForXmlMoodle.push_back(make_pair("$/b", "</strong>"));
    replacementsForXmlMoodle.push_back(make_pair("$i", "<em>"));
    replacementsForXmlMoodle.push_back(make_pair("$/i", "</em>"));
    replacementsForXmlMoodle.push_back(make_pair("$u", "<u>"));
    replacementsForXmlMoodle.push_back(make_pair("$/u", "</u>   "));
    replacementsForXmlMoodle.push_back(make_pair("$c", "<code>"));
    replacementsForXmlMoodle.push_back(make_pair("$/c", "</code>"));
    replacementsForXmlMoodle.push_back(make_pair("$p", "<pre>"));
    replacementsForXmlMoodle.push_back(make_pair("$/p", "</pre>"));
    replacementsForXmlMoodle.push_back(make_pair("$w", ""));
    // End Replacements for xml generations

    // TODO: Dummy entfernen, wenn über Python ausgeführt mit Argumenten dummy = 0; wenn nur c++ Dummy = 1
    int dummy = 0;
    if (argc == 1)
    {
        dummy = 1;
    }

    // cout << "dummy: " << dummy << endl only for c++ debugging when no arguments from GUI are available

    // Creating global Variable for the input and output parts
    string path = "";
    string pathInput = "";
    string pictureInputpath = "";
    string outputFolder = "";
    string plantUMLcommand = "";

    

    string questionPoolId{};
    string qpTitle{};
    string taxId{};
    string taxTitle{};

    // Id's needed for the writing of the taxonomy levels.
    // The actual Id's dont matter so these are just dummies used, ILIAS will overwrite them after import
    int childId{}, parentId{};
    childId = 15001;
    parentId = 15000;

    int numberOfExams{};
    int currentQuestionId{1};
    int currentDiaId{1};
    int currentPictureId{1};
    // Vectors to work with
    // for all Pictures, to Copy them in the end in their respective folder
    vector<Picture> pictures;
    // All Questions that will be created during the process
    vector<Question> questions;
    vector<Diagram> diagrams;
    // All Taxonomy levels that are present in the templates.
    vector<string> taxonomyLevels;

    int mode{};
    int mode2{};
    int MultipleUML = 0;

    // cout << "Which Output is be generated? Enter 1 for IliasXml and 2 for MoodleXml: ";
    if (dummy == 1)
    {
        mode2 = 1;
        ILIASfolderMarking = "1644584918__0__";
        ILIASquestionID = 482009;
    }
    else
    {
        mode2 = atoi(argv[1]);
        ILIASfolderMarking = argv[8];
        ILIASquestionID = atoi(argv[9]);
    }

    if (mode2 == 1)
    {
        // cout << "Please input the question pool id: ";
        // cin >> questionPoolId;
        if (dummy == 1)
        {
            questionPoolId = "48611";
        }
        else
        {
            questionPoolId = argv[2];
        }

        // cout << "Please input the question pool title: ";
        // cin >> qpTitle;
        if (dummy == 1)
        {
            qpTitle = "0";
        }
        else
        {
            qpTitle = argv[3];
        }

        // out << "Please input the Taxonomy Title: ";
        // cin >> taxTitle;
        if (dummy == 1)
        {
            taxTitle = "0";
        }
        else
        {
            taxTitle = argv[4];
        }
    }
    if (mode2 == 3)
    {
        questionPoolId = "asd";

        qpTitle = "asd";

        taxTitle = "asd";
    }

    // cout << "Which Mode is to start? Enter 1 for Random and 2 for All: ";
    // cin >> mode;
    if (dummy == 1)
    {
        mode = 1;
    }
    else
    {
        mode = atoi(argv[5]);
    }

    if (mode == 1)
    {
        // cout << "Please input, how many versions should be generated: ";
        // cin >> numberOfExams;
        if (dummy == 1)
        {
            numberOfExams = 10;
            MultipleUML = 1;
        }
        else
        {
            numberOfExams = atoi(argv[6]);
            MultipleUML = atoi(argv[10]);
        }
    }

    // numberOfExams = 5;
    // questionPoolId = "34568";
    // qpTitle = "TestTitle";
    taxId = "42069";
    // taxTitle = "TaxTest";

    // Parsing Files in Input Folder, each file is seperatly handled and
    // based on the parsed template the questions will be created into the vector questions
    //  "Basierend auf verschiedenen Sektionen in der Schablone werden die unterschiedlichen Parts eingelesen um sie weiter zu verarbeiten. (Line 267-548)"

    // Name of subfolder in outputfolder
    string subfoldername = ILIASfolderMarking + qpl + questionPoolId;
        path = argv[7];

        pathInput = path.append("\\input");
        pictureInputpath = argv[7];
        pictureInputpath.append("\\inputPictures");

        outputFolder = argv[7];
        outputFolder.append("\\output");
        const fs::path outputPath{outputFolder};

        plantUMLcommand = "java -jar plantuml.jar \"";
        plantUMLcommand.append(argv[7]);
        plantUMLcommand.append("\\inputUML\" -o \"");
        plantUMLcommand.append(argv[7]);
        plantUMLcommand.append("\\inputPictures\"");
        // delete Folders created from previous program runs
        deleteFolders(argv[7], subfoldername);


    for (const auto &entry : fs::directory_iterator(pathInput))
    {
        // all Variables that need to be refreshed after each file
        ifstream readFromFile;
        string name{}, author{}, date{}, version{}, description{}, taxonomy{}, task{}, code{}, additionalText{};
        string taskToSet{}, codeToSet{}, additionalTextToSet{};
        vector<Parameter> parameters{};
        vector<string> correctAnswers{};
        vector<string> wrongAnswers{};
        vector<vector<string>> uml{};
        vector<vector<string>> umlsOriginal{};
        vector<tuple<string, string, string, string>> interactions{};
        vector<pair<string, string>> exclusions{};
        Picture picture{};

        string txtFromFile = "";

        string filename = entry.path().string();
        readFromFile.open(filename, ios_base::in);
        cout << "Trying to read " << filename << endl
             << endl;
        if (readFromFile.is_open())
        {
            while (readFromFile.good())
            {
                char c = readFromFile.peek();
                if (c == '@')
                {
                    getline(readFromFile, txtFromFile);
                    if (txtFromFile == "@NAME")
                    {
                        getline(readFromFile, name);
                    }
                    else if (txtFromFile == "@AUTHOR")
                    {
                        getline(readFromFile, author);
                    }
                    else if (txtFromFile == "@DATE")
                    {
                        getline(readFromFile, date);
                    }
                    else if (txtFromFile == "@VERSION")
                    {
                        getline(readFromFile, version);
                    }
                    else if (txtFromFile == "@DESCRIPTION")
                    {
                        getline(readFromFile, description);
                    }
                    else if (txtFromFile == "@ADDITIONALTEXT")
                    {
                        getline(readFromFile, additionalText);
                    }
                    else if (txtFromFile == "@CODE")
                    {
                        string tempLine{};
                        while (readFromFile.peek() != '@' && readFromFile.good())
                        {
                            getline(readFromFile, tempLine);
                            code += tempLine;
                            code += "<br/>";
                        }
                    }
                    else if (txtFromFile == "@PICTURE")
                    {
                        /*
                        string pictureName, pictureHeight, pictureWidth;
                        getline(readFromFile, pictureName);
                        getline(readFromFile, pictureWidth);
                        getline(readFromFile, pictureHeight);
                        picture.SetAll(pictureName, pictureWidth, pictureHeight, currentPictureId);
                        ++currentPictureId;
                        if (!isPictureInVector(pictures, pictureName))
                        {
                            pictures.push_back(picture);
                        }
                        */
                    }
                    else if (txtFromFile == "@TAXONOMY")
                    {
                        getline(readFromFile, taxonomy);
                    }
                    else if (txtFromFile == "@PARAMETER")
                    {
                        while (readFromFile.peek() == '$')
                        {
                            string parameterString;
                            getline(readFromFile, parameterString);
                            Parameter p(parameterString);
                            parameters.push_back(p);
                        }
                    }
                    else if (txtFromFile == "@VALUERANGE")
                    {
                        int position = 0;
                        string tempLine{};
                        string temp{};
                        while (readFromFile.peek() != '@' && readFromFile.good())
                        {
                            getline(readFromFile, tempLine);
                            // check if the ValueRange has a appropriate Parameter for it.
                            if (position < parameters.size())
                            {
                                istringstream is;
                                if (!tempLine.empty())
                                {
                                    is.str(tempLine);
                                    while (is)
                                    {
                                        string whitespace = " ";
                                        while (is.peek() > 32)
                                        {
                                            temp += is.get();
                                        }
                                        is.get();
                                        if (is.peek() == '#')
                                        {
                                            parameters[position].push_backValueRange(temp);
                                            is.get();
                                            temp = "";
                                        }
                                        else if (!temp.empty() && is.peek() != EOF)
                                        {
                                            temp += whitespace;
                                        }
                                        if (is.peek() == '@' || is.peek() == EOF)
                                        {
                                            if (temp != "")
                                            {
                                                parameters[position].push_backValueRange(temp);
                                                temp = "";
                                            }
                                        }
                                    }
                                    ++position;
                                }
                            }
                            else
                            {
                                if (!tempLine.empty())
                                {
                                    cout << "\tThere is no Parameter for the Value Range: " << tempLine << endl;
                                }
                            }
                        }
                    }
                    else if (txtFromFile == "@INTERACTION")
                    {
                        int line = 0;
                        int position = 0;
                        string tempLine{};
                        string temp{};
                        string parameterName{}, parameterValue{}, parameterTwoName{}, parameterTwoValue{};
                        while (readFromFile.peek() != '@' && readFromFile.good())
                        {
                            istringstream is;
                            getline(readFromFile, tempLine);
                            if (!tempLine.empty())
                            {
                                is.str(tempLine);
                                string whitespace = " ";
                                while (is.peek() > 32)
                                {
                                    temp += is.get();
                                }
                                is.get();
                                parameterName = temp;
                                temp = "";
                                // Check if parameter name exists
                                if (isInVector(parameters, parameterName))
                                {
                                    do
                                    {
                                        while (is.peek() > 32)
                                        {
                                            temp += is.get();
                                        }
                                        is.get();
                                        if (is.peek() != '$')
                                        {
                                            temp += whitespace;
                                        }
                                    } while (is.peek() != '$');
                                    parameterValue = temp;
                                    temp = "";
                                    // Check if parameter Value is in Value Range
                                    if (isValueForParameter(parameters, parameterName, parameterValue))
                                    {
                                        while (is.peek() > 32)
                                        {
                                            temp += is.get();
                                        }
                                        is.get();
                                        parameterTwoName = temp;
                                        temp = "";
                                        // Check if Parameter Two exists and is not Parameter One
                                        if (isInVector(parameters, parameterTwoName) && (parameterName != parameterTwoName))
                                        {
                                            while (is)
                                            {
                                                while (is.peek() > 32)
                                                {
                                                    temp += is.get();
                                                }
                                                is.get();
                                                if (is.peek() == '#' || is.peek() < 32)
                                                {
                                                    // make pair pushback into vector
                                                    is.get();
                                                    parameterTwoValue = temp;
                                                    if (isValueForParameter(parameters, parameterTwoName, parameterTwoValue))
                                                    {
                                                        interactions.push_back(make_tuple(parameterName, parameterValue, parameterTwoName, parameterTwoValue));
                                                        ++position;
                                                    }
                                                    else
                                                    {
                                                        cout << "\tIn the Interaction number: " << line + 1 << " the used dependent ParameterValue: " << parameterTwoValue << " was not defined for the Parameter: " << parameterTwoName << ". The InteractionValue was skipped" << endl;
                                                    }
                                                    temp = "";
                                                }
                                                else if (!temp.empty())
                                                {
                                                    temp += whitespace;
                                                }
                                            }
                                            if (temp != "")
                                            {
                                                // make pair pushback vector
                                                parameterTwoValue = temp;
                                                if (isValueForParameter(parameters, parameterTwoName, parameterTwoValue))
                                                {
                                                    interactions.push_back(make_tuple(parameterName, parameterValue, parameterTwoName, parameterTwoValue));
                                                }
                                                else
                                                {
                                                    cout << "\tIn the Interaction number: " << line + 1 << " the used dependent ParameterValue: " << parameterTwoValue << " was not defined for the Parameter: " << parameterTwoName << ". The InteractionValue was skipped" << endl;
                                                }
                                                temp = "";
                                            }
                                        }
                                        else if (parameterName == parameterTwoName)
                                        {
                                            cout << "\tIn the Interaction number: " << line + 1 << " the dependent Parameter: " << parameterTwoName << " is the same as the influencing Parameter: " << parameterName << ". The Interaction was skipped" << endl;
                                        }
                                        else
                                        {
                                            cout << "\tIn the Interaction number: " << line + 1 << " the dependent Parameter: " << parameterTwoName << " was not defined. The Interaction was skipped" << endl;
                                        }
                                    }
                                    else // Error Value is not in Value Range for the parameter
                                    {
                                        cout << "\tIn the Interaction number: " << line + 1 << " the used influencing ParameterValue: " << parameterValue << " was not defined for the Parameter: " << parameterName << ". The Interaction was skipped" << endl;
                                    }
                                }
                                else // Error ParameterOne is not in parameters
                                {
                                    cout << "\tIn the Interaction number: " << line + 1 << " the used influencing Parameter: " << parameterName << " was not defined. The Interaction was skipped" << endl;
                                }
                            }
                            ++line;
                        }
                    }
                    else if (txtFromFile == "@TASK")
                    {
                        getline(readFromFile, task);
                    }
                    else if (txtFromFile == "@TRUE")
                    {
                        while (readFromFile.peek() != '@' && readFromFile.good())
                        {
                            string correctAnswerString;
                            getline(readFromFile, correctAnswerString);
                            if (!correctAnswerString.empty())
                            {
                                correctAnswers.push_back(correctAnswerString);
                            }
                        }
                    }
                    else if (txtFromFile == "@FALSE")
                    {
                        while (readFromFile.peek() != '@' && readFromFile.good())
                        {
                            string wrongAnswerString;
                            getline(readFromFile, wrongAnswerString);
                            if (!wrongAnswerString.empty())
                            {
                                wrongAnswers.push_back(wrongAnswerString);
                            }
                        }
                    }
                    // read in multiple Diagram-Versions; into a 2d vector
                    else if (txtFromFile.find("@STARTUML") != string::npos)
                    {
                        // umlsOriginal contains multiple umlSingles
                        vector<string> umlSingle{};
                        while (readFromFile.peek() != '@' && readFromFile.good())
                        {
                            string umlString;
                            getline(readFromFile, umlString);
                            if (!umlString.empty())
                            {
                                umlSingle.push_back(umlString);
                            }
                        }
                        umlsOriginal.push_back(umlSingle);
                    }
                    else if (txtFromFile == "@EXCLUSIONS")
                    {
                        // c1-cn-1 for correct & w1-wn-1 for wrong
                        while (readFromFile.good())
                        {
                            string tempLine{}, temp{};
                            string tempOne{}, tempTwo{};
                            istringstream is;
                            getline(readFromFile, tempLine);
                            is.str(tempLine);
                            if (!tempLine.empty())
                            {
                                while (is)
                                {
                                    while (is.peek() > 32)
                                    {
                                        temp += is.get();
                                    }
                                    is.get();
                                    tempOne = temp;
                                    temp = "";
                                    while (is.peek() > 32)
                                    {
                                        temp += is.get();
                                    }
                                    is.get();
                                    temp = "";
                                    while (is)
                                    {
                                        temp += is.get();
                                    }
                                    tempTwo = temp;
                                    tempOne[1] = tempOne[1] - 1;
                                    tempTwo[1] = tempTwo[1] - 1;
                                    exclusions.push_back(make_pair(tempOne, tempTwo));
                                }
                            }
                        }
                    }
                    else
                    {
                        cout << "\tThere is something wrong with the Config" << endl;
                        cout << "\tSection: " << txtFromFile << " was not specialised by the generator." << endl;
                    }
                }
                else
                {
                    getline(readFromFile, txtFromFile);
                }
            }
            readFromFile.close();
        }
        cout << "Parsed " << filename << endl
             << endl;

        //"Anschließend  werden alle möglichen und validen Antwortkombinationen und Parameterkombinationen gebildet.(...-660)"
        // Combinatorics, just building all combinations for correct and false answers
        vector<tuple<int, int, int, int>> possibleCombinations;
        for (int i = 0; i < correctAnswers.size(); ++i)
        {
            for (int j = 0; j < wrongAnswers.size(); ++j)
            {
                for (int k = 0; k < wrongAnswers.size(); ++k)
                {
                    for (int l = 0; l < wrongAnswers.size(); ++l)
                    {
                        possibleCombinations.push_back(make_tuple(i, j, k, l));
                    }
                }
            }
        }

        // Delete Where same answers
        possibleCombinations.erase(std::remove_if(possibleCombinations.begin(), possibleCombinations.end(), [](const tuple<int, int, int, int> &n) -> bool
                                                  { return get<1>(n) == get<2>(n); }),
                                   possibleCombinations.end());

        possibleCombinations.erase(std::remove_if(possibleCombinations.begin(), possibleCombinations.end(), [](const tuple<int, int, int, int> &n) -> bool
                                                  { return get<1>(n) == get<3>(n); }),
                                   possibleCombinations.end());

        possibleCombinations.erase(std::remove_if(possibleCombinations.begin(), possibleCombinations.end(), [](const tuple<int, int, int, int> &n) -> bool
                                                  { return get<2>(n) == get<3>(n); }),
                                   possibleCombinations.end());

        // Delete based on Exclusions criteria from @EXCLUSION Part of the question template
        if (exclusions.size() != 0)
        {
            for (pair n : exclusions)
            {
                string firstInPair(1, n.first[1]);
                string secondInPair(1, n.second[1]);
                if (n.first[0] == 'c' || n.first[0] == 't')
                {
                    possibleCombinations.erase(std::remove_if(possibleCombinations.begin(), possibleCombinations.end(), [firstInPair, secondInPair](const tuple<int, int, int, int> &n) -> bool
                                                              { return (get<0>(n) == stoi(firstInPair) && (stoi(secondInPair) == get<1>(n) || stoi(secondInPair) == get<2>(n) || stoi(secondInPair) == get<3>(n))); }),
                                               possibleCombinations.end());
                }
                else
                {
                    possibleCombinations.erase(std::remove_if(possibleCombinations.begin(), possibleCombinations.end(), [firstInPair, secondInPair](const tuple<int, int, int, int> &n) -> bool
                                                              { return (get<1>(n) == stoi(firstInPair) && (stoi(secondInPair) == get<1>(n) || stoi(secondInPair) == get<2>(n) || stoi(secondInPair) == get<3>(n))); }),
                                               possibleCombinations.end());
                    possibleCombinations.erase(std::remove_if(possibleCombinations.begin(), possibleCombinations.end(), [firstInPair, secondInPair](const tuple<int, int, int, int> &n) -> bool
                                                              { return (get<2>(n) == stoi(firstInPair) && (stoi(secondInPair) == get<1>(n) || stoi(secondInPair) == get<2>(n) || stoi(secondInPair) == get<3>(n))); }),
                                               possibleCombinations.end());
                    possibleCombinations.erase(std::remove_if(possibleCombinations.begin(), possibleCombinations.end(), [firstInPair, secondInPair](const tuple<int, int, int, int> &n) -> bool
                                                              { return (get<3>(n) == stoi(firstInPair) && (stoi(secondInPair) == get<1>(n) || stoi(secondInPair) == get<2>(n) || stoi(secondInPair) == get<3>(n))); }),
                                               possibleCombinations.end());
                }
            }
        }

        // Vector for the positions of duplicates
        vector<int> duplicates;

        // Filter all possible Combinations of duplicates, so that two answer combinations, where the answers are the same but but just shuffled
        for (auto pos = 0; pos < possibleCombinations.size(); ++pos)
        {
            for (auto pos2 = 0; pos2 < possibleCombinations.size(); ++pos2)
            {
                if ((pos != pos2) &&
                    (pos < pos2) &&
                    (get<0>(possibleCombinations[pos]) == get<0>(possibleCombinations[pos2])) &&
                    (get<1>(possibleCombinations[pos]) == get<1>(possibleCombinations[pos2]) || get<1>(possibleCombinations[pos]) == get<2>(possibleCombinations[pos2]) || get<1>(possibleCombinations[pos]) == get<3>(possibleCombinations[pos2])) &&
                    (get<2>(possibleCombinations[pos]) == get<1>(possibleCombinations[pos2]) || get<2>(possibleCombinations[pos]) == get<2>(possibleCombinations[pos2]) || get<2>(possibleCombinations[pos]) == get<3>(possibleCombinations[pos2])) &&
                    (get<3>(possibleCombinations[pos]) == get<1>(possibleCombinations[pos2]) || get<3>(possibleCombinations[pos]) == get<2>(possibleCombinations[pos2]) || get<3>(possibleCombinations[pos]) == get<3>(possibleCombinations[pos2])))
                {
                    if (!isInVector(duplicates, pos2))
                    {
                        duplicates.push_back(pos2);
                    }
                }
            }
        }

        // Based on the possible combinations and the duplicates, create all Valid Answer Combinations that are not duplicates
        vector<tuple<int, int, int, int>> validCombinations;
        for (int i = 0; i < possibleCombinations.size(); ++i)
        {
            if (!isInVector(duplicates, i))
            {
                validCombinations.push_back(possibleCombinations[i]);
            }
        }

        // Shuffle the validCombinations
        std::random_device rd;
        std::mt19937 gen(rd());
        std::shuffle(validCombinations.begin(), validCombinations.end(), gen);

        vector<vector<pair<string, string>>> validParameterPairs;
        vector<pair<string, string>> vecForRecursion;
        // create parameter Combinations
        // Parameters must be used recursiv in a function so for all parameters all values next parameter all values.
        if (!parameters.empty())
        {
            vector<vector<pair<string, string>>> allParameterPairs;
            vector<vector<pair<string, string>>> otherParameterPairs;
            for (Parameter p : parameters)
            {
                vector<pair<string, string>> parameterPairs;
                for (string s : p.GetValueRangeVector())
                {
                    parameterPairs.push_back(make_pair(p.GetName(), s));
                }
                allParameterPairs.push_back(parameterPairs);
            }
            createAllParameters(0, allParameterPairs, otherParameterPairs, vecForRecursion);

            for (vector v : otherParameterPairs)
            {
                if (checkValidParameter(v, interactions))
                {
                    validParameterPairs.push_back(v);
                }
            }
        }

        //"Bis zu Line 900 werden die Fragen an sich gebildet. Also zusammenführung von Antworten und Parametern zuzüglich der weiteren Sektionen."
        if (mode == 1)
        {
            // Randomising the all Parameters vector
            std::shuffle(validParameterPairs.begin(), validParameterPairs.end(), gen);

            // Create Number of Questions and Replace potential Parameters
            // Based on the shuffled vecotrs of questions and parameters
            int questionToTake{0}, parametersToTake{0};
            int WhichDiagram = 0;
            for (int i = 0; i < numberOfExams; ++i)
            {

                string correctAnswer{}, wrongAnswerOne{}, wrongAnswerTwo{}, wrongAnswerThree{};
                int correct{}, first{}, second{}, third{};

                correctAnswer = correctAnswers[get<0>(validCombinations[questionToTake])];
                wrongAnswerOne = wrongAnswers[get<1>(validCombinations[questionToTake])];
                wrongAnswerTwo = wrongAnswers[get<2>(validCombinations[questionToTake])];
                wrongAnswerThree = wrongAnswers[get<3>(validCombinations[questionToTake])];
                // Copy of Original, otherwise parameters are overwriten
                uml = umlsOriginal;

                // Setting these Variables because task, code and additionalText are set through the parser and when using Parameters we write them out
                // So to keep the Originals we use the extra variables *ToSet to dont change the variables that might be the same when creating more then one
                taskToSet = task;
                codeToSet = code;
                additionalTextToSet = additionalText;
                // if Parameters exist
                if (!parameters.empty())
                {
                    // Replace all parameters where they could stand
                    // The variables are initialized above inside the for loop,
                    // so for every new question it is replace with another value
                    for (pair n : validParameterPairs.at(parametersToTake))
                    {
                        replaceAll(correctAnswer, get<0>(n), get<1>(n));
                        replaceAll(wrongAnswerOne, get<0>(n), get<1>(n));
                        replaceAll(wrongAnswerTwo, get<0>(n), get<1>(n));
                        replaceAll(wrongAnswerThree, get<0>(n), get<1>(n));
                        replaceAll(codeToSet, get<0>(n), get<1>(n));
                        replaceAll(taskToSet, get<0>(n), get<1>(n));
                        replaceAll(additionalTextToSet, get<0>(n), get<1>(n));
                        for (int i = 0; i < uml.size(); i++)
                        {
                            for (int j = 0; j < uml[i].size(); j++)
                            {
                                // Replace Parameters in UML-Description
                                replaceAll(uml[i][j], get<0>(n), get<1>(n));
                            }
                        }
                    }

                    if (stillParametersInTexts(parameters, correctAnswer, wrongAnswerOne, wrongAnswerTwo, wrongAnswerThree, codeToSet, taskToSet, additionalTextToSet))
                    {
                        do
                        {
                            for (pair n : validParameterPairs.at(parametersToTake))
                            {
                                replaceAll(correctAnswer, get<0>(n), get<1>(n));
                                replaceAll(wrongAnswerOne, get<0>(n), get<1>(n));
                                replaceAll(wrongAnswerTwo, get<0>(n), get<1>(n));
                                replaceAll(wrongAnswerThree, get<0>(n), get<1>(n));
                                replaceAll(codeToSet, get<0>(n), get<1>(n));
                                replaceAll(taskToSet, get<0>(n), get<1>(n));
                                replaceAll(additionalTextToSet, get<0>(n), get<1>(n));
                            }
                        } while (stillParametersInTexts(parameters, correctAnswer, wrongAnswerOne, wrongAnswerTwo, wrongAnswerThree, codeToSet, taskToSet, additionalTextToSet));
                    }
                }
                // Based on the mode replacing certain characters  mode2==2 = Moodle other is Ilias
                if (mode2 == 2)
                {
                    for (pair n : replacementsForXmlMoodle)
                    {
                        replaceAll(correctAnswer, get<0>(n), get<1>(n));
                        replaceAll(wrongAnswerOne, get<0>(n), get<1>(n));
                        replaceAll(wrongAnswerTwo, get<0>(n), get<1>(n));
                        replaceAll(wrongAnswerThree, get<0>(n), get<1>(n));
                        replaceAll(codeToSet, get<0>(n), get<1>(n));
                        replaceAll(taskToSet, get<0>(n), get<1>(n));
                        replaceAll(additionalTextToSet, get<0>(n), get<1>(n));
                    }
                }
                else
                {
                    for (pair n : replacementsForXmlIlias)
                    {
                        replaceAll(correctAnswer, get<0>(n), get<1>(n));
                        replaceAll(wrongAnswerOne, get<0>(n), get<1>(n));
                        replaceAll(wrongAnswerTwo, get<0>(n), get<1>(n));
                        replaceAll(wrongAnswerThree, get<0>(n), get<1>(n));
                        replaceAll(taskToSet, get<0>(n), get<1>(n));
                        // replaceAll(additionalTextToSet, get<0>(n), get<1>(n));
                    }
                    for (pair n : replacementsForXmlIliasSpecialCode)
                    {
                        replaceAll(codeToSet, get<0>(n), get<1>(n));
                    }
                    for (pair n : replacementsForXmlIliasSpecialAdditionalText)
                    {
                        replaceAll(additionalTextToSet, get<0>(n), get<1>(n));
                    }
                }
                // Cycle through the Diagrams; if Checkbox "Use all Templates" is not checked, always use first Template
                if (MultipleUML == 0)
                {
                    WhichDiagram = 0;
                }
                Diagram d(currentDiaId, uml[WhichDiagram]);
                WhichDiagram++;
                // go back to first Diagram if all Diagrams have been used
                if (WhichDiagram >= uml.size())
                {
                    WhichDiagram = 0;
                }
                Question q(currentQuestionId, name, author, description, additionalTextToSet, codeToSet, taxonomy, taskToSet, correctAnswer, wrongAnswerOne, wrongAnswerTwo, wrongAnswerThree, picture);
                if (!questionExists(q, questions))
                {
                    ++currentQuestionId;
                    ++currentDiaId;
                    questions.push_back(q);
                    diagrams.push_back(d);
                }
                ++parametersToTake;
                ++questionToTake;
                if (questionToTake >= validCombinations.size())
                {
                    questionToTake = 0;
                }
                if (parametersToTake >= validParameterPairs.size())
                {
                    parametersToTake = 0;
                }
            }
        }
        else if (mode == 2)
        {
            for (tuple t : validCombinations)
            {
                if (!parameters.empty())
                {
                    for (vector v_validParam : validParameterPairs)
                    {
                        string correctAnswer{}, wrongAnswerOne{}, wrongAnswerTwo{}, wrongAnswerThree{};
                        int correct{}, first{}, second{}, third{};

                        correctAnswer = correctAnswers[get<0>(t)];
                        wrongAnswerOne = wrongAnswers[get<1>(t)];
                        wrongAnswerTwo = wrongAnswers[get<2>(t)];
                        wrongAnswerThree = wrongAnswers[get<3>(t)];

                        // Setting these Variables because task, code and additionalText are set through the parser and when using Parameters we write them out
                        // So to keep the Originals we use the extra variables *ToSet to dont change the variables that might be the same when creating more then one
                        taskToSet = task;
                        codeToSet = code;
                        additionalTextToSet = additionalText;
                        // Replace all parameters where they could stand

                        // textersetzung.
                        for (pair n : v_validParam)
                        {
                            replaceAll(correctAnswer, get<0>(n), get<1>(n));
                            replaceAll(wrongAnswerOne, get<0>(n), get<1>(n));
                            replaceAll(wrongAnswerTwo, get<0>(n), get<1>(n));
                            replaceAll(wrongAnswerThree, get<0>(n), get<1>(n));
                            replaceAll(codeToSet, get<0>(n), get<1>(n));
                            replaceAll(taskToSet, get<0>(n), get<1>(n));
                            replaceAll(additionalTextToSet, get<0>(n), get<1>(n));
                        }
                        if (stillParametersInTexts(parameters, correctAnswer, wrongAnswerOne, wrongAnswerTwo, wrongAnswerThree, codeToSet, taskToSet, additionalTextToSet))
                        {
                            do
                            {
                                for (pair n : v_validParam)
                                {
                                    replaceAll(correctAnswer, get<0>(n), get<1>(n));
                                    replaceAll(wrongAnswerOne, get<0>(n), get<1>(n));
                                    replaceAll(wrongAnswerTwo, get<0>(n), get<1>(n));
                                    replaceAll(wrongAnswerThree, get<0>(n), get<1>(n));
                                    replaceAll(codeToSet, get<0>(n), get<1>(n));
                                    replaceAll(taskToSet, get<0>(n), get<1>(n));
                                    replaceAll(additionalTextToSet, get<0>(n), get<1>(n));
                                }
                            } while (stillParametersInTexts(parameters, correctAnswer, wrongAnswerOne, wrongAnswerTwo, wrongAnswerThree, codeToSet, taskToSet, additionalTextToSet));
                        }

                        // Based on the mode replacing certain characters  mode2==2 = Moodle other is Ilias
                        if (mode2 == 2)
                        {
                            for (pair n : replacementsForXmlMoodle)
                            {
                                replaceAll(correctAnswer, get<0>(n), get<1>(n));
                                replaceAll(wrongAnswerOne, get<0>(n), get<1>(n));
                                replaceAll(wrongAnswerTwo, get<0>(n), get<1>(n));
                                replaceAll(wrongAnswerThree, get<0>(n), get<1>(n));
                                replaceAll(codeToSet, get<0>(n), get<1>(n));
                                replaceAll(taskToSet, get<0>(n), get<1>(n));
                                replaceAll(additionalTextToSet, get<0>(n), get<1>(n));
                            }
                        }
                        else
                        {
                            for (pair n : replacementsForXmlIlias)
                            {
                                replaceAll(correctAnswer, get<0>(n), get<1>(n));
                                replaceAll(wrongAnswerOne, get<0>(n), get<1>(n));
                                replaceAll(wrongAnswerTwo, get<0>(n), get<1>(n));
                                replaceAll(wrongAnswerThree, get<0>(n), get<1>(n));
                                // replaceAll(codeToSet, get<0>(n), get<1>(n));
                                replaceAll(taskToSet, get<0>(n), get<1>(n));
                                // replaceAll(additionalTextToSet, get<0>(n), get<1>(n));
                            }
                            for (pair n : replacementsForXmlIliasSpecialCode)
                            {
                                replaceAll(codeToSet, get<0>(n), get<1>(n));
                            }
                            for (pair n : replacementsForXmlIliasSpecialAdditionalText)
                            {
                                replaceAll(additionalTextToSet, get<0>(n), get<1>(n));
                            }
                        }
                        Question q(currentQuestionId, name, author, description, additionalTextToSet, codeToSet, taxonomy, taskToSet, correctAnswer, wrongAnswerOne, wrongAnswerTwo, wrongAnswerThree, picture);
                        if (!questionExists(q, questions))
                        {
                            ++currentQuestionId;
                            questions.push_back(q);
                        }
                    }
                }
                else
                {
                    string correctAnswer{}, wrongAnswerOne{}, wrongAnswerTwo{}, wrongAnswerThree{};
                    int correct{}, first{}, second{}, third{};

                    correctAnswer = correctAnswers[get<0>(t)];
                    wrongAnswerOne = wrongAnswers[get<1>(t)];
                    wrongAnswerTwo = wrongAnswers[get<2>(t)];
                    wrongAnswerThree = wrongAnswers[get<3>(t)];
                    taskToSet = task;
                    codeToSet = code;
                    additionalTextToSet = additionalText;

                    // Based on the mode replacing certain characters  mode2==2 = Moodle other is Ilias
                    if (mode2 == 2)
                    {
                        for (pair n : replacementsForXmlMoodle)
                        {
                            replaceAll(correctAnswer, get<0>(n), get<1>(n));
                            replaceAll(wrongAnswerOne, get<0>(n), get<1>(n));
                            replaceAll(wrongAnswerTwo, get<0>(n), get<1>(n));
                            replaceAll(wrongAnswerThree, get<0>(n), get<1>(n));
                            replaceAll(codeToSet, get<0>(n), get<1>(n));
                            replaceAll(taskToSet, get<0>(n), get<1>(n));
                            replaceAll(additionalTextToSet, get<0>(n), get<1>(n));
                        }
                    }
                    else
                    {
                        for (pair n : replacementsForXmlIlias)
                        {
                            replaceAll(correctAnswer, get<0>(n), get<1>(n));
                            replaceAll(wrongAnswerOne, get<0>(n), get<1>(n));
                            replaceAll(wrongAnswerTwo, get<0>(n), get<1>(n));
                            replaceAll(wrongAnswerThree, get<0>(n), get<1>(n));
                            // replaceAll(codeToSet, get<0>(n), get<1>(n));
                            replaceAll(taskToSet, get<0>(n), get<1>(n));
                            // replaceAll(additionalTextToSet, get<0>(n), get<1>(n));
                        }
                        for (pair n : replacementsForXmlIliasSpecialCode)
                        {
                            replaceAll(codeToSet, get<0>(n), get<1>(n));
                        }
                        for (pair n : replacementsForXmlIliasSpecialAdditionalText)
                        {
                            replaceAll(additionalTextToSet, get<0>(n), get<1>(n));
                        }
                    }
                    Question q(currentQuestionId, name, author, description, additionalTextToSet, codeToSet, taxonomy, taskToSet, correctAnswer, wrongAnswerOne, wrongAnswerTwo, wrongAnswerThree, picture);
                    if (!questionExists(q, questions))
                    {
                        ++currentQuestionId;
                        questions.push_back(q);
                    }
                }
            }
        }

        // Once per question template, it is checked wheter the current taxonomy is allready existing for the generating process
        if (!isInVector(taxonomyLevels, taxonomy))
        {
            taxonomyLevels.push_back(taxonomy);
        }
        cout << endl
             << "Created Questions for " << filename << endl
             << "There now exist " << currentQuestionId - 1 << " questions and " << currentDiaId - 1 << " Diagrams" << endl
             << endl;
    }

    // Checking in Questions, so that only one question exists where everything but currentQuestionId is the same

    // Creating Output Structure
    if (!fs::exists(outputPath))
    {
        fs::create_directory(outputPath);
    }

    // Add file names to pictures

    pictures = addUMLtoPictures(diagrams, pictures);
    // Create UML input files
    int c = writeUML(diagrams, argv[7]);
    // Call PlantUML and display potential error messages
    cout << "PlantUML Command: " << plantUMLcommand.c_str() << endl
         << endl;
    cout << plantUMLcall(plantUMLcommand.c_str()) << endl
         << endl;
    int height, width;

    // read dimensions of png files and set correct height and width of pictures in questions
    setPNGdimensions(pictureInputpath, questions);

    if (mode2 == 1)
    {
        string questionpoolFolder = outputFolder + "\\" + ILIASfolderMarking + qpl + questionPoolId;
        fs::create_directory(questionpoolFolder);
        // create subfolder of subfolder because this format is needed for .zip ILIAS import
        questionpoolFolder = outputFolder + "\\" + ILIASfolderMarking + qpl + questionPoolId + "\\" + ILIASfolderMarking + qpl + questionPoolId;
        fs::path questionpoolPath{questionpoolFolder};
        fs::create_directory(questionpoolPath);

        fs::path modulesPath{questionpoolFolder + "\\Modules"};
        fs::create_directory(modulesPath);
        fs::path modulesTestQPPath{questionpoolFolder + "\\Modules\\TestQuestionPool"};
        fs::create_directory(modulesTestQPPath);
        fs::path modulesTestQPSetPath{questionpoolFolder + "\\Modules\\TestQuestionPool\\set_1"};
        fs::create_directory(modulesTestQPSetPath);

        fs::path servicesPath{questionpoolFolder + "\\Services"};
        fs::create_directory(servicesPath);
        fs::path servicesTaxPath{questionpoolFolder + "\\Services\\Taxonomy"};
        fs::create_directory(servicesTaxPath);
        fs::path servicesTaxSetPath{questionpoolFolder + "\\Services\\Taxonomy\\set_1"};
        fs::create_directory(servicesTaxSetPath);

        // Write/Append to Files
        ofstream writeToFile;
        // Write Manifest xml
        writeToFile.open(questionpoolFolder + "\\manifest.xml", ios_base::out);
        writeToFile << "<?xml version=\"1.0\" encoding =\"UTF-8\"?><!--Generated by ILIAS XmlWriter--><Manifest MainEntity=\"qpl\" Title=\""
                    << "Testimport"
                    << "\" TargetRelease=\"5.4.0\" InstallationId=\"0\" InstallationUrl=\"https://eassessment.rz.uni-leipzig.de/eklausuren\"><ExportFile Component=\"Modules/TestQuestionPool\" Path=\"Modules/TestQuestionPool/set_1/export.xml\"/><ExportFile Component=\"Services/Taxonomy\" Path=\"Services/Taxonomy/set_1/export.xml\"/></Manifest>";
        writeToFile.close();

        // Write export in TestQuestionPool
        writeToFile.open(questionpoolFolder + "\\Modules\\TestQuestionPool\\set_1\\export.xml", ios_base::out);
        writeToFile << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
                    << "<!--Generated by ILIAS XmlWriter-->"
                    << "<exp:Export InstallationId=\"0\" InstallationUrl=\"https://eassessment.rz.uni-leipzig.de/eklausuren\" Entity=\"qpl\" SchemaVersion=\"4.1.0\" TargetRelease=\"5.4.0\" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xmlns:exp=\"http://www.ilias.de/Services/Export/exp/4_1\" xsi:schemaLocation=\"http://www.ilias.de/Services/Export/exp/4_1 https://eassessment.rz.uni-leipzig.de/eklausuren/xml/ilias_export_4_1.xsd http://www.ilias.de/Modules/TestQuestionPool/htlm/4_1 https://eassessment.rz.uni-leipzig.de/eklausuren/xml/ilias_qpl_4_1.xsd\" xmlns=\"http://www.ilias.de/Modules/TestQuestionPool/htlm/4_1\">"
                    << "<exp:ExportItem Id=\""
                    << questionPoolId
                    << "\"></exp:ExportItem></exp:Export>";
        writeToFile.close();

        // Write the Questions into the qti - items = base of the questions
        writeToFile.open(questionpoolFolder + "\\" + ILIASfolderMarking + qti + questionPoolId + ".xml");
        writeToFile << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
                    << "<!DOCTYPE questestinterop SYSTEM \"ims_qtiasiv1p2p1.dtd\">"
                    << "<!--Generated by ILIAS XmlWriter-->"
                    << "<questestinterop>";
        for (Question q : questions)
        {
            writeToFile << "<item ident=\"il_0_qst_" << ILIASquestionID + q.GetQuestionId() - 1 << "\" title=\"" << q.GetName() << "\" maxattempts=\"0\">"
                        << "<qticomment>" << q.GetDescription() << "</qticomment><duration>P0Y0M0DT0H1M0S</duration><itemmetadata><qtimetadata><qtimetadatafield><fieldlabel>ILIAS_VERSION</fieldlabel><fieldentry>5.4.26 2021-12-22</fieldentry></qtimetadatafield><qtimetadatafield><fieldlabel>QUESTIONTYPE</fieldlabel><fieldentry>SINGLE CHOICE QUESTION</fieldentry></qtimetadatafield>"
                        << "<qtimetadatafield><fieldlabel>AUTHOR</fieldlabel><fieldentry>" << q.GetAuthor() << "</fieldentry></qtimetadatafield><qtimetadatafield><fieldlabel>additional_cont_edit_mode</fieldlabel><fieldentry>default</fieldentry></qtimetadatafield><qtimetadatafield><fieldlabel>thumb_size</fieldlabel><fieldentry/></qtimetadatafield>"
                        << "<qtimetadatafield><fieldlabel>feedback_setting</fieldlabel><fieldentry>2</fieldentry></qtimetadatafield><qtimetadatafield><fieldlabel>singleline</fieldlabel><fieldentry>0</fieldentry></qtimetadatafield></qtimetadata></itemmetadata>"
                        << "<presentation label=\"" << q.GetName() << "\"><flow>"
                        << "<material><mattext texttype=\"text/xhtml\"><![CDATA[<p>" << q.GetTask() << "</p>]]></mattext></material>"
                        << "<response_lid ident=\"MCSR\" rcardinality=\"Single\"><render_choice shuffle=\"Yes\">"
                        << "<response_label ident=\"0\"><material><mattext texttype=\"text/xhtml\"><![CDATA[<p>" << q.GetCorrectAnswer() << "</p>]]></mattext></material></response_label>"
                        << "<response_label ident=\"1\"><material><mattext texttype=\"text/xhtml\"><![CDATA[<p>" << q.GetWrongAnswerOne() << "</p>]]></mattext></material></response_label>"
                        << "<response_label ident=\"2\"><material><mattext texttype=\"text/xhtml\"><![CDATA[<p>" << q.GetWrongAnswerTwo() << "</p>]]></mattext></material></response_label>"
                        << "<response_label ident=\"3\"><material><mattext texttype=\"text/xhtml\"><![CDATA[<p>" << q.GetWrongAnswerThree() << "</p>]]></mattext></material></response_label>"
                        << "</render_choice></response_lid></flow></presentation><resprocessing><outcomes><decvar></decvar></outcomes><respcondition continue=\"Yes\"><conditionvar><varequal respident=\"MCSR\">0</varequal></conditionvar><setvar action=\"Add\">1</setvar><displayfeedback feedbacktype=\"Response\" linkrefid=\"response_0\"/></respcondition>"
                        << "<respcondition continue=\"Yes\"><conditionvar><varequal respident=\"MCSR\">1</varequal></conditionvar><setvar action=\"Add\">0</setvar><displayfeedback feedbacktype=\"Response\" linkrefid=\"response_1\"/></respcondition>"
                        << "<respcondition continue=\"Yes\"><conditionvar><varequal respident=\"MCSR\">2</varequal></conditionvar><setvar action=\"Add\">0</setvar><displayfeedback feedbacktype=\"Response\" linkrefid=\"response_2\"/></respcondition>"
                        << "<respcondition continue=\"Yes\"><conditionvar><varequal respident=\"MCSR\">3</varequal></conditionvar><setvar action=\"Add\">0</setvar><displayfeedback feedbacktype=\"Response\" linkrefid=\"response_3\"/></respcondition></resprocessing>"
                        << "<itemfeedback ident=\"response_0\" view=\"All\"><flow_mat><material><mattext texttype=\"text/plain\">&#10;          </mattext></material></flow_mat></itemfeedback>"
                        << "<itemfeedback ident=\"response_1\" view=\"All\"><flow_mat><material><mattext texttype=\"text/plain\">&#10;          </mattext></material></flow_mat></itemfeedback>"
                        << "<itemfeedback ident=\"response_2\" view=\"All\"><flow_mat><material><mattext texttype=\"text/plain\">&#10;          </mattext></material></flow_mat></itemfeedback>"
                        << "<itemfeedback ident=\"response_3\" view=\"All\"><flow_mat><material><mattext texttype=\"text/plain\">&#10;          </mattext></material></flow_mat></itemfeedback></item>";
        }

        writeToFile << "</questestinterop>";
        writeToFile.close();

        // Writing Question extras like Code or additional text in qpl- questionpool - creating the structure for each question
        //  a question can be with additional Text, Picture or Code
        writeToFile.open(questionpoolFolder + "\\" + ILIASfolderMarking + qpl + questionPoolId + ".xml");
        writeToFile << "<?xml version=\"1.0\" encoding=\"UTF-8\"?><!DOCTYPE Test SYSTEM \"http://www.ilias.uni-koeln.de/download/dtd/ilias_co.dtd\"><!--Export of ILIAS Test Questionpool " << questionPoolId << " of installation .-->"
                    << "<ContentObject Type=\"Questionpool_Test\"><MetaData><General Structure=\"Hierarchical\">"
                    << "<Identifier Catalog=\"ILIAS\" Entry=\"il_0_qpl_" << questionPoolId << "\"/><Title Language=\"de\">" << qpTitle << "</Title><Language Language=\"de\"/><Description Language=\"de\"></Description><Keyword Language=\"de\"/></General></MetaData>"
                    << "<Settings><ShowTaxonomies>1</ShowTaxonomies><NavTaxonomy>0</NavTaxonomy><SkillService>0</SkillService></Settings>";
        for (Question q : questions)
        {
            writeToFile << "<PageObject>";
            if (!q.IsAdditionalTextEmpty())
            {
                writeToFile << "<PageContent><Paragraph Language=\"de\" Characteristic=\"Standard\">" << q.GetAdditionalText() << "</Paragraph></PageContent>";
            }
            if (!q.IsCodeEmpty())
            {
                writeToFile << "<PageContent><Paragraph Language=\"de\" SubCharacteristic=\"cpp\" ShowLineNumbers=\"y\" Characteristic=\"Code\">" << q.GetCode() << "</Paragraph></PageContent>";
            }
            // If Pictures are in question, then create a new content
            if (q.GetPicture().GetName() != "")
            {
                writeToFile << "<PageContent><MediaObject><MediaAlias OriginId=\"il_0_mob_" << ILIASpictureID + q.GetPicture().GetId() << "\"/><MediaAliasItem Purpose=\"Standard\"><Layout HorizontalAlign=\"Left\" Width=\"" << q.GetPicture().GetWidth() << "\" Height=\"" << q.GetPicture().GetHeight() << "\"/></MediaAliasItem></MediaObject></PageContent>";
            }
            writeToFile << "<PageContent><Question QRef=\"il_0_qst_" << ILIASquestionID + q.GetQuestionId() - 1 << "\"/></PageContent></PageObject>";
        }
        for (Picture p : pictures)
        {
            if (p.GetName() != "")
            {
                writeToFile << "<MediaObject><MetaData><General Structure=\"Hierarchical\"><Identifier Catalog=\"ILIAS\" Entry=\"il_0_mob_" << ILIASpictureID + p.GetId() - 1 << "\"/>"
                            << "<Title Language=\"de\">" << p.GetName() << "</Title><Language Language=\"de\"/><Description Language=\"de\">image/png</Description><Keyword Language=\"de\"/></General></MetaData>"
                            << "<MediaItem Purpose=\"Standard\"><Location Type=\"LocalFile\">" << p.GetName() << "</Location><Format>image/png</Format><Layout   HorizontalAlign=\"Left\" /></MediaItem></MediaObject>";
            }
        }
        writeToFile << "<QuestionSkillAssignments>";
        for (Question q : questions)
        {
            writeToFile << "<TriggerQuestion Id=\"" << ILIASquestionID + q.GetQuestionId() - 1 << "\"></TriggerQuestion>";
        }
        writeToFile << "</QuestionSkillAssignments></ContentObject>";
        writeToFile.close();

        // Writing the export xml for the Used Taxonomy
        writeToFile.open(questionpoolFolder + "\\Services\\Taxonomy\\set_1\\export.xml", ios_base::out);
        writeToFile << "<?xml version=\"1.0\" encoding=\"UTF-8\"?><!--Generated by ILIAS XmlWriter--><exp:Export InstallationId=\"0\" InstallationUrl=\"https://eassessment.rz.uni-leipzig.de/eklausuren\" Entity=\"tax\" SchemaVersion=\"4.3.0\" TargetRelease=\"5.4.0\" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xmlns:exp=\"http://www.ilias.de/Services/Export/exp/4_1\" xsi:schemaLocation=\"http://www.ilias.de/Services/Export/exp/4_1 https://eassessment.rz.uni-leipzig.de/eklausuren/xml/ilias_export_4_1.xsd http://www.ilias.de/Services/Taxonomy/tax/4_3 https://eassessment.rz.uni-leipzig.de/eklausuren/xml/ilias_tax_4_3.xsd http://www.ilias.de/Services/DataSet/ds/4_3 https://eassessment.rz.uni-leipzig.de/eklausuren/xml/ilias_ds_4_3.xsd\" xmlns=\"http://www.ilias.de/Services/Taxonomy/tax/4_3\" xmlns:ds=\"http://www.ilias.de/Services/DataSet/ds/4_3\">"
                    << "<exp:ExportItem Id = \"" << taxId << "\"><ds:DataSet InstallationId=\"0\" InstallationUrl=\"https://eassessment.rz.uni-leipzig.de/eklausuren\" TopEntity=\"tax\">";
        // Tax Toplevel
        writeToFile << "<ds:Rec Entity=\"tax\"><Tax><Id>" << taxId << "</Id><Title>" << taxTitle << "</Title><Description></Description><SortingMode>0</SortingMode></Tax></ds:Rec>";
        // TaxTree Toplevel
        writeToFile << "<ds:Rec Entity=\"tax_tree\"><TaxTree><TaxId>" << taxId << "</TaxId><Child>" << parentId << "</Child><Parent>0</Parent><Depth>1</Depth><Type></Type><Title>Root node for taxonomy " << taxId << "</Title><OrderNr>0</OrderNr></TaxTree></ds:Rec>";

        // For each level in taxonomy levels add a node to the tree
        // temp is just the Order which each node is in. each new taxonomy level is one node, under each node do the question et created
        int temp = 10;
        for (string t : taxonomyLevels)
        {
            writeToFile << "<ds:Rec Entity=\"tax_tree\"><TaxTree><TaxId>" << taxId << "</TaxId><Child>"
                        << childId << "</Child><Parent>" << parentId << "</Parent><Depth>2</Depth><Type>taxn</Type><Title>"
                        << t << "</Title><OrderNr>" << temp << "</OrderNr></TaxTree></ds:Rec>";
            for (Question q : questions)
            {
                if (q.GetTaxonomy() == t)
                {
                    writeToFile << "<ds:Rec Entity=\"tax_node_assignment\"><TaxNodeAssignment><NodeId>" << childId
                                << "</NodeId><Component>qpl</Component><ItemType>quest</ItemType><ItemId>" << q.GetQuestionId() << "</ItemId></TaxNodeAssignment></ds:Rec>";
                }
            }
            ++childId;
            temp += 10;
        }
        writeToFile << "<ds:Rec Entity=\"tax_usage\"><TaxUsage><TaxId>" << taxId << "</TaxId><ObjId>" << questionPoolId << "</ObjId></TaxUsage></ds:Rec></ds:DataSet></exp:ExportItem></exp:Export>";
        writeToFile.close();

        if (!pictures.empty())
        {
            // general path for pictures, needed when pictures arent empty.
            fs::path picturePath{questionpoolFolder + "\\objects"};
            fs::create_directory(picturePath);

            // Pictures that are in vector, create folder structure for it.
            for (Picture p : pictures)
            {
                // creating a path for the picture based on the id
                string pStringPath = questionpoolFolder + "\\objects" + "\\il_0_mob_" + to_string(ILIASpictureID + p.GetId() - 1);
                fs::path pPath{pStringPath};
                fs::create_directory(pPath);
                // copy the picture from inputPictured in the newly created folder
                string fromPath{pictureInputpath + "\\" + p.GetName()};
                // Overwrite existing files doesnt work in some cases, gcc bug
                // https://github.com/msys2/MSYS2-packages/issues/1937
                fs::copy_file(fromPath, pStringPath + "\\" + p.GetName(), std::filesystem::copy_options::overwrite_existing);
            }
        }
    }

    if (mode2 == 2)
    {
        ofstream writeToFile;
        // Creating Moodle Xml with just questions and their answers.
        writeToFile.open(outputFolder + "\\" + "moodle.xml");
        writeToFile << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
                    << "<quiz>";
        for (Question q : questions)
        {
            writeToFile << "<question type=\"multichoice\"><name><text>" << q.GetName() << "</text></name>"
                        << "<questiontext format=\"html\"><text><![CDATA[<p dir=\"ltr\" style=\"text-align: left;\">" << q.GetTask() << "</p>]]></text></questiontext>"
                        << "<generalfeedback format=\"html\"><text></text></generalfeedback><defaultgrade>1</defaultgrade><penalty>0</penalty><hidden>0</hidden><idnumber></idnumber>"
                        << "<single>true</single><shuffleanswers>true</shuffleanswers><answernumbering>abc</answernumbering><showstandardinstruction>0</showstandardinstruction><shownumcorrect/>"
                        << "<answer fraction=\"100\" format=\"html\"><text><![CDATA[<p dir=\"ltr\" style=\"text-align: left;\">" << q.GetCorrectAnswer() << "</p>]]></text></answer>"
                        << "<answer fraction=\"0\" format=\"html\"><text><![CDATA[<p dir=\"ltr\" style=\"text-align: left;\">" << q.GetWrongAnswerOne() << "</p>]]></text></answer>"
                        << "<answer fraction=\"0\" format=\"html\"><text><![CDATA[<p dir=\"ltr\" style=\"text-align: left;\">" << q.GetWrongAnswerTwo() << "</p>]]></text></answer>"
                        << "<answer fraction=\"0\" format=\"html\"><text><![CDATA[<p dir=\"ltr\" style=\"text-align: left;\">" << q.GetWrongAnswerThree() << "</p>]]></text></answer>"
                        << "</question>";
        }
        writeToFile << "</quiz>";
        writeToFile.close();
    }

    cout << "Number of questions and Diagrams created: " << currentQuestionId - 1 << " and " << currentDiaId - 1 << endl;

    return 0;
}
