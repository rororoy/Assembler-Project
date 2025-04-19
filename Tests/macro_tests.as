; MACRO TESTS:

; This should return an error due to extranous text at the end
mcro macro_1   something
  add r1,r2
mcroend

; Again:

mcro macro_1
  add r1,r2
mcroend   something


; Saved word:
mcro add
  add r1,r2
mcroend
