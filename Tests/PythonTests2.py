import unittest
import os
import shutil
from tkinter import *
import subprocess
import tomli
import tomli_w

class TestSingleChoiceGenerator(unittest.TestCase):

def setUp(self):
    
self.master = Tk()
self.master.title("Single-Choice Generator with PlantUML integration")
self.master.iconbitmap("icon.ico")
self.master.minsize(700, 600)
self.master.maxsize(700, 600)

def test_config_file_syntax(self):
    with open("config.toml", "w") as f:
        f.write("Output = 0\\nMode = 0\\nQuestionPoolID = 123\\nQuestionPoolTitle = Test\\n")
    self.assertRaises(tomli.TOMLDecodeError, self.master.quit)

def test_output_value_error(self):
    with open("config.toml", "w") as f:
        f.write("Output = 2\\nMode = 0\\nQuestionPoolID = 123\\nQuestionPoolTitle = Test\\n")
    self.assertRaises(SystemExit, self.master.quit)

def test_mode_value_error(self):
    with open("config.toml", "w") as f:
        f.write("Output = 0\\nMode = 2\\nQuestionPoolID = 123\\nQuestionPoolTitle = Test\\n")
    self.assertRaises(SystemExit, self.master.quit)

def test_create_zip(self):
    if os.path.exists("output"):
        shutil.rmtree("output")
    os.mkdir("output")
    with open("config.toml", "w") as f:
        f.write("Output = 0\\nMode = 0\\nQuestionPoolID = 123\\nQuestionPoolTitle = Test\\nILIASFolderMarking = 1\\n")
    self.master.quit()
    self.assertEqual(os.path.exists("output/1__0__qpl_123.zip"), True)

def test_write_tomli(self):
    with open("config.toml", "w") as f:
        f.write("Output = 0\\nMode = 0\\nQuestionPoolID = 123\\nQuestionPoolTitle = Test\\nILIASFolderMarking = 1\\n")
    self.master.quit()
    with open("config.toml", "rb") as f:
        config = tomli.load(f)
    self.assertEqual(config["Output"], 0)
    self.assertEqual(config["Mode"], 0)
    self.assertEqual(config["QuestionPoolID"], "123")
    self.assertEqual(config["QuestionPoolTitle"], "Test")
    self.assertEqual(config["ILIASFolderMarking"], 1)

def test_start_generation(self):
    with open("config.toml", "w") as f:
        f.write("Output = 0\\nMode = 0\\nQuestionPoolID = 123\\nQuestionPoolTitle = Test\\nILIASFolderMarking = 1\\n")
    self.master.quit()
    cmdOutputMC = subprocess.Popen(
        [
            "python",
            "src/main.py",
            "0",
            "123",
            "Test",
            "Taxonomy",
            "0",
            "1",
            os.path.dirname(__file__),
            "1__0__",
            "1",
        ],
        stdout=subprocess.PIPE,
        stderr=subprocess.STDOUT,
    )
    self.assertEqual(cmdOutputMC.wait(), 0)

def tearDown(self):
    self.master.destroy()
