# author Lukas Neupauer
# login xneupa01
# prgram IPP project 2021/2022 interpret.py

import sys
import argparse
import os.path
import xml.etree.ElementTree as et
import re


class Key_words:
    # instruction names with theirs arguments
    INSTRUCTONS = {
        "MOVE" : ["var", "symb"],
        "CREATEFRAME" : [],
        "PUSHFRAME" : [],
        "POPFRAME" : [],
        "DEFVAR" : ["var"],
        "CALL" : ["label"],
        "RETURN" : [],
        "PUSHS" : ["symb"],
        "POPS" : ["var"],
        "ADD" : ["var", "symb", "symb"],
        "SUB" : ["var", "symb", "symb"],
        "MUL" : ["var", "symb", "symb"],
        "IDIV" : ["var", "symb", "symb"],
        "LT" : ["var", "symb", "symb"],
        "GT" : ["var", "symb", "symb"],
        "EQ" : ["var", "symb", "symb"],
        "AND" : ["var", "symb", "symb"],
        "OR" : ["var", "symb", "symb"],
        "NOT" : ["var", "symb"],
        "INT2CHAR" : ["var", "symb"],
        "STRI2INT" : ["var", "symb", "symb"],
        "READ" : ["var", "type"],
        "WRITE" : ["symb"],
        "CONCAT" : ["var", "symb", "symb"],
        "STRLEN" : ["var", "symb"],
        "GETCHAR" : ["var", "symb", "symb"],
        "SETCHAR" : ["var", "symb", "symb"],
        "TYPE" : ["var", "symb"],
        "LABEL" : ["label"],
        "JUMP" : ["label"],
        "JUMPIFEQ" : ["label", "symb", "symb"],
        "JUMPIFNEQ" : ["label", "symb", "symb"],
        "EXIT" : ["symb"],
        "DPRINT" : ["symb"],
        "BREAK" : []
    }

class Stats:
    opcodes = []
    max_vars = 0
    instructions = 0

class Argument:
    def __init__(self, arg_type, value, name):
        self.type = arg_type
        self.value = value
        self.name = name

class Instruction:
    def __init__(self, name, number):
        self.name = name
        self.number = number
        self.args = []
    def add_argument(self, arg_type, value, name):
        self.args.append(Argument(arg_type, value, name))

class Variable:
    def __init__(self, name, var_type, value):
        self.name = name
        self.type = var_type
        self.value = value

class Frame:
    def __init__(self):
        self.variables = []
    def add_var(self, name, var_type, value):
        self.variables.append(Variable(name, var_type, value))

class Label:
    def __init__(self, name, position):
        self.name = name
        self.position = position

