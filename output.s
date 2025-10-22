.data

.text
.globl main


# Function: double
func_double:
    # Prologue
    addi $sp, $sp, -8
    sw $ra, 4($sp)
    sw $fp, 0($sp)
    move $fp, $sp
    # Parameter 0: x
    sw $a0, 8($fp)
    # Allocate space for 1 local variables
    addi $sp, $sp, -4
    # Declared temp
    lw $t0, 8($fp)
    lw $t1, 8($fp)
    add $t0, $t0, $t1
    sw $t0, -4($fp)
    lw $t0, -4($fp)
    move $v0, $t0
    # Return statement
    addi $sp, $sp, 4
    move $sp, $fp
    lw $fp, 0($sp)
    lw $ra, 4($sp)
    addi $sp, $sp, 8
    jr $ra
    # Epilogue
    addi $sp, $sp, 4
    move $sp, $fp
    lw $fp, 0($sp)
    lw $ra, 4($sp)
    addi $sp, $sp, 8
    jr $ra

# Function: triple
func_triple:
    # Prologue
    addi $sp, $sp, -8
    sw $ra, 4($sp)
    sw $fp, 0($sp)
    move $fp, $sp
    # Parameter 0: x
    sw $a0, 8($fp)
    # Allocate space for 1 local variables
    addi $sp, $sp, -4
    # Declared temp
    lw $t0, 8($fp)
    lw $t1, 8($fp)
    add $t0, $t0, $t1
    lw $t1, 8($fp)
    add $t0, $t0, $t1
    sw $t0, -4($fp)
    lw $t0, -4($fp)
    move $v0, $t0
    # Return statement
    addi $sp, $sp, 4
    move $sp, $fp
    lw $fp, 0($sp)
    lw $ra, 4($sp)
    addi $sp, $sp, 8
    jr $ra
    # Epilogue
    addi $sp, $sp, 4
    move $sp, $fp
    lw $fp, 0($sp)
    lw $ra, 4($sp)
    addi $sp, $sp, 8
    jr $ra

# Function: main
main:
    # Prologue
    addi $sp, $sp, -8
    sw $ra, 4($sp)
    sw $fp, 0($sp)
    move $fp, $sp
    # Allocate space for 1 local variables
    addi $sp, $sp, -4
    # Declared x
    li $t0, 5
    sw $t0, -4($fp)
    # Save $ra before nested call
    addi $sp, $sp, -4
    sw $ra, 0($sp)
    lw $t0, -4($fp)
    move $a0, $t0
    jal func_double
    # Restore $ra after nested call
    lw $ra, 0($sp)
    addi $sp, $sp, 4
    move $t1, $v0
    # Print integer
    move $a0, $t1
    li $v0, 1
    syscall
    # Print newline
    li $v0, 11
    li $a0, 10
    syscall
    # Save $ra before nested call
    addi $sp, $sp, -4
    sw $ra, 0($sp)
    lw $t0, -4($fp)
    move $a0, $t0
    jal func_triple
    # Restore $ra after nested call
    lw $ra, 0($sp)
    addi $sp, $sp, 4
    move $t1, $v0
    # Print integer
    move $a0, $t1
    li $v0, 1
    syscall
    # Print newline
    li $v0, 11
    li $a0, 10
    syscall
    li $t0, 0
    move $v0, $t0
    # Return statement
    addi $sp, $sp, 4
    move $sp, $fp
    lw $fp, 0($sp)
    lw $ra, 4($sp)
    addi $sp, $sp, 8
    jr $ra
    # Epilogue
    addi $sp, $sp, 4
    move $sp, $fp
    lw $fp, 0($sp)
    lw $ra, 4($sp)
    addi $sp, $sp, 8
    jr $ra

# Exit program
_exit:
    li $v0, 10
    syscall
