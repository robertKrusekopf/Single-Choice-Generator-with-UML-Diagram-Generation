*** Settings ***
Documentation     UML-Tool
Library           main.py

*** Test Cases ***

	
User can input Questionpoolid as Int
	[Template]	Users can input only integers in ${questionpoolid}
	Peter
	Test
		
User can input Questionpoolid as Int
	[Template]	Users can input only integers in ${questionpoolid}
	Peter
	Test

	
User can input Questionpoolid as Int
	[Template]	Users can input only integers in ${questionpoolid}
	Peter
	Test

	
User can input Questionpoolid as Int
	[Template]	Users can input only integers in ${questionpoolid}
	Peter
	Test

	
User can input Questionpoolid as Int
	[Template]	Users can input only integers in ${questionpoolid}
	Peter
	Test

	
User can input Questionpoolid as Int
	[Template]	Users can input only integers in ${questionpoolid}
	Peter
	Test

	
User can input Questionpoolid as Int
	[Template]	Users can input only integers in ${questionpoolid}
	Peter
	Test


*** Keywords ***
If ${mode2_list} is set to IliasXML, ${mode2_index} is 0 and if it is set on MoodleXML, it is 1
	Should Be Equal As Numbers    ${mode2_list}    ${mode2_index}
	
Users can input only integers in ${questionpoolid}
	Should Be Equal	${questionpoolid}	${questionpoolid}