class Program:
    stats_flag = False
    stats = Stats()
    instructions = []
    global_frame = Frame()
    frames = [] # stack of frames for pushframe instruction
    tmp_frame = None
    labels = []
    stack = [] # stack of variables for pushs instruction
    calls = [] # stack of callings

    # return type of variable from given frame
    def sub_return_type(self, name, frame):
        j = 0
        while j < len(frame.variables):
            if frame.variables[j].name == name:
                return frame.variables[j].type
            j += 1
        sys.stderr.write("Not found variable in frame")
        exit(54)

    # return value of variable from given frame
    def sub_return_value(self, name, frame):
        j = 0
        while j < len(frame.variables):
            if frame.variables[j].name == name:
                return frame.variables[j].value
            j += 1
        sys.stderr.write("Not found variable in frame")
        exit(54)

    # finds out frame and calls sub_return_value()
    # return value of variable
    def return_value(self, name, program):
        if name[0:2] == "GF":
            return(self.sub_return_value(name[3:], program.global_frame))
        elif name[0:2] == "TF":
            if program.tmp_frame == None:
                sys.stderr.write("not define tmp frame")
                exit(55)
            return(self.sub_return_value(name[3:], program.tmp_frame))
        elif name[0:2] == "LF":
            if program.frames == []:
                sys.stderr.write("No local frame")
                exit(55)
            return(self.sub_return_value(name[3:], program.frames[-1]))
        return None

    # finds out frame and calls sub_return_type()
    # return type of variable
    def return_type(self, name, program):
        if name[0:2] == "GF":
            return(self.sub_return_type(name[3:], program.global_frame))
        elif name[0:2] == "TF":
            if program.tmp_frame == None:
                sys.stderr.write("not define tmp frame")
                exit(55)
            return(self.sub_return_type(name[3:], program.tmp_frame))
        elif name[0:2] == "LF":
            if program.frames == []:
                sys.stderr.write("No local frame")
                exit(55)
            return(self.sub_return_type(name[3:], program.frames[-1]))
        return None

    # updates value and type of variable
    def update_value(self, name, value, type, program):
        dump = self.return_value(name, program)
        frame = Frame()
        if name[0:2] == "GF":
            frame = program.global_frame
        elif name[0:2] == "TF":
            frame = program.tmp_frame
        elif name[0:2] == "LF":
            frame = program.frames[-1]
        j = 0
        while j < len(frame.variables):
            if frame.variables[j].name == name[3:]:
                frame.variables[j].value = value
                frame.variables[j].type = type
                break
            j += 1

    # replace escape sequences with letters
    def replace(self, string):
        replace = re.findall("\\\\\d\d\d", string)
        for x in replace:
            string = string.replace(x, chr(int(x.strip("\\"))))
        return string

    # check if symbol is variable or constant and if it's constant check syntax
    # return it's value
    def symb_check(self, arg, type, program):
        if arg.type == "var":
            if program.return_type(arg.value, program) == type:
                return(program.return_value(arg.value, program))
            elif program.return_type(arg.value, program) == None:
                sys.stderr.write("Undefined value")
                exit(56)
        elif arg.type == type:
            ret_value = arg.value
            if type == "bool":
                if arg.value == "true":
                    ret_value = True
                elif arg.value == "false":
                    ret_value = False
                else:
                    exit(57)
            elif type == "int":
                if arg.value.lstrip("-").isdecimal():
                    ret_value = int(arg.value)
                else:
                    sys.stderr.write("Nondigit character in integer")
                    exit(53)
            elif type == "string" and ret_value == None:
                ret_value = ""
            return(ret_value)
        sys.stderr.write("Incorect type in operation")
        exit(53)



# parsing arguments
ap = argparse.ArgumentParser(description="Interpret of already parsed IPPcode22 program. At least of of arguments either --source or --input must be declared.")
ap.add_argument("--source", nargs=1, help="path to xml file with IPPcode22 program, usage: --source=file.xml, defult stdin ")
ap.add_argument("--input", nargs=1, help="path to file with input program data, usage: --input=file.in, default stdin")
ap.add_argument("--stats", nargs=1, help="path to file to store tatistics, usage: --stats=file optional arguments --insts --hot --vars")
ap.add_argument("--insts", action="store_true", help="only with combination with --stats, print number of instructions to the file")
ap.add_argument("--hot", action="store_true", help="only with combination with --stats, print most common instruction")
ap.add_argument("--vars", action="store_true", help="only with combination with --stats, print highest amount of variables at one time")

args = vars(ap.parse_args())
try:
    source = ''.join(map(str, args['source']))
except:
    source = sys.stdin
try:
    input = ''.join(map(str, args['input']))
except:
    input = sys.stdin
if source == sys.stdin and input == sys.stdin:
    sys.stderr.write("Missing argument source or input. Use --help for more info.\n")
    exit(10)
try:
    stats_file = ''.join(map(str, args['stats']))
except:
    stats_file = None
if stats_file == None:
    if "--insts" in sys.argv or "--hot" in sys.argv or "--vars" in sys.argv:
        sys.stderr.write("Incorect combination of Arguments\n")
        exit(10)

# loading xml
if source != sys.stdin:
    if not os.path.exists(source):
        sys.stderr.write("Source file do not exists.\n")
        exit(11) # source file does not exist
    source = open(source, "r")
if input != sys.stdin:
    if not os.path.exists(input):
        sys.stderr.write("Input file do not exists.\n")
        exit(11) # input file does not exist
    input = open(input, "r")
try:
    tree = et.parse(source)
except:
    sys.stderr.write("Wrong format of xml file.\n")
    exit(31) # wrong format of xml file


# xml check
root = tree.getroot()
if root.tag != 'program':
    sys.stderr.write("Wrong program tag.\n")
    exit(32) # wrong program tag
