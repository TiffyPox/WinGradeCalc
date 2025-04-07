// Include necessary libraries
#include <cstdio> // To allow input and output
#include <fstream> // To handle writing grade data to a text file
#include <map> // To use maps (a container to store elements in key-value pairs)
#include <sstream> // Used for building strings like "Average Grade: 99"
#include <string> // To use strings
#include <vector> // To use vectors
#include <windows.h> // Essential for all Windows API applications

// Define control IDs for max buffer size and max students
#define BUFFER_SIZE 255
#define MAX_STUDENTS 7

// An enumeration to hold the different screen states
enum ScreenState
{
    MAIN, // or STUDENT LIST screen
    ADD_STUDENT,
    GRADES,
    STATS
};

// An enumeration to hold the different subjects as well as the maximum (5)
enum Subject
{
    SCIENCE = 0,
    ENGLISH = 1,
    MATHS = 2,
    HISTORY = 3,
    ART = 4,
    MAX_SUBJECTS = 5
};

// A struct to hold the data shown on the STUDENT LIST screen
struct StudentListRow
{
    HWND studentRowName = 0;
    HWND studentRowButton = 0;
    int studentRowButtonID = 0;
    HWND removeRowButton = 0;
    int removeRowButtonID = 0;
};

// A struct to hold the student data
struct StudentData
{
    char name[BUFFER_SIZE];
    int grades[MAX_SUBJECTS];
};

// A vector to hold all of the students
std::vector<StudentData> students {};

// Maps to help handle the student data shown in the STUDENT LIST screen
std::map <StudentData*, StudentListRow> studentToStudentListRow {};
std::map <int, StudentData*> studentButtonIDToStudentData {};
std::map <int, StudentData*> removeButtonIDToStudentData {};

// State control to help handle which screen is currently active and if the program is running
ScreenState currentState = MAIN;
bool Running = true;

// Global variables
const char* windowClassName = "GradeCalculatorWindow";
const int windowWidth = 600;
const int windowHeight = 400;

// An array of subject strings
// Should match the order in the Subject enum to ensure correct string
// For example: SCIENCE at index zero in the Subject enum is equal to "SCIENCE"
std::string SubjectStrings[MAX_SUBJECTS]
{
    "SCIENCE",
    "ENGLISH",
    "MATHS",
    "HISTORY",
    "ART"
};

// Main window handle
HWND mainWindowHandle = 0;

// STUDENT LIST screen handles
HWND studentListWindowHandle = 0;
HWND addStudentButtonHandle = 0;

// GRADES screen handles
HWND gradesScreenWindowHandle = 0;
HWND gradesScreenNameHandle = 0;
HWND gradesScreenScienceGradeHandle = 0;
HWND gradesScreenEnglishGradeHandle = 0;
HWND gradesScreenMathsGradeHandle = 0;
HWND gradesScreenHistoryGradeHandle = 0;
HWND gradesScreenArtGradeHandle = 0;
HWND gradeScreenAverageHandle = 0;

// ADD STUDENT screen handles
HWND addStudentWindowHandle = 0;
HWND studentNameWindowHandle = 0;
HWND scienceGradeWindowHandle = 0;
HWND englishGradeWindowHandle = 0;
HWND mathsGradeWindowHandle = 0;
HWND historyGradeWindowHandle = 0;
HWND artGradeWindowHandle = 0;

// STATS screen handles
HWND statisticsWindowHandle = 0;
HWND statisticsTitleWindowHandle = 0;
HWND averageGradeWindowHandle = 0;
HWND highestGradeWindowHandle = 0;
HWND lowestGradeWindowHandle = 0;

// Styles
HFONT fontHandle = 0;
HBRUSH buttonBrushHandle = 0;

// Define control IDs for the buttons and text fields
#define EXIT_BUTTON_ID 100
#define STAT_BUTTON_ID 102
#define BACK_BUTTON_ID 103
#define ADD_BUTTON_ID 104
#define SUBMIT_BUTTON_ID 105

#define SAVE_BUTTON_ID 117
#define LOAD_BUTTON_ID 118

#define STUDENT_NAME_TEXT_FIELD_ID 106
#define GRADE_1_TEXT_FIELD_ID 107
#define GRADE_2_TEXT_FIELD_ID 108
#define GRADE_3_TEXT_FIELD_ID 109
#define GRADE_4_TEXT_FIELD_ID 110
#define GRADE_5_TEXT_FIELD_ID 111

#define STATS_SCIENCE_BUTTON_ID 112
#define STATS_ENGLISH_BUTTON_ID 113
#define STATS_MATHS_BUTTON_ID 114
#define STATS_HISTORY_BUTTON_ID 115
#define STATS_ART_BUTTON_ID 116

#define STUDENT_BUTTON_ID_START 1000
#define STUDENT_REMOVE_ID_START 2000

// A function to create the menu bar
void CreateMenuBar(HWND hwnd)
{
    // A handle to tell Windows what I am referring to, Windows will create the menu
    HMENU menuBarHandle = CreateMenu();

    // A handle to tell Windows what I am referring to, Windows will create a drop down menu
    HMENU fileDropDownHandle = CreatePopupMenu();

    // Create a pop up window to access the file drop down menu
    AppendMenuA(menuBarHandle, MF_POPUP, (UINT_PTR)fileDropDownHandle, "File");

    // A 'Save' file and 'Load' file button
    AppendMenuA(fileDropDownHandle, MF_STRING, SAVE_BUTTON_ID, "Save");
    AppendMenuA(fileDropDownHandle, MF_STRING, LOAD_BUTTON_ID, "Load");

    // Add a separator (a line) to the drop down menu to split the (Save, Load) and (Exit) options 
    AppendMenuA(fileDropDownHandle, MF_SEPARATOR, 0, 0);
    AppendMenuA(fileDropDownHandle, MF_STRING, EXIT_BUTTON_ID, "Exit");

    // Setting the menu for the given window
    SetMenu(hwnd, menuBarHandle);
}

