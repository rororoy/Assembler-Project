;  ###### Comment tests  ######
;LABEL: ;This comment should be ignored
T;his comment should return an error


; ###### LABEL TESTING ######
; Valid inputs:
l4bel1: add r1, r1
Label2: inc r1

; Invalid inputs:
1Label3: dec r1
Label_4: jmp &JmpLabel
ThisLabelIsTooLongIts33CharsLongg: add r5, r7
add r1, r2 Label4:
LabelWithNoCommand:


; ###### COMMANDS TESTING ######
; Valid inputs
add r1, r2

; Invalid inputs of wrong addressing modes in commands:
add &Label2, r2
add #-5, Label2
lea r3
jmp r5
stop Label2
dec &Label2

; Invalid operand usages
add r1 , , r2
add r1,
add r1 r2



; ###### MACRO TESTING ######
; Invalid macro tests are in macro_tests
; Valid inputs:
mcro macro_1
  Label5: add r1, r1
  add r2, r2
  add r3, r3
mcroend

; Unpacking macro:
macro_1



; ###### DATA TESTING ######
; Valid inputs:
STR: .string "abcde"
LIST: .data 6, -9, 100, 1024, -1, 0
.data -100
K: .data 31

; Invalid inputs:
.string "abcde
.data
