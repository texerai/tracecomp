from parser import parse_log
# put your file path here
file = '/Users/sayat/Documents/GitHub/tracecomp/riscv/test/tests/bin/riscv-tests/rv64ui-p-add'
arr = parse_log(file)
print(arr[-1])