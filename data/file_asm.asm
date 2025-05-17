
call volleyball:
hlt


;=== FUNCTION DEF volleyball ===
volleyball:

;===============  ASSIGN  ============== 
push 9
pop [0]

;===============  ASSIGN  ============== 

;===============  SQRT  ============== 
push [0]
sqrt
pop [1]

;===============  PRINT  ============== 
push [1]
out

;===============  SCAN  ============== 
in
pop [2]

;===============  SCAN  ============== 
in
pop [3]

;===============  SCAN  ============== 
in
pop [4]

;===============  IF  ============== 
push 0
push [2]
jne if_skip_block_0:

;===============  ASSIGN  ============== 
push -1
push [4]
mul
push [3]
div
pop [5]

;===============  PRINT  ============== 
push [5]
out
if_skip_block_0:

;===============  ASSIGN  ============== 
push [3]
push [3]
mul
push 4
push [2]
mul
push [4]
mul
sub
pop [6]

;===============  ASSIGN  ============== 

;===============  SQRT  ============== 
push [6]
sqrt
pop [4]

;===============  PRINT  ============== 
push [4]
out

;===============  IF  ============== 
push 0
push [6]
jae if_skip_block_1:

;===============  PRINT  ============== 
push 999
out
if_skip_block_1:

;===============  IF  ============== 
push 0
push [6]
jne if_skip_block_2:

;===============  ASSIGN  ============== 
push -1
push [3]
mul
push 2
push [2]
mul
div
pop [7]

;===============  PRINT  ============== 
push [7]
out
if_skip_block_2:

;===============  IF  ============== 
push 0
push [6]
jbe if_skip_block_3:

;===============  ASSIGN  ============== 
push -1
push [3]
mul
push [4]
sub
push 2
push [2]
mul
div
pop [7]

;===============  ASSIGN  ============== 
push -1
push [3]
mul
push [4]
add
push 2
push [2]
mul
div
pop [8]

;===============  PRINT  ============== 
push [7]
out

;===============  PRINT  ============== 
push [8]
out
if_skip_block_3:

;=== RETURN ===
ret

