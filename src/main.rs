use std::fs;
use std::fs::File;
use std::io::prelude::*;
use chrono::{Local, DateTime, Duration, Datelike};  

fn main() {
    let current_local: DateTime<Local> = Local::now();  
    create_daily_note(current_local);
}

fn create_daily_note(date: DateTime<Local>) {
    if note_exists(date) {
        println!("Daily note {} already exists", date_to_file_name(date));
        return
    }

    let yesterday = date - Duration::days(1);
    create_daily_note(yesterday);

    let yesterday = date_to_file_name(yesterday);
    let yesterday = read_file(&yesterday);

    let template = read_file("/home/bella/Notes/templates/dnt.md");

    let note = process_tokens(&template, &yesterday);
    let path = date_to_file_name(date);

    let file = File::create(path);
    let _ = file.expect("File createon failed").write_all(note.as_bytes());
}

fn note_exists(date: DateTime<Local>) -> bool {
    let note_name = date_to_file_name(date);
    let result = fs::exists(note_name).expect("The file system is throwing an error?");
    return result
}

fn date_to_file_name(date: DateTime<Local>) -> String {
    let year = date.year();
    let month = date.month();
    let day = date.day();
    let file_name = format!("{}-{:0>2}-{:0>2}.md", year, month, day);
    let base_path = "/home/bella/Notes/".to_string();
    base_path + &file_name
}

fn read_file(path: &str) -> String {
    let file = File::open(path);
    let mut contents = String::new();
    let _ = file.expect("Failed to read file {}").read_to_string(&mut contents);
    return contents
} 

fn process_tokens(template: &str, yesterday: &str) -> String {
    let mut result = "".to_string();
    let mut current_section = "";
    let dont_copy = ["- [x]"];
    for line in template.lines() {
        if line.starts_with("#") {
            current_section = line;
        }
        if line.contains("!copy_last_day") {
            result += &get_section_text(&current_section, &yesterday, &dont_copy);
            result += "\n";
            continue
        }
        result += line;
        result += "\n";
    }
    return result
}
fn get_section_text(section: &str, yesterday: &str, dont_copy: &[&str; 1]) -> String {
    let mut result = "".to_string();
    let mut copy = false;
    for line in yesterday.lines() {
        if line.starts_with("#") {
            copy = false;
        }
        if copy &! should_skip(line, dont_copy) {
            result += line;
            result += "\n";
        }
        if line.starts_with(&section) {
            copy = true;
        }
    }
    return result
}

fn should_skip(line: &str, dont_copy: &[&str; 1]) -> bool {
    for token in dont_copy {
        if line.starts_with(token) {
            return true
        };
    };
    return false
}


#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test() {
        let template = read_file("test_data/template.md");
        let yesterday = read_file("test_data/yesterday.md");

        let result  = process_tokens(&template, &yesterday);

        let expected = read_file("test_data/expected.md");
        assert_eq!(result, expected);
    }
}
