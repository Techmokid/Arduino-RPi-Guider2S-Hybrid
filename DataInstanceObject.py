if __name__ == "__main__":
    import Main
    exit()

import time

class DataObject:
    _instance = None

    def __new__(cls):
        if cls._instance is None:
            cls._instance = super(DataObject, cls).__new__(cls)
            cls._instance._initialized = False
        return cls._instance

    def __init__(self):
        if self._initialized:
            return
        
        self._initialized = True
        self.storedFunction = None
        self.storedData = []
        self.data = None

    def update(self):
        if self.storedFunction == None:
            return
        
        self.storedFunction()

    
