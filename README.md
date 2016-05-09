# Computer_Architecture_Mini_Assembler

Design
------

A mini assembler/disassembler for NTHU course Computer Architecture

Language used: MIPS(simplified)

Build
-----
`make` or `cmake <path to source>`<br/>
Output Filename would be `assembler`

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
An example MIPS codes:<br/>
somewhere: add $1, $2, $3<br/>
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;j somewhere

Disassembler
------------

`-d` for disassembler<br/>

`./assembler -d [options] InputFilename -o OutputFilename`

Options:<br/>
`-dec` show all constants in decimal(default)<br/>
`-hex` show all constants in hexdecimal<br/>
`-label` show branch target with label(default)<br/>
`-nolabel` show branch without label<br/>
An example for `-label` and `-nolabel`<br/>
With `-label`: beq $0, $0, L0<br/>
With `-nolabel`: beq $0, $0, 1