// A function to create the ADD STUDENT screen
void CreateAddStudentScreen(HWND hwnd)
{
    // Set the padding, button height and button width variables
    int padding = 10;
    int buttonHeight = 30;
    int buttonWidth = 100;

    // Add ADD STUDENT screen, which allows the user to input a student
    addStudentWindowHandle = CreateWindowA(windowClassName, "", WS_CHILD, 0, 0, windowWidth, windowHeight, hwnd, NULL, NULL, NULL);
    // Create 'BACK' and 'SUBMIT' buttons
    CreateWindowA("BUTTON", "BACK", WS_VISIBLE | WS_CHILD, padding, 350 - padding - buttonHeight, buttonWidth, buttonHeight, addStudentWindowHandle, (HMENU)BACK_BUTTON_ID, NULL, NULL);
    CreateWindowA("BUTTON", "SUBMIT", WS_VISIBLE | WS_CHILD, windowWidth - padding - buttonWidth - 5, 350 - padding - buttonHeight, buttonWidth, buttonHeight, addStudentWindowHandle, (HMENU)SUBMIT_BUTTON_ID, NULL, NULL);
    
    // Create all of the input boxes for the student name and five grades
    // I have created some padding in between them by adding to the y position
    int yPos = 50;
    CreateWindowA("STATIC", "Student Name:", WS_CHILD | WS_VISIBLE, 50, yPos, 150, buttonHeight, addStudentWindowHandle, NULL, NULL, NULL);
    studentNameWindowHandle = CreateWindowA("EDIT", "", WS_CHILD | WS_VISIBLE, 50 + 150 + padding, yPos, 200, buttonHeight, addStudentWindowHandle, (HMENU)STUDENT_NAME_TEXT_FIELD_ID, NULL, NULL);
    yPos += buttonHeight + padding;
    CreateWindowA("STATIC", "Science Grade:", WS_CHILD | WS_VISIBLE, 50, yPos, 150, buttonHeight, addStudentWindowHandle, NULL, NULL, NULL);
    scienceGradeWindowHandle = CreateWindowA("EDIT", "", WS_CHILD | WS_VISIBLE, 50 + 150 + padding, yPos, 200, buttonHeight, addStudentWindowHandle, (HMENU)GRADE_1_TEXT_FIELD_ID, NULL, NULL);
    yPos += buttonHeight + padding;
    CreateWindowA("STATIC", "English Grade:", WS_CHILD | WS_VISIBLE, 50, yPos, 150, buttonHeight, addStudentWindowHandle, NULL, NULL, NULL);
    englishGradeWindowHandle = CreateWindowA("EDIT", "", WS_CHILD | WS_VISIBLE, 50 + 150 + padding, yPos, 200, buttonHeight, addStudentWindowHandle, (HMENU)GRADE_2_TEXT_FIELD_ID, NULL, NULL);
    yPos += buttonHeight + padding;
    CreateWindowA("STATIC", "Maths Grade:" , WS_CHILD | WS_VISIBLE, 50, yPos, 150, buttonHeight, addStudentWindowHandle, NULL, NULL, NULL);
    mathsGradeWindowHandle = CreateWindowA("EDIT", "", WS_CHILD | WS_VISIBLE, 50 + 150 + padding, yPos, 200, buttonHeight, addStudentWindowHandle, (HMENU)GRADE_3_TEXT_FIELD_ID, NULL, NULL);
    yPos += buttonHeight + padding;
    CreateWindowA("STATIC", "History Grade:", WS_CHILD | WS_VISIBLE, 50, yPos, 150, buttonHeight, addStudentWindowHandle, NULL, NULL, NULL);
    historyGradeWindowHandle = CreateWindowA("EDIT", "", WS_CHILD | WS_VISIBLE, 50 + 150 + padding, yPos, 200, buttonHeight, addStudentWindowHandle, (HMENU)GRADE_4_TEXT_FIELD_ID, NULL, NULL);
    yPos += buttonHeight + padding;
    CreateWindowA("STATIC", "Art Grade:", WS_CHILD | WS_VISIBLE, 50, yPos, 150, buttonHeight, addStudentWindowHandle, NULL, NULL, NULL);
    artGradeWindowHandle = CreateWindowA("EDIT", "", WS_CHILD | WS_VISIBLE, 50 + 150 + padding, yPos, 200, buttonHeight, addStudentWindowHandle, (HMENU)GRADE_5_TEXT_FIELD_ID, NULL, NULL);
}

