import sqlite3, tempfile, os
from backend.sim_api import FormatHex

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
        if reg_name != 'resv_priv':
            reg_value = FormatHex(reg_value)
            reg_value = dquote(reg_value)

        cmd = 'INSERT INTO InitRegValues (RegName, RegValue) VALUES ({}, {})'
        cmd = cmd.format(dquote(reg_name), reg_value)
        self.cursor.execute(cmd)

    def AppendInstruction(self, pc, opcode, dasm, inst_uid):
        assert pc is not None
        assert inst_uid is not None

        if opcode is None:
            opcode = 0

        if dasm is None:
            dasm = ''

        pc = FormatHex(pc)
        opcode = FormatHex(opcode)
        dasm = dasm.replace('\t', '    ')
        cmd = 'INSERT INTO Instructions (PC, Opcode, Dasm, InstUID) VALUES ({}, {}, {}, {})'
        cmd = cmd.format(dquote(pc), dquote(opcode), dquote(dasm), inst_uid)
        self.cursor.execute(cmd)

    def AppendRegChange(self, inst_uid, elem_name, elem_val, expected_elem_val):
        elem_val = FormatHex(elem_val)
        expected_elem_val = FormatHex(expected_elem_val)
        cmd = 'INSERT INTO InstChanges (InstUID, ElemName, ElemVal, ExpectedElemVal) VALUES ({}, {}, {}, {})'
        cmd = cmd.format(inst_uid, dquote(elem_name), dquote(elem_val), dquote(expected_elem_val))
        self.cursor.execute(cmd)

    def AppendMemAccesses(self, inst_uid, mem_reads, mem_writes):
        for read in mem_reads:
            mem_addr = read.addr
            mem_value = read.value
            cmd = 'INSERT INTO MemoryAccesses (InstUID, Addr, Value) VALUES ({}, {}, {})'
            cmd = cmd.format(inst_uid, dquote(mem_addr), dquote(mem_value))
            self.cursor.execute(cmd)

        for write in mem_writes:
            mem_addr = write.addr
            mem_prior = write.prior
            mem_value = write.value
            cmd = 'INSERT INTO MemoryAccesses (InstUID, Addr, Value, Prior) VALUES ({}, {}, {}, {})'
            cmd = cmd.format(inst_uid, dquote(mem_addr), dquote(mem_prior), dquote(mem_value))
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
                RegValue TEXT
            )
        ''')

        self.cursor.execute('''
            CREATE TABLE IF NOT EXISTS Instructions (
                id INTEGER PRIMARY KEY AUTOINCREMENT,
                PC TEXT,
                Opcode TEXT,
                Dasm TEXT,
                InstUID INTEGER
            )
        ''')

        self.cursor.execute('''
            CREATE TABLE IF NOT EXISTS InstChanges (
                id INTEGER PRIMARY KEY AUTOINCREMENT,
                InstUID INTEGER,
                ElemName TEXT,
                ElemVal TEXT,
                ExpectedElemVal TEXT
            )
        ''')

        self.cursor.execute('''
            CREATE TABLE IF NOT EXISTS MemoryAccesses (
                id INTEGER PRIMARY KEY AUTOINCREMENT,
                InstUID INTEGER,
                Addr TEXT,
                Value TEXT,
                Prior TEXT DEFAULT 'N/A'
            )
        ''')

        self.cursor.execute('''
            CREATE INDEX IF NOT EXISTS MemoryAccesses_InstUID ON MemoryAccesses (InstUID)
        ''')

        self.cursor.execute('''
            CREATE INDEX IF NOT EXISTS Instructions_PC ON Instructions (PC)
        ''')
