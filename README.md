Computer Architecture Mini Assembler
====================================

Design
------
A mini assembler/disassembler for NTHU course Computer Architecture(CS410001)

Language used: MIPS(simplified)

Build
-----
`make` or `cmake <path-to-source>`<br/>
Output filename would be `assembler`.

Run
---
`./assembler -h` or `assembler.exe -h` for more information or see below.<br/>

Assembler
---------
`-a` for assembler<br/>

`./assembler -a [options] InputFilename -o OutputFilename`

Options:<br/>
`-pc value` to specify initial pc value using decimal(should be divisible by 4).<br/>
For example, if using `./assembler -a test.s -o iimage.bin -pc 512`,<br/>
```
addi $0, $0, 0    # pc for the first instruction in assembly code will be 512
addi $1, $1, 1
addi $2, $2, 2
```

Branch label supported.<br/>

An example MIPS code:<br/>
```
# use # for comment
# use label on branch instruction
    beq $0, $0, L0
# or use constant directly
    beq $0, $0, -10
# remember to define label using <label name>:
L0: add $1, $2, $3
# number start with 0x is hexdecimal
    addi $1, $1, 0x1F
# use register name t1, t2, sp, etc, directly
    addi $t1, $sp, 62
# for nop, can use "nop" instead of "sll $0, $0, 0"
    nop
# more examples
    lw $1, 2($0)
    sw $1, 4096($0)
    sll $0, $1, 2
    addi $2, $3, -5
    bgtz $5, 10
    j L0
    jal 5
    halt
```

Disassembler
------------
`-d` for disassembler<br/>

`./assembler -d [options] InputFilename -o OutputFilename`

Output assembly code will start from pc 0.<br/>
For example, if an image file has `initial pc = 4` and has only one instruction `halt`,<br/>
output will be:
```
nop
halt
```
Note that `pc of nop = 0`, `pc of halt = 4`.

Options:<br/>
`-dec` show all constants in decimal (default)<br/>
`-hex` show all constants in hexdecimal<br/>
`-label` show branch target with label (default)<br/>
`-nolabel` show branch without label<br/>
An example for `-dec`, `-hex`, `-label` and `-nolabel`:<br/>
```
    addi $1, $1, 31    # with -dec (default)
    addi $1, $1, 0x1f  # with -hex
L0: beq $0, $0, L0     # with -label (default)
    beq $0, $0, 2      # with -nolabel
```