// A function to create the STATS screen
void CreateStatisticsScreen(HWND hwnd)
{
    // Set the padding, button height and button width variables
    int padding = 10;
    int buttonHeight = 30;
    int buttonWidth = 100;

    // Add STATS screen, which contains the statistics of the student grades
    statisticsWindowHandle = CreateWindowA(windowClassName, "", WS_CHILD, 0, 0, windowWidth, windowHeight, hwnd, NULL, NULL, NULL);
    
    // At the top of the screen, display the different subjects as buttons
    // The user will be able to click on a subject button and see the stats for that subject
    // I have created some padding in between them by adding to the x position
    int subjectCount = 5;
    int subjectButtonWidth = ( windowWidth - ( subjectCount + 1 ) * padding ) / subjectCount - 1; 
    int xPos = padding;
    CreateWindowA("BUTTON", "SCIENCE", WS_VISIBLE | WS_CHILD, xPos, padding, subjectButtonWidth, buttonHeight, statisticsWindowHandle, (HMENU)STATS_SCIENCE_BUTTON_ID, NULL, NULL);
    xPos += subjectButtonWidth + padding;
    CreateWindowA("BUTTON", "ENGLISH", WS_VISIBLE | WS_CHILD, xPos, padding, subjectButtonWidth, buttonHeight, statisticsWindowHandle, (HMENU)STATS_ENGLISH_BUTTON_ID, NULL, NULL);
    xPos += subjectButtonWidth + padding;
    CreateWindowA("BUTTON", "MATHS", WS_VISIBLE | WS_CHILD, xPos, padding, subjectButtonWidth, buttonHeight, statisticsWindowHandle, (HMENU)STATS_MATHS_BUTTON_ID, NULL, NULL);
    xPos += subjectButtonWidth + padding;
    CreateWindowA("BUTTON", "HISTORY", WS_VISIBLE | WS_CHILD, xPos, padding, subjectButtonWidth, buttonHeight, statisticsWindowHandle, (HMENU)STATS_HISTORY_BUTTON_ID, NULL, NULL);
    xPos += subjectButtonWidth + padding;
    CreateWindowA("BUTTON", "ART", WS_VISIBLE | WS_CHILD, xPos, padding, subjectButtonWidth, buttonHeight, statisticsWindowHandle, (HMENU)STATS_ART_BUTTON_ID, NULL, NULL);
    xPos += subjectButtonWidth + padding;

    // Create BACK button to return to the STUDENT LIST screen
    CreateWindowA("BUTTON", "BACK", WS_VISIBLE | WS_CHILD, padding, 350 - padding - buttonHeight, buttonWidth, buttonHeight, statisticsWindowHandle, (HMENU)BACK_BUTTON_ID, NULL, NULL);

    // Below the different subject buttons, display what subject is currently selected
    // By default, this will be SCIENCE as that is the first subject in the list
    statisticsTitleWindowHandle = CreateWindowA("STATIC", "", WS_CHILD | WS_VISIBLE, windowWidth / 2 - buttonWidth / 2, 60, buttonWidth, buttonHeight, statisticsWindowHandle, NULL, NULL, NULL);

    // Display the average grade, the highest grade and the lowest grade for each subject
    // I have created some padding in between them by adding to the y position
    int yPos = 100;
    CreateWindowA("STATIC", "Average Grade:", WS_CHILD | WS_VISIBLE, padding, yPos, buttonWidth, buttonHeight, statisticsWindowHandle, NULL, NULL, NULL);
    averageGradeWindowHandle = CreateWindowA("STATIC", "0", WS_CHILD | WS_VISIBLE, padding + buttonWidth + padding, yPos, buttonWidth, buttonHeight, statisticsWindowHandle, NULL, NULL, NULL);
    yPos += buttonHeight + padding;
    CreateWindowA("STATIC", "Highest Grade:", WS_CHILD | WS_VISIBLE, padding, yPos, buttonWidth, buttonHeight, statisticsWindowHandle, NULL, NULL, NULL);
    highestGradeWindowHandle = CreateWindowA("STATIC", "0", WS_CHILD | WS_VISIBLE, padding + buttonWidth + padding, yPos, buttonWidth, buttonHeight, statisticsWindowHandle, NULL, NULL, NULL);
    yPos += buttonHeight + padding;
    CreateWindowA("STATIC", "Lowest Grade:", WS_CHILD | WS_VISIBLE, padding, yPos, buttonWidth, buttonHeight, statisticsWindowHandle, NULL, NULL, NULL);
    lowestGradeWindowHandle = CreateWindowA("STATIC", "0", WS_CHILD | WS_VISIBLE, padding + buttonWidth + padding, yPos, buttonWidth, buttonHeight, statisticsWindowHandle, NULL, NULL, NULL);
}

// A function to create the GRADES screen
void CreateGradesScreen(HWND hwnd)
{
    // Set the padding, button height and button width variables
    int padding = 10;
    int buttonHeight = 20;
    int buttonWidth = 100;

    // Add GRADES screen, which includes all of the students data (name, five grades)
    gradesScreenWindowHandle = CreateWindowA(windowClassName, "", WS_CHILD, 0, 0, windowWidth, windowHeight, hwnd, NULL, NULL, NULL);

    // Create 'BACK' button to return to the STUDENT LIST screen
    CreateWindowA("BUTTON", "BACK", WS_VISIBLE | WS_CHILD, padding, 350 - padding - buttonHeight, buttonWidth, buttonHeight, gradesScreenWindowHandle, (HMENU)BACK_BUTTON_ID, NULL, NULL);

    // Display the student data (name, five grades)
    // I have created some padding in between them by adding to the y position
    int yPos = 50;
    CreateWindowA("STATIC", "Student Name:", WS_CHILD | WS_VISIBLE, 50, yPos, 150, buttonHeight, gradesScreenWindowHandle, NULL, NULL, NULL);
    gradesScreenNameHandle = CreateWindowA("STATIC", "", WS_CHILD | WS_VISIBLE, 50 + 150 + padding, yPos, 200, buttonHeight, gradesScreenWindowHandle, (HMENU)STUDENT_NAME_TEXT_FIELD_ID, NULL, NULL);
    yPos += buttonHeight + padding;
    CreateWindowA("STATIC", "Science Grade:", WS_CHILD | WS_VISIBLE, 50, yPos, 150, buttonHeight, gradesScreenWindowHandle, NULL, NULL, NULL);
    gradesScreenScienceGradeHandle = CreateWindowA("STATIC", "", WS_CHILD | WS_VISIBLE, 50 + 150 + padding, yPos, 200, buttonHeight, gradesScreenWindowHandle, (HMENU)GRADE_1_TEXT_FIELD_ID, NULL, NULL);
    yPos += buttonHeight + padding;
    CreateWindowA("STATIC", "English Grade:", WS_CHILD | WS_VISIBLE, 50, yPos, 150, buttonHeight, gradesScreenWindowHandle, NULL, NULL, NULL);
    gradesScreenEnglishGradeHandle = CreateWindowA("STATIC", "", WS_CHILD | WS_VISIBLE, 50 + 150 + padding, yPos, 200, buttonHeight, gradesScreenWindowHandle, (HMENU)GRADE_2_TEXT_FIELD_ID, NULL, NULL);
    yPos += buttonHeight + padding;
    CreateWindowA("STATIC", "Maths Grade:" , WS_CHILD | WS_VISIBLE, 50, yPos, 150, buttonHeight, gradesScreenWindowHandle, NULL, NULL, NULL);
    gradesScreenMathsGradeHandle = CreateWindowA("STATIC", "", WS_CHILD | WS_VISIBLE, 50 + 150 + padding, yPos, 200, buttonHeight, gradesScreenWindowHandle, (HMENU)GRADE_3_TEXT_FIELD_ID, NULL, NULL);
    yPos += buttonHeight + padding;
    CreateWindowA("STATIC", "History Grade:", WS_CHILD | WS_VISIBLE, 50, yPos, 150, buttonHeight, gradesScreenWindowHandle, NULL, NULL, NULL);
    gradesScreenHistoryGradeHandle = CreateWindowA("STATIC", "", WS_CHILD | WS_VISIBLE, 50 + 150 + padding, yPos, 200, buttonHeight, gradesScreenWindowHandle, (HMENU)GRADE_4_TEXT_FIELD_ID, NULL, NULL);
    yPos += buttonHeight + padding;
    CreateWindowA("STATIC", "Art Grade:", WS_CHILD | WS_VISIBLE, 50, yPos, 150, buttonHeight, gradesScreenWindowHandle, NULL, NULL, NULL);
    gradesScreenArtGradeHandle = CreateWindowA("STATIC", "", WS_CHILD | WS_VISIBLE, 50 + 150 + padding, yPos, 200, buttonHeight, gradesScreenWindowHandle, (HMENU)GRADE_5_TEXT_FIELD_ID, NULL, NULL);
    yPos += buttonHeight + padding;
    CreateWindowA("STATIC", "Average Grade:", WS_CHILD | WS_VISIBLE, 50, yPos, 150, buttonHeight, gradesScreenWindowHandle, NULL, NULL, NULL);
    gradeScreenAverageHandle = CreateWindowA("STATIC", "", WS_CHILD | WS_VISIBLE, 50 + 150 + padding, yPos, 200, buttonHeight, gradesScreenWindowHandle, (HMENU)GRADE_5_TEXT_FIELD_ID, NULL, NULL);
}

