def calc_hcf(x: int, y: int) -> int:
    if x > y:
        a = y
    else:
        a = x
    for i in range(1, a + 1):
        p = x % i
        q = y % i
        if p == 0 and q == 0:
            hcf = i
    return hcf

def calc_lcm(x: int, y: int) -> int:
    if x > y:
        greater = x
    else:
        greater = y
    flag = 1
    while flag > 0:
        p = greater % x
        q = greater % y
        if p == 0 and q == 0:
            lcm = greater
            break
        greater += 1
    return lcm

def lesser(x: int, y: int) -> int:
    if x < y:
        return x
    else:
        return y

def greater(x: int, y: int) -> int:
    if x > y:
        return x
    else:
        return y

def main():
    x: int = 30
    y: int = 50
    print("HCF")
    r1: int =  calc_hcf(x, y)
    print("LCM")
    r2: int = calc_lcm(x, y)
    print("Greater")
    g: int = greater(x, y)
    print("Lesser")
    s: int = lesser(x, y)
    print(r1)
    print(r2)
    print(g)
    print(s)

if __name__ == "__main__":
    main()