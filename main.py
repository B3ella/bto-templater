from datetime import datetime, timedelta
import os

def main():
    daily_note_location = get_daily_note_location()
    template_location = get_template()

    if os.path.isfile(daily_note_location):
        print("daily note already exists")
        return

    template_content = ""
    with open(template_location, "r") as file:
        template_content = file.read()

    new_content = process(template_content)

    with open(daily_note_location, "w") as file:
        file.write(new_content)

def get_daily_note_location():
    notes_dir = get_notes_dir()

    today = datetime.today()
    daily_note_name = today.strftime('%Y-%m-%d')

    daily_note_location = notes_dir + daily_note_name + ".md"

    return daily_note_location

def get_template():
    notes_dir = get_notes_dir()
    return notes_dir + "templates/Daily note template.md"
    
def get_notes_dir():
    return "/home/bella/Notes/"

def process(template_content):
    new_content = check_for_week_day(template_content)
    new_content = check_for_daily_sensitive_content(new_content)
    return new_content

def check_for_daily_sensitive_content(template_content):
    marker = "!by_weekday ["
    marker_start = template_content.find(marker)
    marker_end = template_content.find("]", marker_start)

    if marker_start < 0:
        return template_content.strip()

    possibilities = template_content[
        marker_start + len(marker):
        marker_end
    ].split(";")
    
    weekday = get_weekday()
    chosen = possibilities[weekday].strip()

    new_content = string_without(
        template_content, 
        marker_start, 
        marker_end
    )
    new_content = insert_into_string(new_content, chosen, marker_start)
    return check_for_daily_sensitive_content(new_content)


def check_for_week_day(template_content):

    weekday = get_weekday()
    is_weekday = weekday < 5

    marker_start = template_content.find("!weekday_only")
    line_end = template_content.find("\n", marker_start)

    if marker_start < 0:
        return template_content.strip()

    new_content = string_without(
        template_content,
        marker_start,
        line_end
    )

    if is_weekday:
        marker_end = template_content.find(" ", marker_start)
        new_content = string_without(
            template_content, 
            marker_start, 
            marker_end
        )
    return check_for_week_day(new_content)

 

def string_without(string, start, end):
    if end < start:
        return string[:start]
  
    return string[:start] + string[end+1:]

def get_weekday():
    return datetime.today().weekday()

def insert_into_string(string, new, idx):
    start = string[:idx]
    end = string[idx:]
    return start + new + end


if __name__ == "__main__":
    main()
