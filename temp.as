MAIN: add r3, LIST
LOOP: prn #48
 lea STR, r6
 inc r6
 mov r3, K
 mov r3, K
 sub r1, r4
 bne END
 cmp K, #-6
 bne &END
 dec K
 jmp &LOOP
END: stop
 add r1, r2
 mov r1, r3
 sub r4, r2
STR: .string “abcd”
LIST: .data 6, -9
 .data -100
K: .data 31
 cmp K, #-6
 bne &END
