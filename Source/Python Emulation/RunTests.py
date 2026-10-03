#!/usr/bin/env python
#
#   Unit tests for the Manchester Baby (SSEM) emulation.  Run with:
#
#       python3 -m unittest -v
#
import os
import tempfile
import unittest

from Register import Register
from Instructions import Instructions
from StoreLines import StoreLines, MAX_STORE_SIZE
from CPU import CPU
from ManchesterBaby import ManchesterBaby


class RegisterTests(unittest.TestCase):
    def test_default_and_set(self):
        reg = Register()
        self.assertEqual(reg.Value, 0)
        reg.Value = 200
        self.assertEqual(reg.Value, 200)

    def test_values_are_truncated_to_32_bits(self):
        reg = Register()
        reg.Value = -1
        self.assertEqual(reg.Value, 0xffffffff)
        reg.Value = 0x100000000
        self.assertEqual(reg.Value, 0)

    def test_representations(self):
        reg = Register(0x1f1f)
        self.assertEqual(reg.Hex(), '0x00001f1f')
        self.assertEqual(reg.Binary(), '11111000111110000000000000000000')
        self.assertEqual(reg.ReverseBits(), 0xf8f80000)

    def test_equality(self):
        self.assertEqual(Register(5), Register(5))
        self.assertNotEqual(Register(5), Register(6))
        self.assertNotEqual(Register(5), 5)


class InstructionsTests(unittest.TestCase):
    def setUp(self):
        self.instructions = Instructions()

    def test_decode(self):
        self.assertEqual(self.instructions.Opcode(0x0000000f), Instructions.OPCODE_JMP)
        self.assertEqual(self.instructions.LineNumber(0x0000000f), 0xf)

    def test_mnemonics(self):
        self.assertEqual(self.instructions.Mnemonic(0), 'JMP')
        self.assertEqual(self.instructions.Mnemonic(1), 'JRP')
        self.assertEqual(self.instructions.Mnemonic(7), 'STOP')

    def test_invalid_opcodes(self):
        for opcode in (-1, 8, 12, 14):
            with self.assertRaises(ValueError):
                self.instructions.Mnemonic(opcode)

    def test_disassemble(self):
        self.assertEqual(self.instructions.Disassemble(0b0100000000001010), 'LDN 10')
        self.assertEqual(self.instructions.Disassemble(0x200b), 'JRP 11')

    def test_lookup(self):
        self.assertEqual(self.instructions.Lookup('JRP')[0]['opcode'], Instructions.OPCODE_JRP)
        self.assertEqual(self.instructions.Lookup('JPR')[0]['opcode'], Instructions.OPCODE_JRP)
        self.assertEqual(self.instructions.Lookup('XYZ'), [])


class StoreLinesTests(unittest.TestCase):
    def test_sizes(self):
        self.assertEqual(StoreLines().Length, 32)
        self.assertEqual(StoreLines(100).Length, 100)
        StoreLines(MAX_STORE_SIZE)
        for size in (0, -1, MAX_STORE_SIZE + 1):
            with self.assertRaises(ValueError):
                StoreLines(size)

    def test_set_and_get(self):
        sl = StoreLines(100)
        self.assertEqual(sl.GetLine(0).Value, 0)
        sl.SetLine(0, Register(1))
        self.assertEqual(sl.GetLine(0).Value, 1)

    def test_set_stores_a_copy(self):
        sl = StoreLines()
        reg = Register(1)
        sl.SetLine(0, reg)
        reg.Value = 2
        self.assertEqual(sl.GetLine(0).Value, 1)

    def test_set_requires_register(self):
        with self.assertRaises(TypeError):
            StoreLines().SetLine(0, 1)

    def test_out_of_range(self):
        sl = StoreLines(100)
        for line in (-1, 500):
            with self.assertRaises(IndexError):
                sl.SetLine(line, Register(0))
            with self.assertRaises(IndexError):
                sl.GetLine(line)

    def test_clear(self):
        sl = StoreLines(100)
        sl.SetLine(3, Register(7))
        sl.Clear()
        for line in range(sl.Length):
            self.assertEqual(sl.GetLine(line).Value, 0)


