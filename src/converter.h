#ifndef CONVERTER_H
#define CONVERTER_H

#include <vector>
#include <string>
#include "parameter.h"
#include "picture.h"
#include "question.h"

class Diagram
{
public:
    Diagram(int Id,const std::vector<std::string>& uml);
    int GetDiaId();
    std::vector<std::string> GetUML();

private:
    int diaId;
    std::vector<std::string> uml;
};
int writeUML(std::vector<Diagram> diagrams, std::string path);
std::string plantUMLcall(const char *plantUMLcommand);
std::vector<Picture> addUMLtoPictures(std::vector<Diagram> diagrams,std::vector<Picture> pictures);
void setPNGdimensions(std::string filename, std::vector<Question> &questions);
void deleteFolders(const std::string& path, std::string subfoldername);

#endif