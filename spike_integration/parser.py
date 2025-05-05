import pexpect

log_file = open("spike_output.txt", "wb")

cmd = "spike --log-commits -d /Users/sayat/Documents/GitHub/tracecomp/riscv/test/tests/bin/riscv-tests/rv64ui-p-and 2> trace.log"
child = pexpect.spawn("/bin/sh", ["-c", cmd])

child.logfile_read = log_file

child.expect(pexpect.EOF, timeout=None)

log_file.close()