//this file is part of notepad++
//Copyright (C)2022 Don HO <don.h@free.fr>
//
//This program is free software; you can redistribute it and/or
//modify it under the terms of the GNU General Public License
//as published by the Free Software Foundation; either
//version 2 of the License, or (at your option) any later version.
//
//This program is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//GNU General Public License for more details.
//
//You should have received a copy of the GNU General Public License
//along with this program; if not, write to the Free Software
//Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

#include "PluginDefinition.h"
#include "menuCmdID.h"
#include "Scintilla.h"

#include <algorithm>
#include <cctype>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

std::string newLine = "\r\n";

//
// The plugin data that Notepad++ needs
//
FuncItem funcItem[nbFunc];

//
// The data of Notepad++ that you can use in your plugin commands
//
NppData nppData;

namespace {

struct ItemStats {
    std::string displayItem;
    int appearancesInList1 = 0;
    int appearancesInList2 = 0;
};

std::string trimRight(std::string s)
{
    while (!s.empty()) {
        const char ch = s.back();
        if (ch == '\0' || ch == '\r' || ch == '\n' || ch == ' ' || ch == '\t')
            s.pop_back();
        else
            break;
    }
    return s;
}

std::string trimLeft(std::string s)
{
    while (!s.empty()) {
        const char ch = s.front();
        if (ch == ' ' || ch == '\t')
            s.erase(s.begin());
        else
            break;
    }
    return s;
}

std::string toLowerCopy(const std::string& s)
{
    std::string out = s;
    for (char& ch : out)
        ch = static_cast<char>(std::tolower(static_cast<unsigned char>(ch)));
    return out;
}

bool equalsIgnoreCase(const std::string& a, const std::string& b)
{
    return toLowerCopy(a) == toLowerCopy(b);
}

bool lessIgnoreCase(const std::string& a, const std::string& b)
{
    const size_t minSize = ((a.size()) < (b.size())) ? (a.size()) : (b.size());
    for (size_t i = 0; i < minSize; ++i) {
        const unsigned char ca = static_cast<unsigned char>(std::tolower(static_cast<unsigned char>(a[i])));
        const unsigned char cb = static_cast<unsigned char>(std::tolower(static_cast<unsigned char>(b[i])));
        if (ca < cb)
            return true;
        if (ca > cb)
            return false;
    }
    return a.size() < b.size();
}

std::string formatOneCountLine(int count, const std::string& item)
{
    std::ostringstream oss;
    oss << "";
    if (count == 1) {
        oss << "  " << std::setw(1) << std::setfill('0') << count << "   " << item;
    }
    else {
        oss << " " << std::setw(2) << std::setfill('0') << count << "   " << item;
    }
    return oss.str();
}

std::string formatTwoCountLine(int count1, int count2, const std::string& item)
{
    std::ostringstream oss;
    oss << "";
    if (count1 == 1) {
        oss << "  " << std::setw(1) << std::setfill('0') << count1;
    }
    else {
		oss << " " << std::setw(2) << std::setfill('0') << count1;
    }
    if (count2 == 1) {
        oss << "   " << std::setw(1) << std::setfill('0') << count2;
    }
    else {
        oss << "  " << std::setw(2) << std::setfill('0') << count2;
    }
    oss << "   " << item;
    return oss.str();
}

std::string formatSectionHeaderCommon(size_t count)
{
    std::ostringstream oss;
    oss << "\r\n\r\n=====================\r\n"
        << "     COMMON (" << count << ')'
        << "\r\n====================="
        << "\r\n L1  L2"
        << "\r\n---------";
    return oss.str();
}

std::string formatSectionHeaderSingle(const std::string& title, size_t count)
{
    std::ostringstream oss;
    oss << "\r\n=====================\r\n"
        << title << " (" << count << ')'
        << "\r\n====================="
        << "\r\n  #"
        << "\r\n-----";
    return oss.str();
}

} // namespace

//
// Initialize your plugin data here
// It will be called while plugin loading
void pluginInit(HANDLE /*hModule*/)
{
}

//
// Here you can do the clean up, save the parameters (if any) for the next session
//
void pluginCleanUp()
{
}

//
// Initialization of your plugin commands
// You should fill your plugins commands here
void commandMenuInit()
{
    // with function :
    // setCommand(int index,                      // zero based number to indicate the order of command
    //            TCHAR *commandName,             // the command name that you want to see in plugin menu
    //            PFUNCPLUGINCMD functionPointer, // the symbol of function (function pointer) associated with this command. The body should be defined below. See Step 4.
    //            ShortcutKey *shortcut,          // optional. Define a shortcut to trigger this command
    //            bool check0nInit                // optional. Make this menu item be checked visually
    //            );

    ShortcutKey* shKey = new ShortcutKey;
    shKey->_isAlt = true;
    shKey->_isCtrl = false;
    shKey->_isShift = false;
    shKey->_key = 0x4C; // VK_L

    setCommand(0, TEXT("CrossCheck"), compareLists, shKey, false);
}

