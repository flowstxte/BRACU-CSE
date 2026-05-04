import random

# ══════════════════════════════════════════════════════════════
#  LAB ASSIGNMENT 3
#  Part 1: Mortal Kombat  (Alpha-Beta Pruning)
#  Part 2: Games with Magic (Minimax + Alpha-Beta)
# ══════════════════════════════════════════════════════════════

# ──────────────────────────────────────────────────────────────
#  PART 1 — Mortal Kombat
# ──────────────────────────────────────────────────────────────
#  - Branching factor = 2, Max depth = 5  →  32 leaf nodes / round
#  - Utility: -1 = Scorpion wins,  +1 = Sub-Zero wins
#  - Scorpion = MINIMIZER,  Sub-Zero = MAXIMIZER
#  - First player alternates each round
#  - Best-of-3: first to win 2 rounds wins the game
# ──────────────────────────────────────────────────────────────

DEPTH     = 5
BRANCHING = 2

def generate_leaf_nodes():
    """Generate 2^5 = 32 random utility values (-1 or +1)."""
    return [random.choice([-1, 1]) for _ in range(BRANCHING ** DEPTH)]

def alpha_beta(depth, leaf_iter, is_maximizer, alpha, beta):
    """
    Recursive Alpha-Beta Pruning on an implicit full binary tree.
    depth      : remaining depth (0 = leaf)
    leaf_iter  : iterator over leaf values
    is_maximizer: True if current node is MAX
    Returns    : (node_value, branches_pruned)
    """
    if depth == 0:
        return next(leaf_iter), 0

    pruned = 0
    if is_maximizer:
        value = float('-inf')
        for i in range(BRANCHING):
            child_val, p = alpha_beta(depth - 1, leaf_iter, False, alpha, beta)
            pruned += p
            value = max(value, child_val)
            alpha = max(alpha, value)
            if beta <= alpha:
                # Prune remaining siblings
                remaining = BRANCHING - i - 1
                pruned += remaining
                # Skip the leaves of pruned subtrees
                for _ in range(remaining * (BRANCHING ** (depth - 1))):
                    try: next(leaf_iter)
                    except StopIteration: pass
                break
    else:
        value = float('inf')
        for i in range(BRANCHING):
            child_val, p = alpha_beta(depth - 1, leaf_iter, True, alpha, beta)
            pruned += p
            value = min(value, child_val)
            beta = min(beta, value)
            if beta <= alpha:
                remaining = BRANCHING - i - 1
                pruned += remaining
                for _ in range(remaining * (BRANCHING ** (depth - 1))):
                    try: next(leaf_iter)
                    except StopIteration: pass
                break

    return value, pruned

def mortal_kombat(first_player_input, seed=42):
    """
    first_player_input : 0 = Scorpion starts, 1 = Sub-Zero starts
    seed               : random seed for reproducibility
    """
    random.seed(seed)

    print("=" * 57)
    print("MORTAL KOMBAT")
    print("=" * 57)

    scorpion_wins = 0
    subzero_wins  = 0
    round_winners = []
    round_num     = 1
    current_first = first_player_input

    while scorpion_wins < 2 and subzero_wins < 2:
        leaves = generate_leaf_nodes()
        first_name = "Sub-Zero" if current_first == 1 else "Scorpion"

        print(f"\n  Round {round_num}  |  {first_name} goes first (depth={DEPTH}, leaves={BRANCHING**DEPTH})")
        print(f"  Leaves : {leaves}")

        # Sub-Zero starts → he is MAX;  Scorpion starts → he is MIN
        is_max = (current_first == 1)
        leaf_iter = iter(leaves)
        result, pruned = alpha_beta(DEPTH, leaf_iter, is_max, float('-inf'), float('inf'))

        winner = "Sub-Zero" if result == 1 else "Scorpion"
        round_winners.append(winner)

        print(f"  Alpha-Beta value  : {result:+d}  →  Winner: {winner}")
        print(f"  Branches pruned   : {pruned}")

        if winner == "Scorpion":
            scorpion_wins += 1
        else:
            subzero_wins  += 1

        current_first = 1 - current_first   # alternate who goes first
        round_num += 1

    game_winner  = "Scorpion" if scorpion_wins >= 2 else "Sub-Zero"
    total_rounds = round_num - 1

    print("\n" + "=" * 57)
    print(f"  Game Winner        : {game_winner}")
    print(f"  Total Rounds Played: {total_rounds}")
    for i, w in enumerate(round_winners, 1):
        print(f"  Winner of Round {i}  : {w}")
    print("=" * 57)


