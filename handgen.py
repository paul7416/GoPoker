ranks = "AKQJT98765432"
def nonpair(suffix):
    for i in range(13):
        for j in range(i+1, 13):
            print(f"{ranks[i]}{ranks[j]}{suffix}", end=", ")

nonpair("s")
nonpair("o")
for ch in ranks:
    print(f"{ch}{ch}", end=', ')
