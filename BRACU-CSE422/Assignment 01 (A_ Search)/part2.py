from collections import deque

def bfs_from_goal(graph, goal):
    dist = {goal: 0}
    queue = deque([goal])
    while queue:
        u = queue.popleft()
        for v in graph[u]:
            if v not in dist:
                dist[v] = dist[u] + 1
                queue.append(v)
    return dist

def solve(input_text):
    lines = [line.strip() for line in input_text.strip().splitlines() if line.strip()]
    idx = 0

    n, m   = map(int, lines[idx].split()); idx += 1
    a, b   = map(int, lines[idx].split()); idx += 1   # start, goal

    heuristics = {}
    for _ in range(n):
        x, y = map(int, lines[idx].split()); idx += 1
        heuristics[x] = y

    graph = {i: [] for i in range(1, n + 1)}
    for _ in range(m):
        u, v = map(int, lines[idx].split()); idx += 1
        graph[u].append(v)
        graph[v].append(u)

    true_dist = bfs_from_goal(graph, b)

    inadmissible = []
    for node in range(1, n + 1):
        h_val  = heuristics[node]
        actual = true_dist.get(node, float('inf'))
        if h_val > actual:
            inadmissible.append(node)

    if not inadmissible:
        print("The heuristic values are admissible.")
    else:
        print(0)
        nodes_str = ", ".join(str(x) for x in inadmissible)
        print(f"Here nodes {nodes_str} are inadmissible.")

input1 = """
6 7
1 6
1 3
2 2
3 1
4 2
5 1
6 0
1 2
2 3
3 6
1 4
4 5
5 6
3 5
"""

input2 = """
6 7
1 6
1 6
2 4
3 2
4 5
5 2
6 0
1 2
2 3
3 6
1 4
4 5
5 6
3 5
"""

input3 = """
5 5
2 4
1 0
2 2
3 2
4 0
5 1
5 2
2 3
1 4
4 5
5 3
"""

print("=== Test 1 (expected: admissible) ===")
solve(input1)

print("\n=== Test 2 (expected: 0, nodes 1,2,3,4,5 inadmissible) ===")
solve(input2)

print("\n=== Test 3 (expected: admissible) ===")
solve(input3)