for child in root:
    if child.tag != 'instruction':
        sys.stderr.write("Wrong program tag.\n")
        exit(32) # wrong program tag
    ca = list(child.attrib.keys())
    if not('order' in ca) or not('opcode' in ca):
        sys.stderr.write("Wrong program tag.\n")
        exit(32) # wrong program tag
    for subelem in child:
        if not(re.match(r'arg[123]', subelem.tag)):
            sys.stderr.write("Wrong program tag.\n")
            exit(32) # wrong program tag

# procesing instructions and checking their arguments
program = Program()
for e in root:
    opcode = e.get('opcode').upper()
    order = e.get('order')
    if not order.isnumeric():
        sys.stderr.write("Opcode in not number")
        exit(32)
    instr = Instruction(opcode, int(order))
    for sub in e:
        instr.add_argument(sub.get('type'), sub.text, sub.tag)
    # sorting arguments of instruction
    instr.args = sorted(instr.args, key = lambda arg: arg.name)
    if not instr.name in Key_words.INSTRUCTONS:
        sys.stderr.write("Unknown instruction " + instr.name + " order " + str(instr.number))
        exit(32)
    i = 1
    for arg in instr.args:
        if not arg.name == ("arg" + str(i)):
            sys.stderr.write("Incorrect argument tag " + arg.name + " order " + str(instr.number))
            exit(32)
        if i > len(Key_words.INSTRUCTONS[instr.name]):
            sys.stderr.write("Too much arguments in function " + instr.name + " with order " + str(instr.number))
            exit(32)
        if Key_words.INSTRUCTONS[instr.name][i-1] == "symb":
            if not arg.type in ["int", "string", "nil", "bool", "var"]:
                sys.stderr.write(Key_words.INSTRUCTONS[instr.name][i-1] + "    " + arg.type)
                exit(32)
        elif not Key_words.INSTRUCTONS[instr.name][i-1] == arg.type:
            sys.stderr.write("heloooooo")
            exit(32)
        i += 1
    if not i - 1 == len(Key_words.INSTRUCTONS[instr.name]):
        sys.stderr.write("Not enough arguments in function " + instr.name + " with opcode " + str(instr.number))
        exit(32)
    program.instructions.append(instr)

if program.instructions == []:
    exit(0)

# sorting instruction in correct order
program.instructions = sorted(program.instructions, key = lambda Instruction: Instruction.number)

if program.instructions[0].number < 1:
    sys.stderr.write("Opcode must be more then zero")
    exit(32)

i = 1
while i < len(program.instructions):
    if program.instructions[i-1].number >= program.instructions[i].number:
        sys.stderr.write("Dubbled Opcode " + str(program.instructions[i].number))
        exit(32)
    i += 1
i = 0

# looking for labels in instructions for possible jumps
while i < len(program.instructions):
    instruction = program.instructions[i].name
    if instruction == "LABEL":
        if any(x.name == program.instructions[i].args[0].value for x in program.labels):
            sys.stderr.write("Doubled label")
            exit(52)
        label = Label(program.instructions[i].args[0].value, i)
        program.labels.append(label)
    i += 1

if not stats_file == None:
    program.stats_flag = True