// A function that converts the numerical grade to the appropriate letter grade
char ConvertToGrade(int percent)
{
    // If the student has a grade of 90 or more, they get an A grade
    if (percent >= 90)
    {
        return 'A';
    }
    // If the student has a grade of 80 or more, they get a B grade
    else if (percent >= 80)
    {
        return 'B';
    }
    // If the student has a grade of 70 or more, they get a C grade
    else if (percent >= 70)
    {
        return 'C';
    }
    // If the student has a grade of 60 or more, they get a D grade
    else if (percent >= 60)
    {
        return 'D';
    }
    // Any other grade results in an F grade
    else
    {
        return 'F';
    }
}

// A function to update the GRADES screen
void UpdateGradesScreen(const StudentData* studentData)
{
    // Setting the text to the students name
    SetWindowTextA(gradesScreenNameHandle, studentData->name);

    // A maximum of 20 characters
    char buffer[20];

    // Next, we print the students grade for each subject by calling the ConvertToGrade function 
    // sprintf will print a string with a certain format
    // %c refers to character and %i refers to integer 
    // For example: A (99%) - where A is the character and 99 is the integer
    sprintf(buffer, "%c (%i%)", ConvertToGrade(studentData->grades[SCIENCE]), studentData->grades[SCIENCE]); // Science grade
    SetWindowTextA(gradesScreenScienceGradeHandle, buffer);

    sprintf(buffer, "%c (%i%)", ConvertToGrade(studentData->grades[ENGLISH]), studentData->grades[ENGLISH]); // English grade
    SetWindowTextA(gradesScreenEnglishGradeHandle, buffer);

    sprintf(buffer, "%c (%i%)", ConvertToGrade(studentData->grades[MATHS]), studentData->grades[MATHS]); // Maths grade
    SetWindowTextA(gradesScreenMathsGradeHandle, buffer);

    sprintf(buffer, "%c (%i%)", ConvertToGrade(studentData->grades[HISTORY]),studentData->grades[HISTORY]); // History grade
    SetWindowTextA(gradesScreenHistoryGradeHandle, buffer);

    sprintf(buffer, "%c (%i%)", ConvertToGrade(studentData->grades[ART]), studentData->grades[ART]); // Art grade
    SetWindowTextA(gradesScreenArtGradeHandle, buffer);

    // A variable to hold the grade total
    int gradeTotal = 0;

    // For every grade in grades
    for(int grade : studentData->grades)
    {
        // Keep track of the total
        gradeTotal += grade;
    }

    // Print the average grade by dividing the total by the number of subjects (5) 
    sprintf(buffer, "%.2f", gradeTotal / (float) MAX_SUBJECTS);
    SetWindowTextA(gradeScreenAverageHandle, buffer);
}

// A function to set up the layout of the screens
void CreateScreens(HWND hwnd)
{
    // First screen, which contains the list of students
    studentListWindowHandle = CreateWindowA(windowClassName, "", WS_CHILD, 0, 0, windowWidth, windowHeight, hwnd, NULL, NULL, NULL);
    // Create an 'ADD STUDENT' and the 'STATS' buttons
    addStudentButtonHandle = CreateWindowA("BUTTON", "ADD STUDENT", WS_VISIBLE | WS_CHILD, 10, windowHeight - 30 - 60, 130, 30, studentListWindowHandle, (HMENU)ADD_BUTTON_ID, NULL, NULL);
    HWND statButtonHandle = CreateWindowA("BUTTON", "STATS", WS_VISIBLE | WS_CHILD, 10 + 130 + 10, windowHeight - 30 - 60, 130, 30, studentListWindowHandle, (HMENU)STAT_BUTTON_ID, NULL, NULL);

    // Setting the font 
    SendMessage(statButtonHandle, WM_SETFONT, (WPARAM)fontHandle, TRUE);

    // Creating ADD STUDENT, STATS and GRADES screens
    CreateAddStudentScreen(hwnd);
    CreateStatisticsScreen(hwnd);
    CreateGradesScreen(hwnd);
}