# ──────────────────────────────────────────────────────────────
#  PART 2 — Games with Magic
# ──────────────────────────────────────────────────────────────
#  Fixed tree (3 levels deep):
#
#           Root [PAC-MAN MAX]
#          /                  \
#     Ghost[MIN]           Ghost[MIN]
#      /      \              /      \
#  PM[MAX]  PM[MAX]      PM[MAX]  PM[MAX]
#   / \      / \          / \      / \
#  3   6    2   3        7   1    2   0
#
#  Dark Magic: Pac-Man pays cost c to turn one Ghost MIN node
#              into a MAX node → takes best instead of worst.
# ──────────────────────────────────────────────────────────────

LEAVES_P2 = [3, 6, 2, 3, 7, 1, 2, 0]

def pacman_game(c):
    """
    c : cost of using dark magic
    """
    print("=" * 57)
    print(f"Games with Magic")
    print("=" * 57)

    L = LEAVES_P2

    # ── Level 3: Pac-Man MAX nodes ────────────────────────────
    pm_A = max(L[0], L[1])   # max(3,6) = 6
    pm_B = max(L[2], L[3])   # max(2,3) = 3
    pm_C = max(L[4], L[5])   # max(7,1) = 7
    pm_D = max(L[6], L[7])   # max(2,0) = 2

    # ── Level 2: Ghost MIN nodes ──────────────────────────────
    ghost_left  = min(pm_A, pm_B)   # min(6,3) = 3
    ghost_right = min(pm_C, pm_D)   # min(7,2) = 2

    # ── Level 1: Root Pac-Man MAX ─────────────────────────────
    normal_val = max(ghost_left, ghost_right)   # max(3,2) = 3

    print(f"\n  ── Standard Minimax (no dark magic) ──")
    print(f"  Leaf values       : {L}")
    print(f"  PM-MAX nodes      : max({L[0]},{L[1]})={pm_A},  max({L[2]},{L[3]})={pm_B},  "
          f"max({L[4]},{L[5]})={pm_C},  max({L[6]},{L[7]})={pm_D}")
    print(f"  Ghost-MIN (left)  : min({pm_A},{pm_B}) = {ghost_left}")
    print(f"  Ghost-MIN (right) : min({pm_C},{pm_D}) = {ghost_right}")
    print(f"  Root value        : max({ghost_left},{ghost_right}) = {normal_val}")

    # ── Dark Magic options ────────────────────────────────────
    magic_left_best  = max(pm_A, pm_B)   # Ghost-left becomes MAX → max(6,3)=6
    magic_right_best = max(pm_C, pm_D)   # Ghost-right becomes MAX → max(7,2)=7

    magic_left_val  = magic_left_best  - c   # 6-c
    magic_right_val = magic_right_best - c   # 7-c

    print(f"\n  ── With Dark Magic (cost = {c}) ──")
    print(f"  Go LEFT  + magic  : max({pm_A},{pm_B}) - {c} = {magic_left_best} - {c} = {magic_left_val}")
    print(f"  Go RIGHT + magic  : max({pm_C},{pm_D}) - {c} = {magic_right_best} - {c} = {magic_right_val}")

    best_magic    = max(magic_left_val, magic_right_val)
    best_magic_dir = "right" if magic_right_val >= magic_left_val else "left"

    # ── Decision ─────────────────────────────────────────────
    print(f"\n  ── Decision ──")
    if best_magic > normal_val:
        print(f"  The new minimax value is {best_magic}. "
              f"Pac-Man goes {best_magic_dir} and uses dark magic ✅")
    else:
        print(f"  The minimax value is {normal_val}. "
              f"Pac-Man does not use dark magic ❌")

    # ── Alpha-Beta trace (no magic) ───────────────────────────
    print(f"\n  ── Alpha-Beta Pruning (no dark magic) ──")
    print(f"  Root MAX,  α=-∞,  β=+∞")
    print(f"  → Explore LEFT ghost (MIN, α=-∞, β=+∞):")
    print(f"      Node-A (PM MAX): max({L[0]},{L[1]}) = {pm_A}  →  β = min(∞,{pm_A}) = {pm_A}")
    print(f"      Node-B (PM MAX): max({L[2]},{L[3]}) = {pm_B}  →  ghost_left = min({pm_A},{pm_B}) = {ghost_left}")
    print(f"      ghost_left = {ghost_left}  →  root α = max(-∞,{ghost_left}) = {ghost_left}")
    print(f"  → Explore RIGHT ghost (MIN, α={ghost_left}, β=+∞):")
    print(f"      Node-C (PM MAX): max({L[4]},{L[5]}) = {pm_C}  →  β = min(∞,{pm_C}) = {pm_C}")
    print(f"      Node-D (PM MAX): max({L[6]},{L[7]}) = {pm_D}  →  ghost_right = min({pm_C},{pm_D}) = {ghost_right}")
    print(f"      β={ghost_right} ≤ α={ghost_left}? {'YES → PRUNE' if ghost_right <= ghost_left else 'No pruning'}")
    print(f"  Alpha-Beta root value = max({ghost_left},{ghost_right}) = {normal_val}")

    # ── Summary ───────────────────────────────────────────────
    print(f"\n  ── Summary ──")
    print(f"  Normal minimax    : {normal_val}")
    print(f"  Magic LEFT  (c={c}) : {magic_left_val}  {'✅ better' if magic_left_val > normal_val else '❌ worse or equal'}")
    print(f"  Magic RIGHT (c={c}) : {magic_right_val}  {'✅ better' if magic_right_val > normal_val else '❌ worse or equal'}")
    if best_magic > normal_val:
        print(f"  Dark magic IS beneficial → best choice: go {best_magic_dir} (value={best_magic})")
    else:
        print(f"  Dark magic is NOT beneficial at cost c={c}")
    print("=" * 57)


