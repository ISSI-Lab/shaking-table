from .abstract_file import AbstractFile
import sys, csv, os


class InputValidator(AbstractFile):
    def __init__(self):
        super().__init__()
    
    def replaceInputWithTemp(self, input_file):
        input_file_tmp = input_file + ".tmp"
        if os.path.isfile(input_file):
            os.remove(input_file)
        os.rename(input_file_tmp, input_file)
    
    def reformat(self, input_file):
        input_file_tmp = input_file + ".tmp"
        fieldnames = ['timestamp', 'x_distance', 'y_distance']
        dict_list = []
        with open(input_file, mode='rt') as csv_file:
            csv_reader = csv.DictReader(csv_file)
            for row in csv_reader:
                row1 = {}
                for k, v in row.items():
                    row1[k.strip()] = v.strip()
                dict_list.append(row1)
     
        with open(input_file_tmp, mode='wt') as csv_file:
            #fieldnames = ['emp_name', 'dept', 'birth_month']
            #fieldnames = ['timestamp', 'x_distance', 'y_distance']
            writer = csv.DictWriter(csv_file, fieldnames=fieldnames)
            writer.writeheader()
            for row in dict_list:
                writer.writerow(row)
        
        self.replaceInputWithTemp(input_file)
  
    def validate(self, input_file):
        is_valid = True
        allowed_titles = ['timestamp', 'x_distance', 'y_distance']
        with open(input_file, mode='rt') as csv_file:
            csv_reader = csv.DictReader(csv_file)
            #print(str(csv_reader), file=sys.stderr)
            line_count = 0
            for row in csv_reader:
                if line_count == 0:
                    for head_titles, val in row.items():
                        head_titles = head_titles.strip()
                        if head_titles not in allowed_titles:
                            is_valid = False
                            break
        return is_valid