N = 4
M = 4
size = N * M

def get_index(i, j): return i + j * size

def update_constraint_set(number, i, j):
    index = get_index(i, j)
    return '\tbool valid{} = (domain_sizes[{}] -= !( constraints[{} + value]++ )) != 0; // Cell ({}, {})\n'.format(number, index, index * size, i, j)

def update_constraint_reset(i, j):
    index = get_index(i, j)
    return '\tdomain_sizes[{}] += !( --constraints[{} + value] ); // Cell ({}, {})\n'.format(index, index * size, i, j)

def output_update_domains_set(file, x, y):
    bx = M * int(x / M)
    by = N * int(y / N)
    bxe = bx + M
    bye = by + N

    indices = []

    file.write('bool Generated::update_domains_set_{}_{}(unsigned char domain_sizes[], unsigned char constraints[], int value) {{\n'.format(x, y))
    file.write('\tbool valid = true;\n\n')
    file.write('\t// Update current row\n')

    for i in range(0, x):    
        indices.append(get_index(i, y))
        file.write(update_constraint_set(len(indices), i, y))
    for i in range(x+1, size): 
        indices.append(get_index(i, y))
        file.write(update_constraint_set(len(indices), i, y))

    file.write('\n\t// Check whether all domains are still valid (i.e. non-empty)\n')
    file.write('\tvalid = ')

    for i in range(1, len(indices)):
        file.write('valid{} && '.format(i))
    file.write('valid{};\n'.format(len(indices)))
    temp = len(indices) + 1

    file.write('\n')
    file.write('\t// Update current column\n')

    for j in range(0, y):    
        if j % N != 0: 
            indices.append(get_index(x, j))
            file.write(update_constraint_set(len(indices), x, j))
    for j in range(y+1, size):
        if j % N != 0: 
            indices.append(get_index(x, j))
            file.write(update_constraint_set(len(indices), x, j))

    file.write('\n\t// Check whether all domains are still valid (i.e. non-empty)\n')
    file.write('\tvalid = valid && ')

    for i in range(temp, len(indices)):
        file.write('valid{} && '.format(i))
    file.write('valid{};\n'.format(len(indices)))
    temp = len(indices) + 1

    file.write('\n')
    file.write('\t// Update current block\n')

    for j in range(by+1, y):
        for i in range(bx,  x):   
            indices.append(get_index(i, j))
            file.write(update_constraint_set(len(indices), i, j))
        for i in range(x+1, bxe): 
            indices.append(get_index(i, j))
            file.write(update_constraint_set(len(indices), i, j))
    for j in range(y+1, bye):
        for i in range(bx,  x):   
            indices.append(get_index(i, j))
            file.write(update_constraint_set(len(indices), i, j))
        for i in range(x+1, bxe): 
            indices.append(get_index(i, j))
            file.write(update_constraint_set(len(indices), i, j))

    file.write('\n\t// Check whether all domains are still valid (i.e. non-empty)\n')
    file.write('\treturn valid && ')

    for i in range(temp, len(indices)):
        file.write('valid{} && '.format(i))
    file.write('valid{};\n'.format(len(indices)))
    file.write('}\n\n')

def output_update_domains_reset(file, x, y):
    bx = M * int(x / M)
    by = N * int(y / N)
    bxe = bx + M
    bye = by + N

    file.write('void Generated::update_domains_reset_{}_{}(unsigned char domain_sizes[], unsigned char constraints[], int value) {{\n'.format(x, y))
    file.write('\t// Update current row\n')

    for i in range(0,   x):    file.write(update_constraint_reset(i, y))
    for i in range(x+1, size): file.write(update_constraint_reset(i, y))

    file.write('\n')
    file.write('\t// Update current column\n')

    for j in range(0, y): 
        if j % N != 0: file.write(update_constraint_reset(x, j))
    for j in range(y+1, size):
        if j % N != 0: file.write(update_constraint_reset(x, j))

    file.write('\n')
    file.write('\t// Update current block\n')

    for j in range(by+1, y):
        for i in range(bx,  x):   file.write(update_constraint_reset(i, j))
        for i in range(x+1, bxe): file.write(update_constraint_reset(i, j))
    for j in range(y+1, bye):
        for i in range(bx,  x):   file.write(update_constraint_reset(i, j))
        for i in range(x+1, bxe): file.write(update_constraint_reset(i, j))

    file.write('}\n\n')

# Output the header file
with open('../Generated.h', 'w') as file:
    file.write('''// File generated by Python script, do not modify

typedef bool (* SetFunction)  (unsigned char domain_sizes[], unsigned char constraints[], int value);
typedef void (* ResetFunction)(unsigned char domain_sizes[], unsigned char constraints[], int value);

struct Generated {{
inline static constexpr int N = {N}; 
inline static constexpr int M = {M};

'''.format(N=N, M=M, size=size*size))

    for y in range(0, size):
        for x in range(0, size):
            file.write('static bool update_domains_set_{}_{}  (unsigned char domain_sizes[], unsigned char constraints[], int value);\n'.format(x, y, N, M))
            file.write('static void update_domains_reset_{}_{}(unsigned char domain_sizes[], unsigned char constraints[], int value);\n'.format(x, y, N, M))
            file.write('\n')

    file.write('inline static constexpr const SetFunction table_set[{}] = {{\n'.format(size * size))
    for y in range(0, size):
        for x in range(0, size):
            file.write('\tupdate_domains_set_{}_{},\n'.format(x, y))
    file.write('};\n')
    file.write('inline static constexpr const ResetFunction table_reset[{}] = {{\n'.format(size * size))
    for y in range(0, size):
        for x in range(0, size):
            file.write('\tupdate_domains_reset_{}_{},\n'.format(x, y))
    file.write('};\n')
    file.write('};\n')

# Output the implementation file
with open('../Generated.cpp', 'w') as file:
    file.write('''// File generated by Python script, do not modify
#include "Generated.h"

''')

    for y in range(0, size):
        for x in range(0, size):
            output_update_domains_set(file, x, y)
            output_update_domains_reset(file, x, y)

print('File written successfully!')