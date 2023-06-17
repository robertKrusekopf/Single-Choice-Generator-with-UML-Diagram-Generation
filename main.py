# This code generates a GUI for a single-choice question generator with PlantUML integration, which is implemented in C++
# The GUI allows the user to select various options for generating the questions, such as the output format, mode, number of versions, etc.
# The user can also save the parameters to a configuration file and generate the questions by clicking on the "Generate" button.


import subprocess  # for calling c++ file
import os  # for path and folder operations
import shutil  # for creating zip file
import sys  # for error messages
from tkinter import *  # for the GUI
import tomli  # for reading the tomli config file
import tomli_w  # for writing to tomli
#import robot  # for Testing



# Create the tkinter window and set its properties
master = Tk()
master.title("Single-Choice Generator with PlantUML integration")
master.iconbitmap(True, "icon.ico")
master.minsize(700, 600)
master.maxsize(700, 600)


# read configuration file and store to a dictionary
try:
    with open("config.toml", "rb") as f:
        config = tomli.load(f)
except FileNotFoundError:
    sys.exit("Error: config.toml file not found")
#except tomli.TOMLDecodeError:
#    sys.exit("Error: wrong syntax found in config.toml")


# throw error and exit if Output, Mode ore UseallUML are not set to a valid value
if (config["Output"] != 0) and (config["Output"] != 1):
    sys.exit("Error: Output in config.toml is not set to a valid input (0 or 1)")
elif (config["Mode"] != 0) and (config["Mode"] != 1):
    sys.exit("Error: Mode in config.toml is not set to a valid input (0 or 1)")
elif (config["UseallUML"] != 0) and (config["UseallUML"] != 1):
    sys.exit("Error: Mode in config.toml is not set to a valid input (0 or 1)")

# Create labels and input fields for various options in the GUI
Label(master, text="Output:").grid(row=0, sticky=W)
mode2_list = ["IliasXML", "MoodleXML"]
mode2 = StringVar(master)
mode2.set(mode2_list[config["Output"]])
e1 = OptionMenu(master, mode2, *mode2_list)
e1.config(width=14)
e1.grid(row=0, column=1)


Label(master, text="Mode:").grid(row=1, sticky=W)
mode_list = ["Random", "All"]
mode = StringVar(master)
mode.set(mode_list[config["Mode"]])
e2 = OptionMenu(master, mode, *mode_list)
e2.config(width=14)
e2.grid(row=1, column=1)


Label(master, text="question pool id:").grid(row=2, sticky=W)
questionpoolid = Entry(master)
questionpoolid.insert(0, config["QuestionPoolID"])
questionpoolid.grid(row=2, column=1)


Label(master, text="question pool title:").grid(row=3, sticky=W)
questionpooltitle = Entry(master)
questionpooltitle.insert(0, config["QuestionPoolTitle"])
questionpooltitle.grid(row=3, column=1)

Label(master, text="Taxonomy Title:").grid(row=4, sticky=W)
taxTitle = Entry(master)
taxTitle.insert(0, config["TaxonomyTitle"])
taxTitle.grid(row=4, column=1)


Label(master, text="Number of versions:").grid(row=5, sticky=W)
numberofExams = Entry(master)
numberofExams.insert(0, config["NumberofVersions"])
numberofExams.grid(row=5, column=1)

Label(master, text="ILIAS Folder Marking:").grid(row=6, sticky=W)
FolderNumber = Entry(master)
FolderNumber.insert(0, config["ILIASFolderMarking"])
FolderNumber.grid(row=6, column=1)

Label(master, text=" First Question-ID:").grid(row=7, sticky=W)
QuestionID = Entry(master)
QuestionID.insert(0, config["FirstQuestionID"])
QuestionID.grid(row=7, column=1)

MultipleUML = IntVar(value=config["UseallUML"])
Checkbutton(master, text="Use all Diagram templates", variable=MultipleUML).grid(
    row=8, columnspan=10, sticky=W
)

SaveConfig = IntVar()
Checkbutton(master, text="Save Parameters to config", variable=SaveConfig).grid(
    row=9, columnspan=12, sticky=W
)

# Create a label frame for console output
labelframe = LabelFrame(
    master, text="Console Output - scrollable", height=550, width=425
)
labelframe.grid(row=0, rowspan=10, columnspan=3, column=6, padx=(25, 0))
labelframe.grid_propagate(FALSE)  # fix size of labelframe
OutputMC = Text(labelframe, width=50, height=32, wrap=CHAR)
OutputMC.insert(END, "Waiting...")
OutputMC.grid(row=0, columnspan=3)


