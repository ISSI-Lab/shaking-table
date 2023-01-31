from .abstract_file import AbstractFile
#import sys, os


class StateInfo(AbstractFile):
    def __init__(self, state = "INIT"):
        super().__init__()
        self.state = state
        self.state_filename = "state.cfg"
        
    def getState(self):
        return self.state
        
    def saveState(self, state):
        self.state = state
        self.file_save(self.state_filename, state, self.data_dir, "wt")
    
    def getStateByFile(self):
        state = self.file_get_content(self.state_filename, self.data_dir, "rt")
        state = state.trim()
        self.state = state
        return self.state