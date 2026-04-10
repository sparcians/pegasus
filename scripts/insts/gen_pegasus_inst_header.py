import sys
import json

def print_entry(mavis_json, tag):
    with open(mavis_json, 'r') as fh:
        data = json.load(fh)
        for inst in data:
            entry = {}
            entry['mnemonic'] = inst['mnemonic']
            entry['handler'] = 'unsupported'
            entry['cost'] = 1
            entry['tags'] = tag
            memory = False
            if 'type' in inst.keys():
                memory = "load" in inst['type'] or "store" in inst['type']
            entry['memory'] = memory
            entry['cof'] = False
            print("   ", entry, end=",")
            print()


def main():
    mavis_json_rv64 = sys.argv[1]
    mavis_json_rv32 = ""
    if len(sys.argv) == 3:
        mavis_json_rv32 = sys.argv[2]

    ext = mavis_json_rv64.split('/')[-1]
    ext = ext.removeprefix("isa_rv64")
    ext = ext.removesuffix(".json")

    print("RV"+ext.upper()+"_MAVIS_EXTS = [\""+ext+"\"]")
    print()

    ext = ext.upper()
    if (mavis_json_rv32):
        print("RV32"+ext+"_INST = [")

        tag = ext+"_EXT_32"
        print_entry(mavis_json_rv32, tag)

        print("]")
        print()

    if (mavis_json_rv64):
        print("RV64"+ext+"_INST = [")

        tag = ext+"_EXT_64"
        print_entry(mavis_json_rv64, tag)

    print("]")


if __name__ == "__main__":
    main()