//
// Here you can do the clean up (especially for the shortcut)
//
void commandMenuCleanUp()
{
    delete funcItem[0]._pShKey;
}

//
// This function help you to initialize your plugin commands
//
bool setCommand(size_t index, TCHAR* cmdName, PFUNCPLUGINCMD pFunc, ShortcutKey* sk, bool check0nInit)
{
    if (index >= nbFunc)
        return false;

    if (!pFunc)
        return false;

    lstrcpy(funcItem[index]._itemName, cmdName);
    funcItem[index]._pFunc = pFunc;
    funcItem[index]._init2Check = check0nInit;
    funcItem[index]._pShKey = sk;

    return true;
}

//----------------------------------------------//
//--         ASSOCIATED FUNCTIONS             --//
//----------------------------------------------//
void showDebugMessage_str(std::string str)
{
    const int wlen = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, NULL, 0);
    if (wlen <= 0) {
        ::MessageBoxA(NULL, str.c_str(), "Plugin Message", MB_OK);
        return;
    }

    std::wstring wline(static_cast<size_t>(wlen), L'\0');
    MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, &wline[0], wlen);
    ::MessageBox(NULL, wline.c_str(), TEXT("Plugin Message"), MB_OK);
}

void readLine(HWND curScint, size_t lineNumber, std::string& buffer)
{
    const int len = static_cast<int>(::SendMessage(curScint, SCI_LINELENGTH, lineNumber, 0));

    if (len <= 0) {
        buffer.clear();
        return;
    }

    buffer.resize(static_cast<size_t>(len), '\0');
    ::SendMessage(curScint, SCI_GETLINE, lineNumber, reinterpret_cast<LPARAM>(buffer.data()));
}

HWND getCurrentScintilla()
{
    int which = -1;
    SendMessage(nppData._nppHandle, NPPM_GETCURRENTSCINTILLA, 0, reinterpret_cast<LPARAM>(&which));

    return (which == 0) ? nppData._scintillaMainHandle : nppData._scintillaSecondHandle;
}

void writeFileContentIntoCurrentScintilla_lineByLine(HWND scintilla, const std::wstring& path)
{
    std::ifstream file(path);
    if (!file.is_open()) {
        showDebugMessage_str("ERROR: Could not open the file from writeFileContentIntoCurrentScintilla_lineByLine");
        return;
    }

    ::SendMessage(scintilla, SCI_BEGINUNDOACTION, 0, 0);

    std::string line;
    while (std::getline(file, line)) {
        const std::string text = line + newLine;
        ::SendMessage(scintilla, SCI_APPENDTEXT, static_cast<WPARAM>(text.size()), reinterpret_cast<LPARAM>(text.c_str()));
    }

    ::SendMessage(scintilla, SCI_SETSEL, static_cast<WPARAM>(-1), static_cast<LPARAM>(-1));
    ::SendMessage(scintilla, SCI_ENDUNDOACTION, 0, 0);
}

void writeTextIntoCurrentScintilla(HWND scintilla, const std::string& text)
{
    const std::string textWithNewLine = text + newLine;
    ::SendMessage(scintilla, SCI_APPENDTEXT, static_cast<WPARAM>(textWithNewLine.size()), reinterpret_cast<LPARAM>(textWithNewLine.c_str()));
    ::SendMessage(scintilla, SCI_SETSEL, static_cast<WPARAM>(-1), static_cast<LPARAM>(-1));
}

void writeTextArrayIntoCurrentScintilla_lineByLine(HWND scintilla, const std::vector<std::string>& lines, bool isolateCTRL_Z)
{
    if (isolateCTRL_Z)
        ::SendMessage(scintilla, SCI_BEGINUNDOACTION, 0, 0);

    for (const std::string& line : lines) {
        const std::string text = line + newLine;
        ::SendMessage(scintilla, SCI_APPENDTEXT, static_cast<WPARAM>(text.size()), reinterpret_cast<LPARAM>(text.c_str()));
    }

    ::SendMessage(scintilla, SCI_SETSEL, static_cast<WPARAM>(-1), static_cast<LPARAM>(-1));

    if (isolateCTRL_Z)
        ::SendMessage(scintilla, SCI_ENDUNDOACTION, 0, 0);
}

