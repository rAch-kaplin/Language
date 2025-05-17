
call volleyball:
hlt


;=== FUNCTION DEF volleyball ===
volleyball:

;===============  SCAN  ============== 
in
pop [0]

;===============  SCAN  ============== 
in
pop [1]

;===============  SCAN  ============== 
in
pop [2]

;===============  IF  ============== 
push 0
push [0]
jne if_skip_block_0:

;===============  IF  ============== 
push 0
push [1]
jne if_skip_block_1:

;===============  PRINT  ============== 
push 999
out

;=== RETURN ===
ret
if_skip_block_1:

;=== FUNCTION CALL SolveLinSq ===
call SolveLinSq:

;=== RETURN ===
ret
if_skip_block_0:
push [1]
push [1]
mul
push 4
push [0]
mul
push [2]
mul
sub
pop [3]

;===============  SQRT  ============== 
push [3]
sqrt
pop [2]

;===============  IF  ============== 
push 0
push [3]
jne if_skip_block_2:
push -1
push [1]
mul
push 2
push [0]
mul
div
pop [4]

;===============  PRINT  ============== 
push [4]
out

;=== RETURN ===
ret
if_skip_block_2:

;===============  IF  ============== 
push 0
push [3]
ja if_skip_block_3:

;=== FUNCTION CALL SolveQuadSq ===
call SolveQuadSq:

;=== RETURN ===
ret
if_skip_block_3:

;===============  IF  ============== 
push 0
push [3]
jb if_skip_block_4:

;===============  PRINT  ============== 
push 999
out
if_skip_block_4:

;=== RETURN ===
ret

;=== FUNCTION DEF SolveLinSq ===
SolveLinSq:
push -1
push [2]
mul
push [1]
div
pop [5]

;===============  PRINT  ============== 
push [5]
out

;=== RETURN ===
ret

;=== FUNCTION DEF SolveQuadSq ===
SolveQuadSq:
push -1
push [1]
mul
push [2]
sub
push 2
push [0]
mul
div
pop [6]
push -1
push [1]
mul
push [2]
add
push 2
push [0]
mul
div
pop [7]

;===============  PRINT  ============== 
push [6]
out

;===============  PRINT  ============== 
push [7]
out

;=== RETURN ===
ret

