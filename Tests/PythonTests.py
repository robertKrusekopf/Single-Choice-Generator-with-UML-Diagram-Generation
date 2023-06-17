
unittest.main()


import unittest
import os
import sys
from tkinter import *
import tomli
import tomli_w

class TestSingleChoiceGenerator(unittest.TestCase):
    def test_output_value(self):
        try:
            with open("config.toml", "rb") as f:
                config = tomli.load(f)
        except FileNotFoundError:
            self.fail("config.toml file not found")
        except tomli.TOMLDecodeError:
            self.fail("config.toml file has incorrect syntax")
        self.assertIn(config["Output"], [0, 1])

    def test_mode_value(self):
        try:
            with open("config.toml", "rb") as f:
                config = tomli.load(f)
        except FileNotFoundError:
            self.fail("config.toml file not found")
        except tomli.TOMLDecodeError:
            self.fail("config.toml file has incorrect syntax")
        self.assertIn(config["Mode"], [0, 1])

    def test_numeric_input_fields(self):
        master = Tk()
        questionpoolid = Entry(master)
        questionpoolid.insert(0, "123")
        numberofExams = Entry(master)
        numberofExams.insert(0, "2")
        FolderNumber = Entry(master)
        FolderNumber.insert(0, "1")
        QuestionID = Entry(master)
        QuestionID.insert(0, "1")
        self.assertTrue(questionpoolid.get().isdigit())
        self.assertTrue(numberofExams.get().isdigit())
        self.assertTrue(FolderNumber.get().isdigit())
        self.assertTrue(QuestionID.get().isdigit())

    def test_non_numeric_input_fields(self):
        master = Tk()
        questionpoolid = Entry(master)
        questionpoolid.insert(0, "123a")
        numberofExams = Entry(master)
        numberofExams.insert(0, "2")
        FolderNumber = Entry(master)
        FolderNumber.insert(0, "1")
        QuestionID = Entry(master)
        QuestionID.insert(0, "1")
        self.assertFalse(questionpoolid.get().isdigit())
        self.assertTrue(numberofExams.get().isdigit())
        self.assertTrue(FolderNumber.get().isdigit())
        self.assertTrue(QuestionID.get().isdigit())




    def test_save_parameters_to_config(self):
        master = Tk()
        SaveConfig = IntVar()
        SaveConfig.set(1)
        mode2 = StringVar(master)
        mode2.set("IliasXML")
        mode = StringVar(master)
        mode.set("Random")
        questionpoolid = Entry(master)
        questionpoolid.insert(0, "123")
        questionpooltitle = Entry(master)
        questionpooltitle.insert(0, "Test Question Pool")
        taxTitle = Entry(master)
        taxTitle.insert(0, "Test Taxonomy Title")
        numberofExams = Entry(master)
        numberofExams.insert(0, "2")
        FolderNumber = Entry(master)
        FolderNumber.insert(0, "1")
        QuestionID = Entry(master)
        QuestionID.insert(0, "1")
        write_tomli(mode2_list.index(mode2.get()), mode_list.index(mode.get()))
        with open("config.toml", "rb") as f:
            config = tomli.load(f)
        self.assertEqual(config["Output"], mode2_list.index(mode2.get()) - 1)
        self.assertEqual(config["Mode"], mode_list.index(mode.get()) - 1)
        self.assertEqual(config["QuestionPoolID"], questionpoolid.get())
        self.assertEqual(config["QuestionPoolTitle"], questionpooltitle.get())
        self.assertEqual(config["TaxonomyTitle"], taxTitle.get())
        self.assertEqual(config["NumberofVersions"], numberofExams.get)


if name == 'main':
    unittest.main()