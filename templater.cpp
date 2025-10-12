#include <bits/stdc++.h>

#include <iostream>
#include <fstream>
#include <ctime>

#include <stdio.h>
#include <string>

#include <sys/stat.h>

#define vstr(s) str(s)
#define str(s) #s

using namespace std;

string stringfy(int value, int size){
    string str = to_string(value);
    if (str.length() >= size) {
        return str;
    }
    int pad_size = size - str.length();
    string pad = string(pad_size, '0');
    return pad + str;
}

string get_note_name(){
    time_t timestamp = time(&timestamp);
    struct tm today = *localtime(&timestamp);
    string year = to_string(1900 + today.tm_year);
    string month = stringfy(today.tm_mon + 1, 2);
    string day = stringfy(today.tm_mday, 2);
    string result = year + "-" + month + "-" + day + ".md";
    return result;
}

void print(string str){
    cout << str;
    cout << "\n";
}

bool has_tolken(string line){
    return line.find("!", 0) == 0;
}

bool is_session(string line){
    return line.find("#", 0) == 0;
}

string get_tolken(string line){
    int tolken_end = line.find(" ", 0);
    return line.substr(0, tolken_end);
};

string remove_tolken(string line){
    int tolken_end = line.find(" ", 0);
    return line.substr(tolken_end + 1);
}

int get_weekday(){
    time_t timestamp = time(&timestamp);
    struct tm today = *localtime(&timestamp);
    int day = today.tm_wday;
    return day;
}

bool is_weekday(){
    int day = get_weekday();
    return day != 0 && day != 6;
}

string weekday_only(string line){
    if (is_weekday()){
        return remove_tolken(line);
    }
    return "";
}

string by_weekday(string line){
    string options[7] = {};

    int start = line.find("[") + 1;
    int end = line.find(";", start);
    for (int i = 0; i < 7; i++){
        string option = line.substr(start, end-start);
        options[i] = option;

        start = end + 1;
        end = line.find(";", start);

        if (end < 0){
            end = line.find("]");
        }
    }

    int day = get_weekday();
    return options[day];
}
string get_yesterdays_note_name(){
    time_t timestamp = time(&timestamp) - 60 * 60 * 24;
    struct tm yesterday = *localtime(&timestamp);

    string year = to_string(1900 + yesterday.tm_year);
    string month = stringfy(yesterday.tm_mon + 1, 2);
    string day = stringfy(yesterday.tm_mday, 2);

    string result = year + "-" + month + "-" + day + ".md";
    return result;
};

string copy_last_day(string current_session){
    string notes_dir = "/home/bella/Notes/";

    string note_name = get_yesterdays_note_name();
    string note_location = notes_dir + note_name;

    ifstream note(note_location);
    string line;
    string result;
    bool is_target_session = false;
    while (getline(note, line)){
        if (is_session(line)){
            is_target_session = false;
        };
        if (line == current_session){
            is_target_session = true;
            continue;
        };
        if (!is_target_session){
            continue;
        };
        result.append(line);
        result.append("\n");
    }
    note.close();
    return result;
};

string get_content(string line, string current_session){
    string tolken = get_tolken(line);

    if (tolken == "!weekday_only"){
        string result = weekday_only(line);
        result.append("\n");
        return result;
    };
    if (tolken == "!by_weekday"){
        string result = by_weekday(line);
        result.append("\n");
        return result;
    };
    if (tolken == "!copy_last_day"){
        return copy_last_day(current_session);
    };
    return "";
}

int main() {
    string notes_dir = "/home/bella/Notes/";

    string template_location = notes_dir + "templates/Daily note template.md";
    string note_name = get_note_name();
    string note_location = notes_dir + note_name;

    struct stat buffer;   
    bool note_exists = (stat (note_location.c_str(), &buffer) == 0); 
    if (note_exists){
        cout << "note exists";
        return 0;
    };

    ifstream template_content(template_location);
    ofstream note(note_location);
    string line;
    string current_session = note_name;
    while (getline(template_content, line)){
        if (is_session(line)){
            current_session = line;
        };
        if(has_tolken(line)){
            note << get_content(line, current_session);
            continue;
        };
        note << line << "\n";
    }
    note.close();
    template_content.close();
    return 0;
};
