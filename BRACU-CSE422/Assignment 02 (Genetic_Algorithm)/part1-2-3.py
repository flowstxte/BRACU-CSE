import random

# ─────────────────────────────────────────────
#  CONFIGURATION
# ─────────────────────────────────────────────
INITIAL_CAPITAL    = 1000
HISTORICAL_PRICES  = [-1.2, 3.4, -0.8, 2.1, -2.5, 1.7, -0.3, 5.8, -1.1, 3.5]
INITIAL_POPULATION = [
    {"stop_loss": 2,   "take_profit": 5, "trade_size": 20},
    {"stop_loss": 3,   "take_profit": 7, "trade_size": 30},
    {"stop_loss": 1.5, "take_profit": 4, "trade_size": 25},
    {"stop_loss": 2.5, "take_profit": 6, "trade_size": 15},
]
GENERATIONS    = 10
MUTATION_RATE  = 0.05
POPULATION_SIZE = 4

random.seed(42)   # for reproducibility

# ─────────────────────────────────────────────
#  HELPER: encode / decode chromosome strings
# ─────────────────────────────────────────────
def encode(chrom):
    """Dict → 6-char string  e.g. {'stop_loss':2,'take_profit':5,'trade_size':20} → '020520'"""
    sl = round(chrom["stop_loss"])
    tp = round(chrom["take_profit"])
    ts = round(chrom["trade_size"])
    return f"{sl:02d}{tp:02d}{ts:02d}"

def decode(s):
    """6-char string → dict"""
    return {
        "stop_loss":   int(s[0:2]),
        "take_profit": int(s[2:4]),
        "trade_size":  int(s[4:6]),
    }

# ─────────────────────────────────────────────
#  STEP 3 – FITNESS FUNCTION
# ─────────────────────────────────────────────
def simulate_trades(chrom, prices=HISTORICAL_PRICES, capital=INITIAL_CAPITAL, verbose=False):
    sl = chrom["stop_loss"]   / 100
    tp = chrom["take_profit"] / 100
    ts = chrom["trade_size"]  / 100

    if verbose:
        print(f"\n{'Day':<5}{'Price%':<10}{'Trade$':<12}{'Exit':<18}{'P/L$':<10}{'Capital$'}")
        print("-" * 65)

    for i, price_change_pct in enumerate(prices):
        trade_usd = round(capital * ts, 2)
        price_frac = price_change_pct / 100

        # Apply stop-loss / take-profit caps
        if price_change_pct < 0 and abs(price_frac) > sl:
            actual_frac = -sl
            exit_cond   = "Stop-Loss hit"
        elif price_change_pct > 0 and price_frac > tp:
            actual_frac = tp
            exit_cond   = "Take-Profit hit"
        else:
            actual_frac = price_frac
            exit_cond   = "No SL/TP hit"

        pnl = round(trade_usd * actual_frac, 2)
        capital = round(capital + pnl, 2)

        if verbose:
            print(f"{i+1:<5}{price_change_pct:<10}{trade_usd:<12}{exit_cond:<18}{pnl:<10}{capital}")

    return capital

def fitness(chrom):
    return round(simulate_trades(chrom) - INITIAL_CAPITAL, 4)

# ─────────────────────────────────────────────
#  STEP 4 – RANDOM PARENT SELECTION
# ─────────────────────────────────────────────
def select_parents(population):
    return random.sample(population, 2)

# ─────────────────────────────────────────────
#  STEP 5 – SINGLE-POINT CROSSOVER
# ─────────────────────────────────────────────
def single_point_crossover(p1, p2):
    s1, s2 = encode(p1), encode(p2)
    point = random.randint(1, len(s1) - 1)
    child1_str = s1[:point] + s2[point:]
    child2_str = s2[:point] + s1[point:]
    return decode(child1_str), decode(child2_str), point

# ─────────────────────────────────────────────
#  STEP 6 – MUTATION
# ─────────────────────────────────────────────
def mutate(chrom, mutation_rate=MUTATION_RATE):
    s = list(encode(chrom))
    mutated = False
    for i in range(len(s)):
        if random.random() < mutation_rate:
            s[i] = str(random.randint(0, 9))
            mutated = True
    new_chrom = decode("".join(s))
    # Clamp to valid range [1..99]
    for key in new_chrom:
        new_chrom[key] = max(1, min(99, new_chrom[key]))
    return new_chrom, mutated

# ─────────────────────────────────────────────
#  STEP 7 – NEXT GENERATION
# ─────────────────────────────────────────────
def next_generation(population):
    # Sort by fitness descending
    ranked = sorted(population, key=fitness, reverse=True)

    new_pop = [ranked[0]]   # elitism: keep the best

    while len(new_pop) < POPULATION_SIZE:
        p1, p2 = select_parents(ranked)
        c1, c2, _ = single_point_crossover(p1, p2)
        c1, _ = mutate(c1)
        c2, _ = mutate(c2)
        new_pop.append(c1)
        if len(new_pop) < POPULATION_SIZE:
            new_pop.append(c2)

    return new_pop