#interpreting code
i = 0
while i < len(program.instructions):
    instruction = program.instructions[i].name
    if instruction == "DEFVAR":
        frame = Frame()
        if program.instructions[i].args[0].value[0:2] == "GF":
            frame = program.global_frame
        elif program.instructions[i].args[0].value[0:2] == "TF":
            if program.tmp_frame == None:
                sys.stderr.write("not define tmp frame")
                exit(55)
            frame = program.tmp_frame
        elif program.instructions[i].args[0].value[0:2] == "LF":
            if program.frames == []:
                sys.stderr.write("not define local frame")
                exit(55)
            frame = program.frames[-1]
        else:
            sys.stderr.write("Unknow frame")
            exit(32)
        if any(x.name == program.instructions[i].args[0].value[3:] for x in frame.variables):
            sys.stderr.write("Doubled var name in frame")
            exit(52)
        frame.add_var(program.instructions[i].args[0].value[3:], None, None)
    elif instruction == "MOVE":
        type = ""
        value = ""
        if program.instructions[i].args[1].type in ["int", "string", "nil", "bool"]:
            if program.instructions[i].args[1].type == "int" and not program.instructions[i].args[1].value.lstrip("-").isdigit():
                sys.stderr.write("integer with nondigit characters")
                exit(32)
            type = program.instructions[i].args[1].type
            if program.instructions[i].args[1].type == "int":
                value = int(program.instructions[i].args[1].value)
            elif program.instructions[i].args[1].type == "bool":
                if program.instructions[i].args[1].value == "true":
                    value = True
                elif program.instructions[i].args[1].value == "false":
                    value = False
                else:
                    sys.stderr.write("Wrond declaration of bool")
                    exit(69)
            else:
                value = program.instructions[i].args[1].value
        else:
            value = program.return_value(program.instructions[i].args[1].value, program)
            type = program.return_type(program.instructions[i].args[1].value, program)
        if type == None:
            sys.stderr.write("Unset value")
            exit(56)
        program.update_value(program.instructions[i].args[0].value, value, type, program)
    elif instruction in ["ADD", "MUL", "SUB", "IDIV"]:
        value1 = program.symb_check(program.instructions[i].args[1], "int", program)
        value2 = program.symb_check(program.instructions[i].args[2], "int", program)
        if instruction == "ADD":
            program.update_value(program.instructions[i].args[0].value, int(value1 + value2), "int", program)
        elif instruction == "SUB":
            program.update_value(program.instructions[i].args[0].value, int(value1 - value2), "int", program)
        elif instruction == "MUL":
            program.update_value(program.instructions[i].args[0].value, int(value1 * value2), "int", program)
        else:
            if int(value2) == 0:
                sys.stderr.write("Divide by zero.\n")
                exit(57)
            program.update_value(program.instructions[i].args[0].value, int(value1 / value2), "int", program)
    elif instruction == "WRITE":
        string = ""
        if program.instructions[i].args[0].type in ["string", "int", "bool"]:
            string = program.instructions[i].args[0].value
        if program.instructions[i].args[0].type == "var":
            if program.return_type(program.instructions[i].args[0].value, program) == None:
                sys.stderr.write("Writing unset variable")
                exit(56)
            if not program.return_type(program.instructions[i].args[0].value, program) == "nil":
                string = str(program.return_value(program.instructions[i].args[0].value, program))
            if program.return_type(program.instructions[i].args[0].value, program) == "bool":
                string = string.lower()
        print(program.replace(string), end='')
    elif instruction == "POPFRAME":
        if program.frames == []:
            sys.stderr.write("popframe empty")
            exit(55)
        program.tmp_frame = program.frames.pop()
    elif instruction == "PUSHFRAME":
        if program.tmp_frame == None:
            sys.stderr.write("pushframe not defined")
            exit(55)
        program.frames.append(program.tmp_frame)
        program.tmp_frame = None
    elif instruction == "CREATEFRAME":
        program.tmp_frame = Frame()
    elif instruction in ["JUMP", "JUMPIFEQ", "JUMPIFNEQ", "CALL"]:
        found = False
        position = 0
        for x in program.labels:
            if x.name == program.instructions[i].args[0].value:
                if instruction == "JUMP":
                    i = x.position
                    position = x.position
                elif instruction == "CALL":
                    program.calls.append(i)
                    i = x.position
                found = True
                break
        if not found:
            sys.stderr.write("nonexisting label")
            exit(52)
        if instruction == "JUMPIFEQ" or instruction == "JUMPIFNEQ":
            value1 = 0
            value1_type = ""
            value2 = 0
            value2_type = ""
            if program.instructions[i].args[1].type == "int":
                value1 = int(program.instructions[i].args[1].value)
                value1_type = "int"
            elif program.instructions[i].args[1].type == "var":
                value1 = program.return_value(program.instructions[i].args[1].value, program)
                value1_type = program.return_type(program.instructions[i].args[1].value, program)
            else:
                value1 = program.instructions[i].args[1].value
                value1_type = program.instructions[i].args[1].type
            if program.instructions[i].args[2].type == "int":
                value2 = int(program.instructions[i].args[2].value)
                value2_type = "int"
            elif program.instructions[i].args[2].type == "var":
                value2 = program.return_value(program.instructions[i].args[2].value, program)
                value2_type = program.return_type(program.instructions[i].args[2].value, program)
            else:
                value2 = program.instructions[i].args[2].value
                value2_type = program.instructions[i].args[2].type
            if value1_type == None or value2_type == None:
                sys.stderr.write("Invalid comparasion")
                exit(56)
            if not value1_type == value2_type:
                if not value1_type == "nil" and not value2_type == "nil":
                    sys.stderr.write("Invalid comparasion")
                    exit(53)
            if value1_type == "string":
                value1 = program.replace(str(value1))
                value2 = program.replace(str(value2))
            if instruction == "JUMPIFEQ":
                if value1 == value2:
                    i = x.position
            else:
                if not value1 == value2:
                    i = x.position
    elif instruction == "EXIT":
        exit_val = program.symb_check(program.instructions[i].args[0], "int", program)
        if exit_val < 0 or exit_val > 49 :
            exit_val = 57
        exit(exit_val)
    elif instruction == "TYPE":
        type = ""
        if program.instructions[i].args[1].type == "var":
            type = program.return_type(program.instructions[i].args[1].value, program)
        else:
            type = program.instructions[i].args[1].type
        if type == None:
            type = ""
        program.update_value(program.instructions[i].args[0].value, type, "string", program)
    elif instruction == "PUSHS":
        if program.instructions[i].args[0].type == "var":
            var = Variable(program.instructions[i].args[0].value, program.return_type(program.instructions[i].args[0].value, program), program.return_value(program.instructions[i].args[0].value, program))
            if var.type == None:
                sys.stderr.write("Unset variable")
                exit(56)
            program.stack.append(var)
        else:
            value = program.instructions[i].args[0].value
            if program.instructions[i].args[0].type == "int":
                if not value.isdecimal():
                    sys.stderr.write("Int not decimal")
                    exit(58)
                value = int(value)
            elif program.instructions[i].args[0].type == "bool":
                if value == "true":
                    value = True
                elif arg.value == "false":
                    value = False
                else:
                    exit(57)
            var = Variable(None, program.instructions[i].args[0].type, value)
            program.stack.append(var)
    elif instruction == "POPS":
        if program.stack == []:
            sys.stderr.write("Pop empty stack")
            exit(56)
        var = program.stack.pop()
        program.update_value(program.instructions[i].args[0].value, var.value, var.type, program)
    elif instruction == "INT2CHAR":
        char = program.symb_check(program.instructions[i].args[1], "int", program)
        if char > 1114111 or char < 0:
            sys.stderr.write("int out of range")
            exit(58)
        program.update_value(program.instructions[i].args[0].value, chr(char), "string", program)
    elif instruction == "RETURN":
        if program.calls == []:
            sys.stderr.write("No return index")
            exit(56)
        i = program.calls.pop()
    elif instruction in ["AND", "OR", "NOT"]:
        symb1 = program.symb_check(program.instructions[i].args[1], "bool", program)
        if instruction == "NOT":
            program.update_value(program.instructions[i].args[0].value, not symb1 ,"bool", program)
            i += 1
            continue
        symb2 = program.symb_check(program.instructions[i].args[2], "bool", program)
        if instruction == "AND":
            program.update_value(program.instructions[i].args[0].value,  symb1 and symb2 ,"bool", program)
        else:
            program.update_value(program.instructions[i].args[0].value,  symb1 or symb2 ,"bool", program)
    elif instruction == "CONCAT":
        str1 = program.symb_check(program.instructions[i].args[1], "string", program)
        str2 = program.symb_check(program.instructions[i].args[2], "string", program)
        program.update_value(program.instructions[i].args[0].value, str1 + str2, "string", program)
    elif instruction == "STRLEN":
        string = program.symb_check(program.instructions[i].args[1], "string", program)
        program.update_value(program.instructions[i].args[0].value, len(string), "int", program)
    elif instruction == "GETCHAR" or instruction == "STRI2INT":
        string = program.symb_check(program.instructions[i].args[1], "string", program)
        char_pos = program.symb_check(program.instructions[i].args[2], "int", program)
        if len(string) <= char_pos or char_pos < 0:
            sys.stderr.write("Index out of range")
            exit(58)
        if instruction == "GETCHAR":
            program.update_value(program.instructions[i].args[0].value, string[char_pos], "string", program)
        else:
            program.update_value(program.instructions[i].args[0].value, ord(string[char_pos]), "int", program)
    elif instruction == "SETCHAR":
        string = program.symb_check(program.instructions[i].args[0], "string", program)
        char_pos = program.symb_check(program.instructions[i].args[1], "int", program)
        char = program.symb_check(program.instructions[i].args[2], "string", program)
        if len(string) <= char_pos or char_pos < 0 or len(char) == 0:
            sys.stderr.write("Index out of range")
            exit(58)
        char = program.replace(char)
        string = list(string)
        string[char_pos] = char[0]
        string = "".join(string)
        program.update_value(program.instructions[i].args[0].value, string, "string", program)
    elif instruction in ["GT", "LT", "EQ"]:
        type1 = ""
        value1 = ""
        if program.instructions[i].args[1].type == "var":
            type1 = program.return_type(program.instructions[i].args[1].value, program)
            value1 = program.return_value(program.instructions[i].args[1].value, program)
        else:
            type1 = program.instructions[i].args[1].type
            value1 = program.symb_check(program.instructions[i].args[1], type1, program)
        type2 = ""
        value2 = ""
        if program.instructions[i].args[2].type == "var":
            type2 = program.return_type(program.instructions[i].args[2].value, program)
            value2 = program.return_value(program.instructions[i].args[2].value, program)
        else:
            type2 = program.instructions[i].args[2].type
            value2 = program.symb_check(program.instructions[i].args[2], type2, program)
        if type1 == None or type2 == None:
            sys.stderr.write("Undefined comparation")
            exit(56)
        if (type1 == "nil" or type2 == "nil") and not instruction == "EQ":
            sys.stderr.write("Comparation of nil type")
            exit(53)
        if not type1 == type2:
            if not ((type1 == "nil" or type2 == "nil") and instruction == "EQ"):
                sys.stderr.write("Comparation of different types")
                exit(53)
        if type1 == "string":
            if value1 == None:
                value1 = ""
            else:
                value1 = program.replace(value1)
        if type2 == "string":
            if value2 == None:
                vaue2 = ""
            else:
                value2 = program.replace(value2)
        if instruction == "GT":
            program.update_value(program.instructions[i].args[0].value, value1 > value2, "bool", program)
        elif instruction == "LT":
            program.update_value(program.instructions[i].args[0].value, value1 < value2, "bool", program)
        else:
            program.update_value(program.instructions[i].args[0].value, value1 == value2, "bool", program)
    elif instruction == "READ":
        if not program.instructions[i].args[1].value in ["int", "string", "bool"]:
            sys.stderr.write("Unknown type")
            exit(53)
        line = input.readline()
        value = ""
        type = ""
        if not line:
            type = "nil"
            value = "nil"
        else:
            line = line.rstrip("\n")
            if program.instructions[i].args[1].value == "bool":
                if line.lower() == "true":
                    value = True
                else:
                    value = False
                type = "bool"
            elif program.instructions[i].args[1].value == "int":
                if line.lstrip("-").isdecimal():
                    value = int(line)
                    type = "int"
                else:
                    type = "nil"
                    value = "nil"
            else:
                type = "string"
                value = line
        program.update_value(program.instructions[i].args[0].value, value, type, program)
    elif instruction == "DPRINT":
        sys.stderr.write(program.symb_check(program.instructions[i].args[0], "string", program))
    elif instruction == "BREAK":
        sys.stderr.write("Position: " + str(i))
    else:
        program.stats.instructions -= 1
    if program.stats_flag == True:
        program.stats.opcodes.append(program.instructions[i].number)
        vars = len(program.global_frame.variables)
        if not program.tmp_frame == None:
            vars += len(program.tmp_frame.variables)
        for frame in program.frames:
            vars += len(frame.variables)
        if vars > program.stats.max_vars:
            program.stats.max_vars = vars
    i += 1
    program.stats.instructions += 1

# printing statistics
if not stats_file == None:
    f = open(stats_file, "w")
    for arg in sys.argv:
        if arg == "--insts":
            f.write(str(program.stats.instructions) + "\n")
        if arg == "--hot":
            f.write(str(max(set(program.stats.opcodes), key = program.stats.opcodes.count)) + "\n")
        if arg == "--vars":
            f.write(str(program.stats.max_vars) + "\n")
exit(0)
