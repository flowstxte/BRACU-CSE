def parse_graph(path):
    graph = {}
    try:
        with open(path, 'r') as file:
            for l in file:
                l = l.strip()
                if '->' not in l:
                    continue
                try:
                    u_str, v_str = l.split('->')
                    u = int(u_str.strip())
                    v_list = []
                    if v_str.strip():
                        v_list = list(map(int, v_str.strip().split(',')))
                    graph[u] = v_list
                    for v in v_list:
                        if v not in graph:
                            graph[v] = []
                except ValueError:
                    continue
    except FileNotFoundError:
        print(f"Error: File not found at {path}")
        return None
    except Exception as e:
        print(f"Error reading graph file: {e}")
        return None
    return graph

def write_out(path, c):
    try:
        with open(path, 'w') as file:
            file.write(c)
    except Exception as e:
        print(f"Error writing to file: {e}")

def eulerian_path(graph):
    ind = {}
    outd = {}
    nodes = set()
    total = 0
    for u in graph:
        nodes.add(u)
        for v in graph[u]:
            nodes.add(v)

    for i in nodes:
        ind[i] = 0
        outd[i] = 0

    for u in graph:
        outd[u] = len(graph[u])
        total += len(graph[u])
        for v in graph[u]:
            ind[v] = ind.get(v, 0) + 1
    
    start = end = None
    for i in nodes:
        diff = outd[i] - ind[i]
        if diff == 1:
            if start:
                return "no eulerian path"
            start = i
        elif diff == -1:
            if end:
                return "no eulerian path"
            end = i
        elif diff != 0:
            return "no eulerian path"
    
    if start is None:
        start = next((i for i in nodes if outd[i] > 0), None)
        if start is None and total > 0:
            return "no eulerian path"
        elif start is None and total == 0:
            return "no eulerian path"
    st = [start]
    path = []
    tg = {u: list(v) for u, v in graph.items()}

    while st:
        u = st[-1]
        if tg[u]:
            st.append(tg[u].pop())
        else:
            path.append(st.pop())
    
    if len(path) != total + 1:
        return "no eulerian path"
    
    for u in nodes:
        if tg[u]:
            return "no eulerian path"

    return '->'.join(map(str, path[::-1]))

inp='D:\\CSE443_Assignments\\Assignment 01\\sample2.txt'
out='D:\\CSE443_Assignments\\Assignment 01\\output2.txt'
graph = parse_graph(inp)
path = eulerian_path(graph)
write_out(out, path)