// Clearing the text fields so that the previous input is not displayed every time
void ClearTextFields()
{
    SetWindowTextA(studentNameWindowHandle, "");
    SetWindowTextA(scienceGradeWindowHandle, "");
    SetWindowTextA(englishGradeWindowHandle, "");
    SetWindowTextA(mathsGradeWindowHandle, "");
    SetWindowTextA(historyGradeWindowHandle, "");
    SetWindowTextA(artGradeWindowHandle, "");
}

// A function to extract the text that the user input into each field and save a new student
void AddStudent()
{
    // An empty student object
    StudentData student {};

    // The maximum characters to hold (255)
    char buffer[BUFFER_SIZE];

    // Get the student name that the user input
    GetWindowTextA(studentNameWindowHandle, student.name, BUFFER_SIZE);

    // Get the Science grade that the user input
    GetWindowTextA(scienceGradeWindowHandle, buffer, BUFFER_SIZE);
    // atoi is a function that converts the string into a numerical value
    // For example: "99" becomes 99
    student.grades[SCIENCE] = atoi(buffer);

    // Get the English grade that the user input
    GetWindowTextA(englishGradeWindowHandle, buffer, BUFFER_SIZE);
    student.grades[ENGLISH] = atoi(buffer);

    // Get the Maths grade that the user input
    GetWindowTextA(mathsGradeWindowHandle, buffer, BUFFER_SIZE);
    student.grades[MATHS] = atoi(buffer);

    // Get the History grade that the user input
    GetWindowTextA(historyGradeWindowHandle, buffer, BUFFER_SIZE);
    student.grades[HISTORY] = atoi(buffer);

    // Get the Art grade that the user input
    GetWindowTextA(artGradeWindowHandle, buffer, BUFFER_SIZE);
    student.grades[ART] = atoi(buffer);

    // Add the student to the list of students
    students.push_back(student);
}

// A function to rebuild (or reset) the list of students
void RebuildStudentToList()
{
    // We remake button IDs every time we reset the student list to clear the map, ready to be filled with new data
    studentButtonIDToStudentData.clear();
    removeButtonIDToStudentData.clear();

    // Destroy all of the old UI for each student, since we will remake it
    for (auto studentListRowPair:studentToStudentListRow)
    {
        DestroyWindow(studentListRowPair.second.studentRowButton);
        DestroyWindow(studentListRowPair.second.studentRowName);
        DestroyWindow(studentListRowPair.second.removeRowButton);
    }

    // Clear the student list row map as we will fill it with new data
    studentToStudentListRow.clear();

    // Keep track of the count (how many students have been processed)
    int count = 0;

    // For every student in students
    for (StudentData &studentData : students)
    {
        // Display the students name, a button to view their grades and a button to remove the student
        int buttonId = STUDENT_BUTTON_ID_START + count;
        int removeButtonId = STUDENT_REMOVE_ID_START + count;
        HWND studentNameHandle = CreateWindowA("STATIC", studentData.name, WS_CHILD | WS_VISIBLE, 10, 10 + (30 + 10) * count, 200, 30, studentListWindowHandle, NULL, NULL, NULL);
        HWND studentButtonHandle = CreateWindowA("BUTTON", "VIEW GRADES", WS_VISIBLE | WS_CHILD, 10 + 200 + 10, 10 + (30 + 10) * count, 200, 30, studentListWindowHandle, (HMENU)(intptr_t)buttonId, NULL, NULL);
        HWND removeButtonHandle = CreateWindowA("BUTTON", "REMOVE", WS_VISIBLE | WS_CHILD, 10 + 200 + 10 + 200 + 10, 10 + (30 + 10) * count, 100, 30, studentListWindowHandle, (HMENU)(intptr_t)removeButtonId, NULL, NULL);
        
        // Keep track of which student maps to which 'View Grades' button
        studentButtonIDToStudentData.emplace(buttonId, &studentData);

        // Keep track of which student maps to which 'Remove' button
        removeButtonIDToStudentData.emplace(removeButtonId, &studentData);

        // Keep track of all the UI handles for the student
        // So we can do clean-up when needed
        StudentListRow studentListRow {};
        studentListRow.studentRowButton = studentButtonHandle;
        studentListRow.studentRowName = studentNameHandle;
        studentListRow.studentRowButtonID = buttonId;
        studentListRow.removeRowButton = removeButtonHandle;
        studentListRow.removeRowButtonID = removeButtonId;
        studentToStudentListRow.emplace(&studentData, studentListRow);

        count++;
    }  
}

// A function to update the STATS screen
void UpdateStatisticsScreen(Subject Subject)
{
    // Set the STATS screen title based on the subject
    SetWindowTextA(statisticsTitleWindowHandle, SubjectStrings[Subject].c_str());

    // If no grades are input, return to avoid division by zero
    // This error-handling prevents the program from crashing 
    if (students.empty())
    {
        return;
    }

    // Variables to ensure we find the true highest and lowest values
    int highest = INT_MIN;
    int lowest = INT_MAX;
    int total = 0;

    // For every student in students
    for(const StudentData& student : students)
    {
        // Find which grade is the highest and which is the lowest
        int grade = student.grades[Subject]; 

        if (grade > highest)
        {
            highest = grade;
        }
        if (grade < lowest)
        {
            lowest = grade;
        }

        total += grade;
    }

    // A maximum of 10 characters
    char buffer[10];

    // Calculate and display the average
    float average = total / (float) students.size();
    sprintf(buffer, "%.2f", average);
    SetWindowTextA(averageGradeWindowHandle, buffer);

    // Display the highest and lowest grades
    sprintf(buffer, "%i", highest);
    SetWindowTextA(highestGradeWindowHandle, buffer);
    sprintf(buffer, "%i", lowest);
    SetWindowTextA(lowestGradeWindowHandle, buffer);
}

