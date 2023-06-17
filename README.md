# Single-Choice-Generator-with-UML-Diagram-Generation

Generator to create parameterized single choice questions, UML-diagrams and question pools based on templates.

## Usage:

1. Building main.c++ in src folder either using make (compiles and executes the program), or just compiling the .cpp-Files with an adequate compiler. Currently everything was tested with g++ -std=c++2a.

2. Use tool with main.py (Initial Values are stored in config.toml)

## Input Mask options explained:

Output: Output stored in ILIAS or Moodle format

Mode: Create all Questions and Diagrams or random

Question Pool ID: ID of the Question pool for ILIAS Uploas

Question Pool Title: Title of the Question pool for ILIAS Upload

Taxonomy Title: Taxonomy Title for ILIAS Upload

Number of Versions: How many versions to be generated

ILIAS Folder Marking: Forder Marking for the Output-Folder Structure

First Question ID: From which Question ID to start in Output Structure

Use all Diagram Templates: Switch between all specified UML-Templates, or take only first one

Save Parameters to config: Save current values from Input mask in config.toml, so that they are loaded on next program start

## Additional Infos:

The path structure is currently only tested on Windows, and would probaby need some customization and further developement.

Questions have to be created as stated in the TemplateSpecification.txt Each question has to be in its own file, in the input folder. 

There are two modi when using the generator. Randomised generation, so that you can specify how many instantiations you want for each question. - The valid Answer possibilities and parameter combinations get randomised and then matched in cycles. If using randomised, and Parameters, please be sure that at least one parameter combination is valid and not that the interactions exclude all possibilities. All - generates all possible instantiation for each question. Permutations of Answers are matched with each permutation of parameters.

The Generator supports the export format for ILIAS-Questionpool Imports. In Addition to this a simple Moodle-XML is supported, were ONLY the questions and answers can be imported!

## Tools used

This tool is an expansion of an already existing single choice question generator 

More infos: https://github.com/NWillert/Template-basedGeneratorForSingleChoice/tree/main

The generation of UML diagrams is done by PlantUML, distributed under LGPL.

More infos: https://plantuml.com/
