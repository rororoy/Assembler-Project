MAIN: add r3, LIST
LOOP: prn #48
mcro a_mc
 cmp K, #-6
 bne &END
mcroend
 lea STR, r6
 inc r6
 mov r3, K
mcro b_mc
 add r1, r2
 mov r1, r3
 sub r4, r2
mcroend
 mov r3, K
 sub r1, r4
 bne END
a_mc
 dec K
 jmp &LOOP
END: stop
b_mc
STR: .string "ab cd"
LIST: .data 6, -9,5,4
 .data -100
K: .data 31
a_mc
