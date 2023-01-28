import os, sys

class AbstractFile(object):
    
    def __init__(self):
        self.app_root = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
        self.data_dir = os.path.join(self.app_root, "data")       
        self.upload_dir = os.path.join(self.app_root, "data", "uploads")
        print(self.upload_dir, sys.stderr)

    def get_data_dir(self):
        return self.data_dir
    
    def get_upload_dir(self):
        return self.upload_dir

    def get_app_root(self):
        return self.app_root
          
    def file_save(self, filename, content, path="", mode = "wt"):
        print("saving filename " + str(filename), file=sys.stderr)
        if path == "":
            path = self.upload_dir
        fout = open(os.path.join(path, filename), mode)
        fout.write(content)
        fout.close()
        return 1

    def file_get_content(self, filename, path = "", mode = "rt"):
        print("getting filename >" + str(path) + "< " + str(filename), file=sys.stderr)
        try:
            if path == "":
                path = self.upload_dir
                print("getting filename >>" + str(os.path.join(path, filename)) + "<< " + str(filename), file=sys.stderr)
            f = open(os.path.join(path, filename), mode)
        except OSError:
            print("No such file!!! " + str(filename), file=sys.stderr)
            return ""
        content = f.read()
        f.close()
        return content