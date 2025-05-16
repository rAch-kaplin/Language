in
pop [0]
push 0
push [0]
jne if_skip_block_0:
push 1
out
if_skip_block_0:
push 0
push [0]
je if_skip_block_1:
push 1
pop [1]
push 1
pop [2]
while_start_0:
push [0]
push [2]
jbe while_end_0:
push [2]
push 1
add
pop [2]
push [1]
push [2]
mul
pop [1]
jmp while_start_0:
while_end_0:
push [1]
out
if_skip_block_1:
hlt