# function to create a zip file of the generated questions for easier upload to Ilias
def create_zip():
    # name of output folder
    foldername = os.path.join(os.path.dirname(__file__), "output")
    # name of subfolder in output folder
    subfoldername = FolderNumber.get() + "__0__qpl_" + questionpoolid.get()
    # create zip file
    shutil.make_archive(
        foldername + "\\" + subfoldername, "zip", foldername + "\\" + subfoldername
    )
    # Check if the zip file was created successfully and display a message
    if os.path.exists(foldername + "\\" + subfoldername + ".zip"):
        OutputMC.insert(END, subfoldername + " created!")
    else:
        OutputMC.insert(END, "ZIP file not created")


# function to write current params to configuration file
def write_tomli(mode2_index, mode_index):
    doc = {
        "title": "config file for Single-Choice Generator with PlantUML integration",
        "Output": mode2_index - 1,
        "Mode": mode_index - 1,
        "QuestionPoolID": questionpoolid.get(),
        "QuestionPoolTitle": questionpooltitle.get(),
        "TaxonomyTitle": taxTitle.get(),
        "NumberofVersions": numberofExams.get(),
        "ILIASFolderMarking": FolderNumber.get(),
        "FirstQuestionID": QuestionID.get(),
        "UseallUML": MultipleUML.get(),
    }
    with open("config.toml", "wb") as f:
       tomli_w.dump(doc, f)
       


# start generation process on button-click
def start_generation():
    # mode text to index
    if mode2.get() == mode2_list[0]:
        mode2_index = 1
    else:
        mode2_index = 2
    if mode.get() == mode_list[0]:
        mode_index = 1
    else:
        mode_index = 2

    OutputMC.config(state=NORMAL)
    # filename = os.path.join(os.path.dirname(__file__), 'main.exe')
    filename_exe = os.path.dirname(__file__) + "\\src\\" + "main.exe"
    dirname = os.path.join(os.path.dirname(__file__))
    OutputMC.delete(1.0, END)
    # check if there are no non-numeric values in numeric input fields
    if (
        questionpoolid.get().isdigit()
        and numberofExams.get().isdigit()
        and FolderNumber.get().isdigit()
        and QuestionID.get().isdigit()
    ):  # if yes, open cmd with parameters
        cmdOutputMC = subprocess.Popen(
            [
                filename_exe,
                str(mode2_index),
                questionpoolid.get(),
                questionpooltitle.get(),
                taxTitle.get(),
                str(mode_index),
                numberofExams.get(),
                dirname,
                FolderNumber.get() + "__0__",
                QuestionID.get(),
                str(MultipleUML.get())
            ],
            stdout=subprocess.PIPE,
            stderr=subprocess.STDOUT,
        )

        OutputMC.insert(END, cmdOutputMC.stdout.read())
        # only when exit code is not zero
        if cmdOutputMC.wait() != 0:
            OutputMC.insert(END, "non-zero exit status: " + str(cmdOutputMC.wait()))
            # TODO: prüfen ob der Errorcode nur bei diesem Fehler auftritt
            if cmdOutputMC.wait() == 3221225477:
                OutputMC.insert(END, " (not enough false answers.)")
        create_zip()
        if SaveConfig.get() == 1:
           write_tomli(mode2_index, mode_index)
        OutputMC.config(state=DISABLED)
    else:  # if no, show error message
        OutputMC.insert(END, "found non-numeric input in numeric input field.")



# Alternative, but viable Method to directly call plantUML
"""
def plantUML():
    OutputUML.config(state=NORMAL)
    filenameIn = os.path.join(os.path.dirname(__file__), "inputUML")
    filenameOut = os.path.join(os.path.dirname(__file__), "outputUML")
    print(filenameIn)
    print(filenameOut)
    cmdOutputUML = subprocess.Popen(
        ["java", "-jar", "plantuml.jar", filenameIn, "-o", filenameOut],
        stdout=subprocess.PIPE,
        stderr=subprocess.STDOUT,
    )
    OutputUML.delete(1.0, END)
    OutputUML.insert(END, cmdOutputUML.stdout.read())
    # nur wenn Exit-Code nicht 0
    if cmdOutputUML.wait() != 0:
        OutputUML.insert(END, "non-zero exit status: " + str(cmdOutputUML.wait()))
    else:
        OutputUML.insert(END, "Successfully generated Output!")
    OutputUML.config(state=DISABLED)

"""


Button(master, text="Generate", command=start_generation, height=2, width=20).grid(
    row=10, rowspan=3, columnspan=4
)

mainloop()
