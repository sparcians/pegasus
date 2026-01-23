import sys
import json

def main():

    mavis_json = sys.argv[1]
    tag = sys.argv[2]

    with open(mavis_json, 'r') as fh:
        data = json.load(fh)
        for inst in data:
            entry = {}
            entry['mnemonic'] = inst['mnemonic']
            entry['handler'] = 'nop'
            entry['cost'] = 1
            entry['tags'] = tag
            memory = False
            if 'type' in inst.keys():
                memory = "load" in inst['type'] or "store" in inst['type']
            entry['memory'] = memory
            entry['cof'] = False
            print(entry, end="")
            print(",")
            #print(json.dumps(entry, indent=4))


if __name__ == "__main__":
    main()
