op1 r1, r2
op2 r2, r1
mcro test_macro
  add, $4
  do something1
  do something 2

  test
mcroend

LALALA
mcro t1
  test1
mcroend

test_macro