# ══════════════════════════════════════════════════════════════
#  RUN
# ══════════════════════════════════════════════════════════════

# ── Part 1 ── change player_input to 0 or 1 ──
player_input = 0    # 0 = Scorpion starts, 1 = Sub-Zero starts
mortal_kombat(player_input, seed=7)

print()

# ── Part 2 ── test both sample cases ──
pacman_game(2)
print()
pacman_game(5)

# ── Part 3: Food for Thought (0 pts) ──
print("""
╔══════════════════════════════════════════════════════╗
║          PART 3 — Food for Thought                   ║
╠══════════════════════════════════════════════════════╣
║                                                      ║
║  Q1: Is the first player always a maximizer node?    ║
║  A : NO. It depends on convention. In standard       ║
║      minimax the root is usually MAX, but the        ║
║      "first player" could be designed as MIN         ║
║      (e.g., Scorpion in this problem is MIN).        ║
║      Who is MAX/MIN is a design choice, not tied     ║
║      to who moves first.                             ║
║                                                      ║
║  Q2: Can alpha-beta pruning handle stochastic        ║
║      (random/chance) environments?                   ║
║  A : NOT directly. Alpha-beta assumes a fully        ║
║      deterministic, adversarial two-player game.     ║
║      For stochastic games, Expectiminimax is used,   ║
║      which adds CHANCE nodes. Alpha-beta pruning     ║
║      can be adapted for chance nodes but becomes     ║
║      much less effective because the expected        ║
║      value of a chance node depends on ALL its       ║
║      children — you can't safely prune them.         ║
╚══════════════════════════════════════════════════════╝
""")