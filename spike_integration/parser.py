import pexpect

log_file = open("spike_output.txt", "wb")
pk = '/opt/homebrew/riscv64-unknown-elf/bin/pk'
file = '/Users/sayat/Documents/GitHub/tracecomp/riscv/test/tests/bin/riscv-tests/rv64ui-p-add'
cmd = "spike -d --log-commits " + " 2> trace.log" + " " + file
child = pexpect.spawn("/bin/sh", ["-c", cmd])

child.logfile_read = log_file

child.expect(pexpect.EOF, timeout=None)

log_file.close()
log_contents = ""
# Read the log file and print its contents
with open("trace.log", "r") as f:
    log_contents = f.read()
content = []
pass_next = 0
not_pass = 0
for line in log_contents.splitlines():
    if "xrv64i2p1_m2p0_a2p1_f2p2_d2p2_zicsr2p0_zifencei2p0_zmmul1p0" in line:
        not_pass = 1
    if "ecall" in line:
        not_pass = 0
    if not_pass:
        if "(spike)" in line or ">>>>" in line or "exception" in line or pass_next: # ignore the interactive traces
            if pass_next:
                pass_next = 0
            if ">>>>" in line:
                pass_next = 1
            continue
        else:
            content.append(line)
    else:
        continue
print(len(content))
log_data = []
for line in content:
    line_split = line.split()
    log = {}
    if "tval" in line:
        log = {
            "pc": line_split[3],
            "instruction": "0x00000013",
            "register": None,
            "value": None,
            "mem": None,
            "mem_value": None
        }
        log_data.append(log)
        continue
    log = {
        "pc": line_split[3],
        "instruction": line_split[4][1:-1]
    }
    if len(line_split) > 5:
        log["register"] = line_split[5]
        log["value"] = line_split[6]
    else:
        log["register"] = None
        log["value"] = None
    if "mem" in line:
        if len(line_split) > 8:
            log["mem"] = line_split[8] #load
        else:
            log["mem"] = line_split[6]
            log["mem_value"] = line_split[7]  # store
            log["register"] = None
            log["value"] = None
    else:
        log["mem"] = None
        log["mem_value"] = None
    log_data.append(log)
start = 0
count = 0
for log in log_data:
    if log["instruction"] == "0xf1402573":
        start = 1
    if log["instruction"] == "0x00200193":
        start = 0
        break
    if start:
        log = {
            "pc": log["pc"],
            "instruction": "0x00000013",
            "register": None,
            "value": None,
            "mem": None,
            "mem_value": None
        }
        count += 1
    
print(len(log_data))
print(log_data[-1])
print(count)