import sys

def load_blosum62():
    blosum62_str = """   A  R  N  D  C  Q  E  G  H  I  L  K  M  F  P  S  T  W  Y  V
A  4 -1 -2 -2  0 -1 -1  0 -2 -1 -1 -1 -1 -2 -1  1  0 -3 -2  0
R -1  5  0 -2 -3  1  0 -2  0 -3 -2  2 -1 -3 -2 -1 -1 -3 -2 -3
N -2  0  6  1 -3  0  0  0  1 -3 -3  0 -2 -3 -2  1  0 -4 -2 -3
D -2 -2  1  6 -3  0  2 -1 -1 -3 -4 -1 -3 -3 -1  0 -1 -4 -3 -3
C  0 -3 -3 -3  9 -3 -4 -3 -3 -1 -1 -3 -1 -2 -3 -1 -1 -2 -2 -1
Q -1  1  0  0 -3  5  2 -2  0 -3 -2  1  0 -3 -1  0 -1 -2 -1 -2
E -1  0  0  2 -4  2  5 -2  0 -3 -3  1 -2 -3 -1  0 -1 -3 -2 -2
G  0 -2  0 -1 -3 -2 -2  6 -2 -4 -4 -2 -3 -3 -2  0 -2 -2 -3 -3
H -2  0  1 -1 -3  0  0 -2  8 -3 -3 -1 -2 -1 -2 -1 -2 -2  2 -3
I -1 -3 -3 -3 -1 -3 -3 -4 -3  4  2 -3  1  0 -3 -2 -1 -3 -1  3
L -1 -2 -3 -4 -1 -2 -3 -4 -3  2  4 -2  2  0 -3 -2 -1 -2 -1  1
K -1  2  0 -1 -3  1  1 -2 -1 -3 -2  5 -1 -3 -1  0 -1 -3 -2 -2
M -1 -1 -2 -3 -1  0 -2 -3 -2  1  2 -1  5  0 -2 -1 -1 -1 -1  1
F -2 -3 -3 -3 -2 -3 -3 -3 -1  0  0 -3  0  6 -4 -2 -2  1  3 -1
P -1 -2 -2 -1 -3 -1 -1 -2 -2 -3 -3 -1 -2 -4  7 -1 -1 -4 -3 -2
S  1 -1  1  0 -1  0  0  0 -1 -2 -2  0 -1 -2 -1  4  1 -3 -2 -2
T  0 -1  0 -1 -1 -1 -1 -2 -2 -1 -1 -1 -1 -2 -1  1  5 -2 -2  0
W -3 -3 -4 -4 -2 -2 -3 -2 -2 -3 -2 -3 -1  1 -4 -3 -2 11  2 -3
Y -2 -2 -2 -3 -2 -1 -2 -3  2 -1 -1 -2 -1  3 -3 -2 -2  2  7 -1
V  0 -3 -3 -3 -1 -2 -2 -3 -3  3  1 -2  1 -1 -2 -2  0 -3 -1  4"""
    lines = blosum62_str.split("\n")
    headers = lines[0].split()
    matrix = {}
    for line in lines[1:]:
        parts = line.split()
        row, scores = parts[0], list(map(int, parts[1:]))
        matrix[row] = dict(zip(headers, scores))
    return matrix

def affine_alignment(v, w, sigma=11, epsilon=1):
    n, m = len(v), len(w)
    BLOSUM62 = load_blosum62()
    NEG_INF = -10**9
    M = [[NEG_INF]*(m+1) for _ in range(n+1)]
    X = [[NEG_INF]*(m+1) for _ in range(n+1)]
    Y = [[NEG_INF]*(m+1) for _ in range(n+1)]
    backM = [[None]*(m+1) for _ in range(n+1)]
    backX = [[None]*(m+1) for _ in range(n+1)]
    backY = [[None]*(m+1) for _ in range(n+1)]

    M[0][0] = 0
    for i in range(1, n+1):
        X[i][0] = -sigma - (i-1)*epsilon
    for j in range(1, m+1):
        Y[0][j] = -sigma - (j-1)*epsilon
    for i in range(1, n+1):
        for j in range(1, m+1):
            scores = [
                (M[i-1][j-1], "M"),
                (X[i-1][j-1], "X"),
                (Y[i-1][j-1], "Y") ]
            best_prev, state = max(scores, key=lambda x: x[0])
            M[i][j] = best_prev + BLOSUM62[v[i-1]][w[j-1]]
            backM[i][j] = state
            scores = [
                (M[i-1][j] - sigma, "M"),
                (X[i-1][j] - epsilon, "X"),
                (Y[i-1][j] - sigma, "Y")]
            X[i][j], backX[i][j] = max(scores, key=lambda x: x[0])
            scores = [
                (M[i][j-1] - sigma, "M"),
                (Y[i][j-1] - epsilon, "Y"),
                (X[i][j-1] - sigma, "X")]
            Y[i][j], backY[i][j] = max(scores, key=lambda x: x[0])

    final_scores = [(M[n][m], "M"), (X[n][m], "X"), (Y[n][m], "Y")]
    score, state = max(final_scores, key=lambda x: x[0])
    aln_v, aln_w = [], []
    i, j = n, m
    while i > 0 or j > 0:
        if state == "M":
            prev_state = backM[i][j]
            aln_v.append(v[i-1])
            aln_w.append(w[j-1])
            i, j = i-1, j-1
        elif state == "X":
            prev_state = backX[i][j]
            aln_v.append(v[i-1])
            aln_w.append("-")
            i -= 1
        else:
            prev_state = backY[i][j]
            aln_v.append("-")
            aln_w.append(w[j-1])
            j -= 1
        state = prev_state

    return score, "".join(reversed(aln_v)), "".join(reversed(aln_w))

with open("D:\\CSE443_Assignments\\Assignment 02\\input.txt") as f:
    v = f.readline().strip()
    w = f.readline().strip()
score, aln_v, aln_w = affine_alignment(v, w)

with open("D:\\CSE443_Assignments\\Assignment 02\\output.txt", "w") as f:
    f.write(str(score) + "\n")
    f.write(aln_v + "\n")
    f.write(aln_w + "\n")