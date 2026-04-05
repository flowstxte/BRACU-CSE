import random

def profile(motifs):
    k=len(motifs[0])
    t=len(motifs)
    profile={'A':[1]*k,'C':[1]*k,'G':[1]*k,'T':[1]*k}

    for i in range(t):
        for j in range(k):
            nucleotide=motifs[i][j]
            profile[nucleotide][j]+=1
    
    for neucleotide in profile:
        for j in range(k):
            profile[neucleotide][j]/=t+4
    return profile

def probability(kmer,profile):
    p=1
    for i in range(len(kmer)):
        p*=profile[kmer[i]][i]
    return p

def motifs_profile(profile,dna,k):
    best_motifs=[]
    for i in dna:
        max=-1
        best_kmer=''
        for j in range(len(i)-k+1):
            kmer=i[j:j+k]
            p=probability(kmer,profile)
            if p>max:
                max=p
                best_kmer=kmer
        best_motifs.append(best_kmer)
    return best_motifs

def m_score(motifs):
    if not motifs:
        return 0
    k=len(motifs[0])
    t=len(motifs)
    score=0
    for i in range(k):
        counts={'A':0,'C':0,'G':0,'T':0}
        for j in range(t):
            counts[motifs[j][i]]+=1
        
        mc=0
        for nucleotide in counts:
            if counts[nucleotide]>mc:
                mc=counts[nucleotide]
        
        score+=(t-mc)
    return score

def random_motifs(dna,k,t):
    motifs=[]
    for i in dna:
        s=random.randint(0,len(i)-k)
        motifs.append(i[s:s+k])
    
    best_motifs=list(motifs)

    while True:
        p=profile(motifs)
        motifs=motifs_profile(p,dna,k)
        if m_score(motifs)<m_score(best_motifs):
            best_motifs=list(motifs)
        else:
            return best_motifs

def run_multiple(dna,k,t,n=1000):
    overall=None
    min_score=float('inf')
    for i in range(n):
        motifs=random_motifs(dna,k,t)
        sc=m_score(motifs)
        if sc<min_score:
            min_score=sc
            overall=list(motifs)
        
    return overall

def solve(inp,out):
    try:
        with open(inp,'r') as file:
            l=file.readlines()
            k,t=map(int,l[0].strip().split())
            dna=[i.strip() for i in l[1:] if i.strip()]
        
        motifs=run_multiple(dna,k,t)

        with open(out,'w') as output:
            for i in motifs:
                output.write(i+'\n')
    except FileNotFoundError:
        print('File not found')
    except Exception as e:
        print(e)

inp='D:\\CSE443_Assignments\\Assignment 01\\sample1.txt'
out='D:\\CSE443_Assignments\\Assignment 01\\output1.txt'
solve(inp,out)