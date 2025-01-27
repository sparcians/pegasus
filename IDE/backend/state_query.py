import sqlite3

class StateQuery:
    def __init__(self, db_file):
        self.conn = sqlite3.connect(db_file)
        self.cursor = self.conn.cursor()
        self.init_reg_values = None

    def GetInstructions(self):
        cmd = 'SELECT PC, Opcode, Dasm, InstUID FROM Instructions'
        self.cursor.execute(cmd)
        return self.cursor.fetchall()

    def GetSnapshot(self, pc):
        pc = int(pc, 16) if isinstance(pc, str) else pc
        reg_vals = self.__GetInitRegValues()

        class RegVal:
            def __init__(self, val):
                self.prev_val = None
                self.curr_val = val

            def SetValue(self, val):
                self.prev_val = self.curr_val
                self.curr_val = val

        reg_vals = {reg_name: RegVal(reg_val) for reg_name, reg_val in reg_vals.items()}

        cmd = 'SELECT ElemName, ElemVal, InstUID FROM InstChanges'
        self.cursor.execute(cmd)

        changes_by_inst_uid = {}
        for elem_name, curr_val, inst_uid in self.cursor.fetchall():
            if inst_uid not in changes_by_inst_uid:
                changes_by_inst_uid[inst_uid] = {}
            changes_by_inst_uid[inst_uid][elem_name] = curr_val

        cmd = 'SELECT Opcode, Dasm, InstUID FROM Instructions WHERE PC = {}'.format(pc)
        self.cursor.execute(cmd)
        opcode, dasm, inst_uid = self.cursor.fetchone()

        cmd = 'SELECT PC,InstUID FROM Instructions'
        self.cursor.execute(cmd)
        for inst_pc, uid in self.cursor.fetchall():
            # Don't replay the simulation too far
            if inst_pc > pc:
                break

            if uid in changes_by_inst_uid:
                for elem_name, curr_val in changes_by_inst_uid[uid].items():
                    reg_vals[elem_name].SetValue(curr_val)

        changes = changes_by_inst_uid.get(inst_uid, {})
        changes = {reg_name: (reg_vals[reg_name].prev_val, reg_vals[reg_name].curr_val) for reg_name in changes}
        reg_vals = {reg_name: reg_val.curr_val for reg_name, reg_val in reg_vals.items()}

        return StateSnapshot(pc, opcode, dasm, changes, reg_vals)

    def GetInitialState(self):
        cmd = 'SELECT MIN(PC) FROM Instructions'
        self.cursor.execute(cmd)
        pc = self.cursor.fetchone()[0]
        reg_vals = self.__GetInitRegValues()

        return StateSnapshot(pc, None, None, None, reg_vals)

    def GetMnemonic(self, pc):
        pc = int(pc, 16) if isinstance(pc, str) else pc
        cmd = 'SELECT Dasm FROM Instructions WHERE PC = {}'.format(pc)
        self.cursor.execute(cmd)
        dasm = self.cursor.fetchone()[0]
        return dasm.lstrip().split()[0]

    def __GetInitRegValues(self):
        if self.init_reg_values is not None:
            return self.init_reg_values.copy()

        cmd = 'SELECT RegName, RegValue FROM InitRegValues'
        self.cursor.execute(cmd)
        self.init_reg_values = dict(self.cursor.fetchall())

        return self.init_reg_values.copy()

class StateSnapshot:
    def __init__(self, pc, opcode, dasm, changes, reg_vals):
        self.pc = pc
        self.opcode = opcode
        self.dasm = dasm
        self.changes = changes
        self.reg_vals = reg_vals

    def getPC(self):
        return self.pc

    def getOpcode(self):
        return self.opcode

    def dasmString(self):
        return self.dasm

    def getChanges(self):
        return self.changes

    def getRegValue(self, reg_name):
        return self.reg_vals[reg_name]
