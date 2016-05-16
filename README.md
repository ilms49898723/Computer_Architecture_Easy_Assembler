# Computer_Architecture_Mini_Assembler

Design
------

A mini assembler/disassembler for NTHU course Computer Architecture

Language used: MIPS(simplified)

Build
-----
`make` or `cmake <path to source>`<br/>
Output filename would be `assembler`

Run
---
`./assembler -h` or `assembler.exe -h` for more information or see below.<br/>

Assembler
---------

`-a` for assembler<br/>

`./assembler -a [options] InputFilename -o OutputFilename`

Options:<br/>
`-pc value` to specify initial pc value, for example, `-pc 512`

Branch label supported.<br/>
An example MIPS code:<br/>
```
# use # for comment(like // in c language)
# use label on branch instruction
    beq $0, $0, L0
# or use constant directly
    beq $0, $0, 2
# number start with 0x is hexdecimal
    addi $1, $1, 0x1F
# you can also use register name t1, t2, sp, etc, directly
    addi $t1, $sp, 62
# remember to define label use <label name>:
L0: add $1, $2, $3
# more examples
    lw $1, 2($0)
    addi $2, $3, -5
```

Disassembler
------------

`-d` for disassembler<br/>

`./assembler -d [options] InputFilename -o OutputFilename`

Options:<br/>
`-dec` show all constants in decimal (default)<br/>
`-hex` show all constants in hexdecimal<br/>
`-label` show branch target with label (default)<br/>
`-nolabel` show branch without label<br/>
An example for `-label` and `-nolabel`<br/>
With `-label`: beq $0, $0, L0<br/>
With `-nolabel`: beq $0, $0, 3