// A function to handle the STATS screen
void HandleStatisticsSelection(unsigned int buttonID)
{
    // Based on what button the user clicks
    switch(buttonID)
    {
        // If the user clicks SCIENCE, display the Science stats
        case STATS_SCIENCE_BUTTON_ID:
        {
            UpdateStatisticsScreen(SCIENCE);
        } break;
        // If the user clicks ENGLISH, display the English stats
        case STATS_ENGLISH_BUTTON_ID:
        {
            UpdateStatisticsScreen(ENGLISH);
        } break;
        // If the user clicks MATHS, display the Maths stats
        case STATS_MATHS_BUTTON_ID:
        {
            UpdateStatisticsScreen(MATHS);
        } break;
        // If the user clicks HISTORY, display the History stats
        case STATS_HISTORY_BUTTON_ID:
        {
            UpdateStatisticsScreen(HISTORY);
        } break;
        // If the user clicks ART, display the Art stats
        case STATS_ART_BUTTON_ID:
        {
            UpdateStatisticsScreen(ART);
        } break;
    }
}

// A function to check that the user entered a valid number
bool IsValidNumber(const char* str) 
{ 
    int charCount = 0;

    while (*str) 
    {
        // A utility to check if a string contains only digits
        if (!isdigit(*str))
        {
            return false;
        }  
        ++charCount;
        ++str; 
    } 

    // Also ensure the user entered at least one character, as a blank string is not a valid number
    return charCount > 0; 
}

// A function to check that the grade entered is more than zero and less than 100
bool IsNumberInRange(int val, int min, int max)
{
    return val >= min && val <= max;
}

// Error-Handling
// A function to ensure valid input
bool ValidateStudentData()
{
    // Display an error message box (a pop up screen) if the user does not enter a name 
    if(GetWindowTextLengthA(studentNameWindowHandle) <= 0)
    {
        MessageBoxA(0, "Student must have a valid name!", "Warning!", MB_ICONERROR);
        return false;
    }
    
    // The maximum number of characters (255)
    char buffer[255];
    // Get the input the user has entered in the Student Name field
    GetWindowTextA(studentNameWindowHandle, buffer, BUFFER_SIZE);

    // For every student in students
    for(const StudentData& student : students)
    {
        // Can't have students with the same name
        if(strcmp(student.name, buffer) == 0)
        {
            // Display an error message box
            MessageBoxA(0, "Student already exists!", "Warning!", MB_ICONERROR);
            return false;
        }
    }

    // Get the input the user has entered in the Science grade field
    GetWindowTextA(scienceGradeWindowHandle, buffer, BUFFER_SIZE);
    if(!IsValidNumber(buffer))
    {
        // Display an error message box if the Science grade is an invalid number
        MessageBoxA(0, "Grade is not a valid number!", "Warning!", MB_ICONERROR);
        return false;
    }
    if (!IsNumberInRange(atoi(buffer), 0, 100))
    {
        // Display an error message box if the Science grade is not between 0-100
        MessageBoxA(0, "Grade must be between 0-100!", "Warning!", MB_ICONERROR);
        return false;
    }

    // Get the input the user has entered in the English grade field
    GetWindowTextA(englishGradeWindowHandle, buffer, BUFFER_SIZE);
    if(!IsValidNumber(buffer))
    {
        // Display an error message box if the English grade is an invalid number
        MessageBoxA(0, "Grade is not a valid number!", "Warning!", MB_ICONERROR);
        return false;
    }
    if (!IsNumberInRange(atoi(buffer), 0, 100))
    {
        // Display an error message box if the English grade is not between 0-100
        MessageBoxA(0, "Grade must be between 0-100!", "Warning!", MB_ICONERROR);
        return false;
    }

    // Get the input the user has entered in the Maths grade field
    GetWindowTextA(mathsGradeWindowHandle, buffer, BUFFER_SIZE);
    if(!IsValidNumber(buffer))
    {
        // Display an error message box if the Maths grade is an invalid number
        MessageBoxA(0, "Grade is not a valid number!", "Warning!", MB_ICONERROR);
        return false;
    }
    if (!IsNumberInRange(atoi(buffer), 0, 100))
    {
        // Display an error message box if the Maths grade is not between 0-100
        MessageBoxA(0, "Grade must be between 0-100!", "Warning!", MB_ICONERROR);
        return false;
    }

    // Get the input the user has entered in the History grade field
    GetWindowTextA(historyGradeWindowHandle, buffer, BUFFER_SIZE);
    if(!IsValidNumber(buffer))
    {
        // Display an error messsage box if the History grade is an invalid number
        MessageBoxA(0, "Grade is not a valid number!", "Warning!", MB_ICONERROR);
        return false;
    }
    if (!IsNumberInRange(atoi(buffer), 0, 100))
    {
        // Display an error message box if the History grade is not between 0-100
        MessageBoxA(0, "Grade must be between 0-100!", "Warning!", MB_ICONERROR);
        return false;
    }

    // Get the input the user has enetered in the Art grade field
    GetWindowTextA(artGradeWindowHandle, buffer, BUFFER_SIZE);
    if(!IsValidNumber(buffer))
    {
        // Display an error message box if the Art grade is an invalid number
        MessageBoxA(0, "Grade is not a valid number!", "Warning!", MB_ICONERROR);
        return false;
    }
    if (!IsNumberInRange(atoi(buffer), 0, 100))
    {
        // Display an error message box if the Art grade is not between 0-100
        MessageBoxA(0, "Grade must be between 0-100!", "Warning!", MB_ICONERROR);
        return false;
    }

    // Return true if all of the input is valid
    return true;
}