class CPUTests(unittest.TestCase):
    def test_program(self):
        sl = StoreLines()
        cpu = CPU(sl)
        sl.SetLine(1, Register(0b0100000000001010))     # LDN 10
        sl.SetLine(2, Register(0b1000000000001011))     # SUB 11
        sl.SetLine(3, Register(0b0110000000001100))     # STO 12
        sl.SetLine(4, Register(0b1100000000000000))     # CMP (SKN)
        sl.SetLine(5, Register(0b0000000000001100))     # JMP 12 (line 12 holds 1, so execution continues at line 2)
        sl.SetLine(6, Register(0b0010000000001011))     # JRP 11 (add 9 to CI, currently 6)
        sl.SetLine(16, Register(0b1110000000000000))    # STOP
        sl.SetLine(10, Register(0xfffffff6))            # -10 (negated by LDN)
        sl.SetLine(11, Register(9))
        sl.SetLine(12, Register(1))
        cpu.Reset()

        cpu.SingleStep()                                # LDN 10
        self.assertEqual(cpu.CI.Value, 1)
        self.assertEqual(cpu.Accumulator.Value, 10)
        cpu.SingleStep()                                # SUB 11
        self.assertEqual(cpu.CI.Value, 2)
        self.assertEqual(cpu.Accumulator.Value, 1)
        cpu.SingleStep()                                # STO 12
        self.assertEqual(cpu.Accumulator.Value, 1)
        self.assertEqual(sl.GetLine(12).Value, 1)
        self.assertTrue(cpu.UpdateDisplayTube)
        cpu.SingleStep()                                # CMP
        self.assertEqual(cpu.CI.Value, 4)
        for _ in range(4):                              # JMP 12, SUB 11, STO 12, CMP
            cpu.SingleStep()
        cpu.SingleStep()                                # JRP 11
        self.assertEqual(cpu.CI.Value, 15)
        cpu.SingleStep()                                # STOP
        self.assertTrue(cpu.Stopped)
        with self.assertRaises(RuntimeError):
            cpu.SingleStep()

    def test_no_store_lines(self):
        with self.assertRaises(RuntimeError):
            CPU().SingleStep()

    def test_reset_clears_registers(self):
        cpu = CPU(StoreLines())
        cpu.PI = Register(5)
        cpu.Reset()
        self.assertEqual(cpu.PI.Value, 0)


class ManchesterBabyTests(unittest.TestCase):
    def assemble(self, baby, text):
        with tempfile.NamedTemporaryFile('w', suffix='.ssem', delete=False) as f:
            f.write(text)
        self.addCleanup(os.remove, f.name)
        baby.Assembler(f.name)

    def test_requires_assembled_program(self):
        baby = ManchesterBaby()
        with self.assertRaises(RuntimeError):
            baby.RunProgram()
        with self.assertRaises(RuntimeError):
            baby.Print()

    def test_unknown_instruction_reports_line(self):
        baby = ManchesterBaby()
        with self.assertRaisesRegex(ValueError, 'line 4'):
            self.assemble(baby, '\n-- comment\n0: NUM 1\n1: FOO 2\n')

    def test_blank_lines_and_comments_are_skipped(self):
        baby = ManchesterBaby()
        self.assemble(baby, '\n-- comment\n0: NUM 5\n\n1: STOP\n')
        self.assertEqual(baby.RunProgram(), 1)

    def test_instruction_limit(self):
        baby = ManchesterBaby()
        self.assemble(baby, '0: JMP 0\n')
        with self.assertRaises(RuntimeError):
            baby.RunProgram(maxInstructions = 50)

    def test_callbacks_are_per_call(self):
        baby = ManchesterBaby()
        self.assemble(baby, '0: NUM 0\n1: STO 5\n2: STOP\n')
        calls = []
        baby.RunProgram(updateDisplayTube = calls.append)
        self.assertEqual(len(calls), 2)      # Initial display plus the STO.
        baby.RunProgram()
        self.assertEqual(len(calls), 2)

    def test_errors_are_reported_to_the_ui(self):
        class UI:
            def __init__(self):
                self.errors = []
            def DisplayError(self, message):
                self.errors.append(message)
        ui = UI()
        baby = ManchesterBaby(ui)
        self.assemble(baby, '0: JMP 0\n')
        with self.assertRaises(RuntimeError):
            baby.RunProgram(maxInstructions = 5)
        self.assertEqual(len(ui.errors), 1)

    def test_assembles_and_runs_sample(self):
        path = os.path.join(os.path.dirname(os.path.abspath(__file__)), 'Sources', 'hfr989.ssem')
        baby = ManchesterBaby()
        baby.Assembler(path)
        self.assertGreater(baby.RunProgram(), 0)


if __name__ == '__main__':
    unittest.main()
