import sqlite3, tempfile, os

def quote(s):
    s = s.strip("'").strip('"')
    return "'" + s + "'"

def dquote(s):
    s = s.strip("'").strip('"')
    return '"' + s + '"'

class StateDB:
    def __init__(self):
        self.db_file = tempfile.NamedTemporaryFile(delete=False)
        self.__conn = None

    @property
    def conn(self):
        if not self.__conn:
            self.__conn = sqlite3.connect(self.db_file.name)
            self.__CreateSchema()

        return self.__conn

    @property
    def cursor(self):
        return self.conn.cursor()

    def SetInitRegValue(self, reg_name, reg_value):
        cmd = 'INSERT INTO InitRegValues (RegName, RegValue) VALUES (%s, %d)' % (dquote(reg_name), reg_value)
        self.cursor.execute(cmd)

    def AppendInstruction(self, pc, opcode, dasm, inst_uid):
        dasm = dasm.replace('\t', '    ')
        cmd = 'INSERT INTO Instructions (PC, Opcode, Dasm, InstUID) VALUES ({}, {}, {}, {})'
        cmd = cmd.format(pc, opcode, quote(dasm), inst_uid)
        self.cursor.execute(cmd)

    def AppendRegChange(self, inst_uid, elem_name, elem_val):
        cmd = 'INSERT INTO InstChanges (InstUID, ElemName, ElemVal) VALUES ({}, {}, {})'
        cmd = cmd.format(inst_uid, dquote(elem_name), elem_val)
        self.cursor.execute(cmd)

    def Close(self):
        if self.__conn:
            self.__conn.close()
            self.__conn = None

        if self.db_file:
            self.db_file.close()
            os.unlink(self.db_file.name)
            self.db_file = None

    def CallInTransaction(self, func):
        self.cursor.execute('BEGIN TRANSACTION')
        try:
            func()
            self.cursor.execute('COMMIT')
        except:
            self.cursor.execute('ROLLBACK')
            raise

    def __del__(self):
        self.Close()

    def __CreateSchema(self):
        self.cursor.execute('''
            CREATE TABLE IF NOT EXISTS InitRegValues (
                id INTEGER PRIMARY KEY AUTOINCREMENT,
                RegName TEXT,
                RegValue INTEGER
            )
        ''')

        self.cursor.execute('''
            CREATE TABLE IF NOT EXISTS Instructions (
                id INTEGER PRIMARY KEY AUTOINCREMENT,
                PC INTEGER,
                Opcode INTEGER,
                Dasm TEXT,
                InstUID INTEGER
            )
        ''')

        self.cursor.execute('''
            CREATE TABLE IF NOT EXISTS InstChanges (
                id INTEGER PRIMARY KEY AUTOINCREMENT,
                InstUID INTEGER,
                ElemName TEXT,
                ElemVal INTEGER
            )
        ''')
