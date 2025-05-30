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
for line in log_contents.splitlines():
    if "(spike)" in line or ">>>>" in line or "exception" in line or pass_next: # ignore the interactive traces
        if pass_next:
            pass_next = 0
        if ">>>>" in line:
            pass_next = 1
        continue
    else:
        content.append(line)
print(len(content))
log_data = []
for line in content:
    line_split = line.split()
    if "tval" in line:
        continue  # skip lines with tval
    log = {
        "core": line_split[1][0],
        "something": line_split[2],
        "pc": line_split[3],
        "instruction": line_split[4][1:-1]
    }
    if len(line_split) > 5:
        log["register"] = line_split[5]
        log["value"] = line_split[6]
    else:
        log["register"] = None
        log["value"] = None
    if len(line_split) > 8:
        log["mem"] = line_split[8]
    else:
        log["mem"] = None
    log_data.append(log)

print(len(log_data))
print(log_data[-10])