std::vector<std::string> sortArray(const std::vector<std::string>& v)
{
    std::vector<std::string> sorted = v;
    std::sort(sorted.begin(), sorted.end(), lessIgnoreCase);
    return sorted;
}

std::vector<std::string> readNonEmptyBlock(HWND sci, size_t& line_no, size_t lineCount)
{
    std::vector<std::string> lines;
    std::string line;

    while (line_no < lineCount) {
        readLine(sci, line_no, line);
        line = trimRight(line);
        line = trimLeft(line);
        if (!line.empty())
            break;
        ++line_no;
    }

    while (line_no < lineCount) {
        readLine(sci, line_no, line);
        line = trimRight(line);
        line = trimLeft(line);
        if (line.empty())
            break;
        lines.push_back(line);
        ++line_no;
    }

    return lines;
}

void compareLists()
{
    HWND sci = getCurrentScintilla();
    const size_t lineCount = static_cast<size_t>(::SendMessage(sci, SCI_GETLINECOUNT, 0, 0));
    size_t line_no = 0;

    // Build the lists deined by the user
    const std::vector<std::string> list1 = readNonEmptyBlock(sci, line_no, lineCount);
    const std::vector<std::string> list2 = readNonEmptyBlock(sci, line_no, lineCount);

	// Sort the lists
    const std::vector<std::string> list1Sorted = sortArray(list1);
    const std::vector<std::string> list2Sorted = sortArray(list2);

	//  Build a map of all items with their stats (number of appearances in each list) //
    std::unordered_map<std::string, ItemStats> itemsTable;
    itemsTable.reserve(list1.size() + list2.size());

	//  - Add the items from list 1
    for (const std::string& item : list1) {
		ItemStats& stats = itemsTable[toLowerCopy(item)]; // this will default construct the stats if the item is not already in the map
        if (stats.displayItem.empty())
            stats.displayItem = item;
        ++stats.appearancesInList1;
    }
    //  - Add the items from list 2
    for (const std::string& item : list2) {
		ItemStats& stats = itemsTable[toLowerCopy(item)]; // this will default construct the stats if the item is not already in the map
        if (stats.displayItem.empty())
            stats.displayItem = item;
        ++stats.appearancesInList2;
    }

    std::vector<std::string> orderedKeys;
    orderedKeys.reserve(itemsTable.size());
    std::unordered_set<std::string> seenKeys;
    seenKeys.reserve(itemsTable.size());

    for (const std::string& item : list1Sorted) {
        const std::string key = toLowerCopy(item);
        if (seenKeys.insert(key).second)
            orderedKeys.push_back(key);
    }
    for (const std::string& item : list2Sorted) {
        const std::string key = toLowerCopy(item);
        if (seenKeys.insert(key).second)
            orderedKeys.push_back(key);
    }

    std::vector<std::string> list1Only;
    std::vector<std::string> list2Only;
    std::vector<std::string> listCommon;

    list1Only.reserve(itemsTable.size());
    list2Only.reserve(itemsTable.size());
    listCommon.reserve(itemsTable.size());

    for (const std::string& key : orderedKeys) {
        const ItemStats& stats = itemsTable.at(key);

        if (stats.appearancesInList1 > 0 && stats.appearancesInList2 == 0)
            list1Only.push_back(formatOneCountLine(stats.appearancesInList1, stats.displayItem));

        if (stats.appearancesInList2 > 0 && stats.appearancesInList1 == 0)
            list2Only.push_back(formatOneCountLine(stats.appearancesInList2, stats.displayItem));

        if (stats.appearancesInList1 > 0 && stats.appearancesInList2 > 0)
            listCommon.push_back(formatTwoCountLine(stats.appearancesInList1, stats.appearancesInList2, stats.displayItem));
    }

    writeTextIntoCurrentScintilla(sci, formatSectionHeaderCommon(listCommon.size()));
    writeTextArrayIntoCurrentScintilla_lineByLine(sci, listCommon, false);

    writeTextIntoCurrentScintilla(sci, formatSectionHeaderSingle("   LIST 1 ONLY", list1Only.size()));
    writeTextArrayIntoCurrentScintilla_lineByLine(sci, list1Only, false);

    writeTextIntoCurrentScintilla(sci, formatSectionHeaderSingle("   LIST 2 ONLY", list2Only.size()));
    writeTextArrayIntoCurrentScintilla_lineByLine(sci, list2Only, false);

    writeTextIntoCurrentScintilla(sci, ""); //Extra empty line at the end
}
