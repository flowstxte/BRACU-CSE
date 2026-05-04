import heapq

def manhattan(r1, c1, r2, c2):
    return abs(r1 - r2) + abs(c1 - c2)

def a_star_maze(n, m, start, goal, maze):
    sr, sc = start
    gr, gc = goal

    # (f_cost, g_cost, row, col, path_string)
    heap = [(manhattan(sr, sc, gr, gc), 0, sr, sc, "")]
    visited = {}

    while heap:
        f, g, r, c, path = heapq.heappop(heap)

        if (r, c) in visited:
            continue
        visited[(r, c)] = g

        if (r, c) == (gr, gc):
            return g, path

        for dr, dc, action in [(-1, 0, 'U'), (1, 0, 'D'), (0, -1, 'L'), (0, 1, 'R')]:
            nr, nc = r + dr, c + dc
            if 0 <= nr < n and 0 <= nc < m and maze[nr][nc] == '0' and (nr, nc) not in visited:
                ng = g + 1
                nf = ng + manhattan(nr, nc, gr, gc)
                heapq.heappush(heap, (nf, ng, nr, nc, path + action))

    return -1, None

def solve(input_text):
    lines = [line.strip() for line in input_text.strip().splitlines() if line.strip()]
    idx = 0

    n, m   = map(int, lines[idx].split()); idx += 1
    sr, sc = map(int, lines[idx].split()); idx += 1
    gr, gc = map(int, lines[idx].split()); idx += 1

    maze = []
    for i in range(n):
        maze.append(lines[idx]); idx += 1

    cost, path = a_star_maze(n, m, (sr, sc), (gr, gc), maze)

    if cost == -1:
        print(-1)
    else:
        print(cost)
        print(path)

input1 = """
7 8
0 3
6 5
###0####
#000000#
###0#00#
#000#00#
#0######
#000000#
#####0##
"""

input2 = """
10 12
0 1
9 8
#0##########
#000000###0#
###0###0##0#
#0#0#000##0#
#0#0###0##0#
#0000#00##0#
###0###0##0#
#000#0000#0#
#0#####0000#
#######0####
"""

input3 = """
15 15
1 0
13 14
###############
0000#########0#
###0#####00##0#
#00000000#0##0#
#0######0####0#
#0######0#0##0#
#0000000000000#
##########0####
#0######000####
#0######000000#
#0##########00#
#0000000000000#
###0#######0###
#00000000000000
###############
"""

print("=== Test 1 ===")
solve(input1)

print("\n=== Test 2 ===")
solve(input2)

print("\n=== Test 3 ===")
solve(input3)