def read_input(filename):
    with open(filename, "r") as f:
        lines = [line.strip() for line in f if line.strip()]

    lines = [line for line in lines if not line.startswith("-")]
    sequence = lines[0]
    alphabet = lines[1].split()
    states = lines[2].split()
    trans_start = 3 + 1
    trans = {}
    for i, st in enumerate(states):
        parts = lines[trans_start + i].split()
        probs = list(map(float, parts[1:]))
        trans[st] = dict(zip(states, probs))

    emiss_start = trans_start + len(states) + 1
    emiss = {}
    for i, st in enumerate(states):
        parts = lines[emiss_start + i].split()
        probs = list(map(float, parts[1:]))
        emiss[st] = dict(zip(alphabet, probs))

    return sequence, alphabet, states, trans, emiss

def viterbi(sequence, states, trans, emiss):
    n = len(sequence)
    dp = {st: [0] * n for st in states}
    back = {st: [None] * n for st in states}
    for st in states:
        dp[st][0] = emiss[st][sequence[0]]

    for i in range(1, n):
        for st in states:
            max_prob, prev_st = max(
                (dp[ps][i-1] * trans[ps][st] * emiss[st][sequence[i]], ps)
                for ps in states
            )
            dp[st][i] = max_prob
            back[st][i] = prev_st

    last_state = max(states, key=lambda st: dp[st][n-1])
    path = [last_state]
    for i in range(n-1, 0, -1):
        last_state = back[last_state][i]
        path.append(last_state)

    return "".join(reversed(path))

sequence, alphabet, states, trans, emiss = read_input("D:\\CSE443_Assignments\\Assignment 02\\input.txt")
path = viterbi(sequence, states, trans, emiss)
with open("D:\\CSE443_Assignments\\Assignment 02\\output.txt", "w") as f:
    f.write(path + "\n")