// A function that saves the grades to a file
void SaveToFile()
{
    // Create a StudentGrades.txt file
    std::ofstream fileStream {"StudentGrades.txt"};
    // For every student in students, input the data into the file
    for (const StudentData& studentData : students)
    {
        fileStream << studentData.name << " ";

        for (int grade : studentData.grades)
        {
            fileStream << grade << " ";
        }

        // End the line so that each student is on a new line
        fileStream << std::endl;
    }

    // Close the file, file will now be saved
    fileStream.close();
}

// A function to load a file of student data
void LoadFromFile()
{
    // Clear the list of students
    students.clear();

    // Find and open the StudentGrades.txt file
    std::ifstream fileStream {"StudentGrades.txt"};
    
    // If file has been found and opened
    if (fileStream.is_open())
    {
        std::string line;

        // Read each line in the file
        while (std::getline(fileStream, line)) 
        {
            // Use a stringstream to split the line
            std::stringstream ss(line);
            StudentData student;
            // Import student name
            ss >> student.name;

            // For every grade, import grades
            for (int& grade : student.grades)
            {
                ss >> grade;
            }
            // Add student to the vector
            students.push_back(student);
        }
    }
}

// A function that processes the different buttons
void ProcessButtonCommand(HWND hwnd, unsigned int buttonID)
{
    // Based on what button is pressed
    switch (buttonID)
    {
    // If the 'Exit' button is pressed, quit the program 
    case EXIT_BUTTON_ID:
    {
        PostQuitMessage(0);
        Running = false;
    }
    break;
    // If the 'Load' button is pressed, load the file
    case LOAD_BUTTON_ID:
    {
        LoadFromFile();
        RebuildStudentToList();
    } break;
    // If the 'Save' button is pressed, save the file
    case SAVE_BUTTON_ID:
    {
        SaveToFile();
    } break;
    // If the 'ADD STUDENT' button is pressed, change the screen state
    case ADD_BUTTON_ID:
    {
        currentState = ADD_STUDENT;

        ShowWindow(studentListWindowHandle, SW_HIDE); // Hide the STUDENT LIST screen
        ShowWindow(addStudentWindowHandle, SW_SHOW); // Show the ADD STUDENT screen
    } break;
    // If the 'STATS' button is pressed, change the screen state
    case STAT_BUTTON_ID:
    {
        currentState = STATS;

        // Show the SCIENCE stats when opening STATS screen
        UpdateStatisticsScreen(SCIENCE);

        ShowWindow(studentListWindowHandle, SW_HIDE); // Hide the STUDENT LIST screen
        ShowWindow(statisticsWindowHandle, SW_SHOW); // Show the STATS screen
    } break;
    // If the 'SUBMIT' button is pressed, check the data is valid
    case SUBMIT_BUTTON_ID:
    {
        const bool IsValidData = ValidateStudentData();
        if(!IsValidData)
        {
            break;
        }

        // If the data is valid, add the student to the student list
        AddStudent();

        // Reset the student list so we can add new UI for each student
        RebuildStudentToList();

        // Clear the text fields on the ADD STUDENT screen
        ClearTextFields();

        // Either lock, or unlock the 'ADD STUDENT' button depending on how many students we have
        // We can only have a maximum of 7 students
        EnableWindow(addStudentButtonHandle, students.size() < MAX_STUDENTS);

        // Set the current state to the MAIN screen
        currentState = MAIN;

        ShowWindow(addStudentWindowHandle, SW_HIDE); // Hide the ADD STUDENT screen
        ShowWindow(studentListWindowHandle, SW_SHOW); // Show the STUDENT LIST screen
    } break;
    // If any of the subject buttons are pressed
    case STATS_SCIENCE_BUTTON_ID:
    case STATS_ENGLISH_BUTTON_ID:
    case STATS_MATHS_BUTTON_ID:
    case STATS_HISTORY_BUTTON_ID:
    case STATS_ART_BUTTON_ID:
    {
        // Handle the button logic based on what the user pressed
        HandleStatisticsSelection(buttonID);
    } break;
    // If the 'BACK' button is pressed, go back to the STUDENT LIST screen
    case BACK_BUTTON_ID:
    {
        currentState = MAIN;
        
        ShowWindow(addStudentWindowHandle, SW_HIDE); // Hide the ADD STUDENT screen
        ShowWindow(statisticsWindowHandle, SW_HIDE); // Hide the STATS screen
        ShowWindow(gradesScreenWindowHandle, SW_HIDE); // Hide the GRADES screen
        ShowWindow(studentListWindowHandle, SW_SHOW); // Show the STUDENT LIST screen
    } break;
    default:
    {
        // Check if the button ID is associated with either a 'View' or a 'Remove'
        auto studentButtonIter = studentButtonIDToStudentData.find(buttonID);
        auto removeButtonIter = removeButtonIDToStudentData.find(buttonID);
        if (studentButtonIter != studentButtonIDToStudentData.end())
        {
            // Get the student from the iter and update the GRADES screen based on their data
            UpdateGradesScreen((*studentButtonIter).second);

            ShowWindow(studentListWindowHandle, SW_HIDE); // Hide the STUDENT LIST screen
            ShowWindow(gradesScreenWindowHandle, SW_SHOW); // Show the GRADES screen
        }
        else if(removeButtonIter != removeButtonIDToStudentData.end())
        {
            // Find the student in the student list and remove them
            StudentData* iterStudent = (*removeButtonIter).second;
            for(auto i = students.begin(); i != students.end(); ++i)
            {
                if(&(*i) == iterStudent)
                {
                    students.erase(i);
                    break;
                }
            }

            // Rebuild the student list since we have altered it
            RebuildStudentToList();
            
            // Either lock, or unlock the 'ADD STUDENT' button depending on how many students we have
            EnableWindow(addStudentButtonHandle, students.size() < MAX_STUDENTS);
        }
    } break;
    }

    // Redraw the window 
    RedrawWindow(NULL, 0, NULL, RDW_ERASE | RDW_INVALIDATE | RDW_ALLCHILDREN);
}

