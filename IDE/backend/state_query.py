import sqlite3
from backend.sim_api import FormatHex

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
        pc = FormatHex(pc)
        reg_vals = self.__GetInitRegValues()

        class RegVal:
            def __init__(self, val):
                self.prev_val = None
                self.curr_val = FormatHex(val)

            def SetValue(self, val):
                self.prev_val = self.curr_val
                self.curr_val = FormatHex(val)

        reg_vals = {reg_name: RegVal(reg_val) for reg_name, reg_val in reg_vals.items()}
        cmd = 'SELECT ElemName, ElemVal, ExpectedElemVal, InstUID FROM InstChanges'
        self.cursor.execute(cmd)

        changes_by_inst_uid = {}
        expected_vals_by_inst_uid = {}
        for elem_name, curr_val, expected_val, inst_uid in self.cursor.fetchall():
            if inst_uid not in changes_by_inst_uid:
                changes_by_inst_uid[inst_uid] = {}
            if inst_uid not in expected_vals_by_inst_uid:
                expected_vals_by_inst_uid[inst_uid] = {}

            changes_by_inst_uid[inst_uid][elem_name] = FormatHex(curr_val)
            expected_vals_by_inst_uid[inst_uid][elem_name] = FormatHex(expected_val)

        cmd = 'SELECT Opcode, Dasm, InstUID FROM Instructions WHERE PC = "{}"'.format(pc)
        self.cursor.execute(cmd)
        opcode, dasm, inst_uid = self.cursor.fetchone()

        cmd = 'SELECT PC, InstUID FROM Instructions'
        self.cursor.execute(cmd)
        for inst_pc, uid in self.cursor.fetchall():
            # Don't replay the simulation too far
            if int(inst_pc, 16) > int(pc, 16):
                break

            if uid in changes_by_inst_uid:
                for elem_name, curr_val in changes_by_inst_uid[uid].items():
                    reg_vals[elem_name].SetValue(curr_val)

        changes = changes_by_inst_uid.get(inst_uid, {})
        changes = {reg_name: (FormatHex(reg_vals[reg_name].prev_val), FormatHex(reg_vals[reg_name].curr_val)) for reg_name in changes}
        reg_vals = {reg_name: FormatHex(reg_val.curr_val) for reg_name, reg_val in reg_vals.items()}

        expected_vals = expected_vals_by_inst_uid.get(inst_uid, {})
        expected_vals = {reg_name: expected_vals[reg_name] for reg_name in changes}
        for reg_name in expected_vals:
            reg_val = expected_vals[reg_name]
            if reg_val is None:
                continue

            expected_vals[reg_name] = FormatHex(reg_val)

        return StateSnapshot(pc, opcode, dasm, changes, reg_vals, expected_vals)

    def GetInitialState(self):
        cmd = 'SELECT PC FROM Instructions ORDER BY Id ASC LIMIT 1'
        self.cursor.execute(cmd)
        pc = FormatHex(self.cursor.fetchone()[0])
        reg_vals = self.__GetInitRegValues()

        return StateSnapshot(pc, None, None, None, reg_vals, {})

    def __GetInitRegValues(self):
        if self.init_reg_values is None:
            cmd = 'SELECT RegName, RegValue FROM InitRegValues'
            self.cursor.execute(cmd)
            self.init_reg_values = dict(self.cursor.fetchall())

        return self.init_reg_values.copy()

class StateSnapshot:
    def __init__(self, pc, opcode, dasm, changes, reg_vals, expected_vals):
        self.pc = pc
        self.opcode = opcode
        self.dasm = dasm
        self.changes = changes
        self.reg_vals = reg_vals
        self.expected_vals = expected_vals

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

    def getExpectedValue(self, reg_name):
        return self.expected_vals.get(reg_name)