# ══════════════════════════════════════════════════════════════
#  MAIN  –  run the GA
# ══════════════════════════════════════════════════════════════
def run_ga():
    separator = "=" * 65

    # ── Show worked example from the PDF ──────────────────────
    print(separator)
    print("  WORKED EXAMPLE")
    print(separator)
    simulate_trades({"stop_loss": 2, "take_profit": 5, "trade_size": 20}, verbose=True)
    print(f"\n  Fitness = {fitness({'stop_loss':2,'take_profit':5,'trade_size':20}):.2f}")

    # ── PART 1 ─────────────────────────────────────────────────
    print(f"\n{separator}")
    print("  PART 1")
    print(separator)

    population = [c.copy() for c in INITIAL_POPULATION]

    print("\n Initial Population:")
    print(f"  {'#':<4}{'Encoded':<12}{'stop_loss':<12}{'take_profit':<14}{'trade_size':<12}{'Fitness $'}")
    print("  " + "-" * 60)
    for i, c in enumerate(population):
        print(f"  {i+1:<4}{encode(c):<12}{c['stop_loss']:<12}{c['take_profit']:<14}{c['trade_size']:<12}{fitness(c):.4f}")

    for gen in range(1, GENERATIONS + 1):
        population = next_generation(population)
        best = max(population, key=fitness)
        print(f"\n  Generation {gen:>2} │ Best so far → {encode(best)}  "
              f"(SL={best['stop_loss']}%, TP={best['take_profit']}%, TS={best['trade_size']}%)  "
              f"Fitness=${fitness(best):.4f}")

    best_strategy = max(population, key=fitness)
    final_profit  = fitness(best_strategy)

    print(f"\n{'─'*65}")
    print("  FINAL RESULT (Part 1)")
    print(f"{'─'*65}")
    print(f"  best_strategy : stop_loss={best_strategy['stop_loss']}%, "
          f"take_profit={best_strategy['take_profit']}%, "
          f"trade_size={best_strategy['trade_size']}%")
    print(f"  Encoded       : {encode(best_strategy)}")
    print(f"  Final_profit  : ${final_profit:.4f}")

    # ── PART 2 ─────────────────────────────────────────────────
    print(f"\n{separator}")
    print("  PART 2")
    print(separator)

    p1, p2 = random.sample(INITIAL_POPULATION, 2)
    s1, s2 = encode(p1), encode(p2)

    # Choose two random points ensuring pt2 > pt1
    pt1 = random.randint(1, len(s1) - 2)
    pt2 = random.randint(pt1 + 1, len(s1) - 1)

    child1_str = s1[:pt1] + s2[pt1:pt2] + s1[pt2:]
    child2_str = s2[:pt1] + s1[pt1:pt2] + s2[pt2:]

    child1 = {
        "stop_loss":   float(int(child1_str[0:2])),
        "take_profit": float(int(child1_str[2:4])),
        "trade_size":  float(int(child1_str[4:6])),
    }
    child2 = {
        "stop_loss":   float(int(child2_str[0:2])),
        "take_profit": float(int(child2_str[2:4])),
        "trade_size":  float(int(child2_str[4:6])),
    }

    print(f"\n  Randomly selected parents from initial population:")
    print(f"    Parent 1 : {s1}  (SL={p1['stop_loss']}%, TP={p1['take_profit']}%, TS={p1['trade_size']}%)")
    print(f"    Parent 2 : {s2}  (SL={p2['stop_loss']}%, TP={p2['take_profit']}%, TS={p2['trade_size']}%)")
    print(f"\n  Two-point crossover points chosen randomly:")
    print(f"    Point 1 : between index {pt1-1} and {pt1}")
    print(f"    Point 2 : between index {pt2-1} and {pt2}")

    # Visual diagram
    def annotate(s, p1, p2):
        result = ""
        for i, ch in enumerate(s):
            if i == p1 or i == p2:
                result += "|"
            result += ch
        return result

    print(f"\n  Parent 1 : {annotate(s1, pt1, pt2)}   (segment [{pt1}:{pt2}] swapped)")
    print(f"  Parent 2 : {annotate(s2, pt1, pt2)}")
    print(f"\n  Child 1  : {child1_str}  →  SL={child1['stop_loss']}%, TP={child1['take_profit']}%, TS={child1['trade_size']}%")
    print(f"  Child 2  : {child2_str}  →  SL={child2['stop_loss']}%, TP={child2['take_profit']}%, TS={child2['trade_size']}%")

    print(f"\n  Fitness of Child 1: ${fitness(child1):.4f}")
    print(f"  Fitness of Child 2: ${fitness(child2):.4f}")

    print(f"\n{separator}")
    print("  PART 3")
    print(separator)
    print("""
  Tournament Selection works as follows:
  1. Randomly pick k individuals from the population (tournament size k).
  2. The individual with the HIGHEST fitness among those k wins.
  3. The winner becomes a parent.
  4. Repeat to select the second parent.

  Advantages over random selection:
  ● Selection pressure is tunable via k (larger k = stronger pressure).
  ● Works without sorting the whole population.
  ● Scales well to large populations.
  ● Does NOT require fitness values to be positive or normalised.
    """)

if __name__ == "__main__":
    run_ga()