// A function that handles the messages within the window
// hwnd is a pointer to the window that is sending the messages
LRESULT CALLBACK WinWindowCallback(HWND hwnd, UINT messageType, WPARAM wParam, LPARAM lParam)
{
    // Switch statement based on the message type
    switch (messageType)
    {
    // Process buttons
    case WM_COMMAND:
    {
        ProcessButtonCommand(hwnd, wParam);
    }
    break;

    // Handle static color draw. Removes the background from STATIC controls
    case WM_CTLCOLORSTATIC:
    {
        return (LRESULT)GetStockObject(NULL_BRUSH);
    }
    break;

    // Destroy the window if WM_DESTROY, WM_QUIT or WM_CLOSE is the message
    case WM_DESTROY:
    case WM_QUIT:
    case WM_CLOSE:
    {
        PostQuitMessage(0);
        Running = false;
    }
    break;

    // Otherwise, let Windows handle the messages
    default:
    {
        return DefWindowProc(hwnd, messageType, wParam, lParam);
    }
    }

    return 0;
}

// This is the window "blueprint", defining how a window should look and act
void WinRegisterWindowClass(HINSTANCE hInstance)
{
    // A struct to hold the data
    WNDCLASSA windowClass{};

    // We are not using any of the styles so this is set to zero
    // https://learn.microsoft.com/en-us/windows/win32/winmsg/window-class-styles
    windowClass.style = 0;

    // This points to the procedure the window will call
    // Without this, our messages couldn't be processed
    // https://learn.microsoft.com/en-us/windows/win32/api/winuser/nc-winuser-wndproc
    windowClass.lpfnWndProc = WinWindowCallback;

    // A handle to the instance that contains the window procedure for the class
    windowClass.hInstance = hInstance;

    // Set the default cursor
    windowClass.hCursor = LoadCursor(NULL, IDC_ARROW);
    windowClass.hbrBackground = CreateSolidBrush(RGB(242, 195, 230)); // Pink background

    // Set the window class name
    windowClass.lpszClassName = windowClassName;

    // Tell Windows to use the window I have created
    RegisterClassA(&windowClass);
}

// A function to create the font and button brush
void CreateStyles()
{
    // Create UI font and button brush
    fontHandle = CreateFontA(18, 0, 0, 0, FW_SEMIBOLD, FALSE, FALSE,
                             FALSE,
                             ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
                             DEFAULT_QUALITY,
                             DEFAULT_PITCH | FF_SWISS, "Segoe UI");
    buttonBrushHandle = CreateSolidBrush(RGB(180, 240, 180));
}

// A function that creates a window using the Windows API
// This will return a pointer to that window
HWND WinCreateWindow(HINSTANCE hInstance)
{
    // Setting the flags to help configure the window
    // https://learn.microsoft.com/en-us/windows/win32/winmsg/window-styles
    DWORD windowFlags = WS_OVERLAPPED | WS_SYSMENU;

    // Create the window from the registered class
    HWND hwnd = CreateWindowA(
        windowClassName,          // The "blueprint" we want to use
        "Tiffany's Student Grade Calculator", // The window title
        windowFlags,              // The flags we configured
        CW_USEDEFAULT,            // X position of the window
        CW_USEDEFAULT,            // Y position of the window
        windowWidth,                      // Width of the window
        windowHeight,                      // Height of the window
        NULL,                     // Unused (hWndParent)
        NULL,                     // Unused (hMenu)
        hInstance,                // Instance handle from Windows
        NULL                      // Unused (lpParam - We can pass optional data here that'll be picked up on WM_CREATE msg)
    );

    // Return the pointer to that window
    return hwnd;
}

// Main entry point
// The parameters are passed in by the kernel
// The kernel is a core component of an Operating System that manages the operations of the computer and its hardware
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    // Creating the "blueprint" for our window
    WinRegisterWindowClass(hInstance);

    // Create the font and button brush 
    CreateStyles();

    // hwnd points to the window that has been created
    mainWindowHandle = WinCreateWindow(hInstance);

    // If hwnd is NULL, no window has been created, print an error message and return
    if (mainWindowHandle == NULL)
    {
        printf("Failed to create window..\n");
        return -1;
    }

    // Create the menu bar and the screens
    CreateMenuBar(mainWindowHandle);
    CreateScreens(mainWindowHandle);

    // Update the window
    UpdateWindow(mainWindowHandle);

    // Show the window by calling to the Windows API, otherwise it would not show
    ShowWindow(mainWindowHandle, nCmdShow);
    ShowWindow(studentListWindowHandle, SW_SHOW);

    // Defining a message variable that will contain message data e.g. "the window is closed"
    // https://learn.microsoft.com/en-us/windows/win32/api/winuser/ns-winuser-msg
    MSG windowMessage{};

    // Loop until we no longer have messages to process
    // GetMessage is blocking (or waiting), so won't return until a message is got ( https://learn.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-getmessage )
    while (GetMessage(&windowMessage, mainWindowHandle, 0, 0) != 0)
    {
        // Translates virtual-key messages into a more understanding format
        TranslateMessage(&windowMessage);
        // Dispatches the message, which gets picked up by the procedure (WinWindowCallback)
        DispatchMessage(&windowMessage);

        // Ensure we no longer process messages if we are no longer running
        if (!Running)
        {
            break;
        }
    }

    // Print a message to say the program is closing
    printf("Closing window. Exiting app..\n");
    
    return 0;
}