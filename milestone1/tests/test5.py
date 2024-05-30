def calc_hcf(x, y):
    if x > y:
        a = y
    else:
        a = x
    for i in range(1, a+1):
        p=x%i
        q=y%i
        if (p == 0 and q == 0 ):
            hcf = i 
    return hcf

def calc_lcm(x, y):
    if x > y:
        greater = x
    else:
        greater = y
    flag=1
    while (flag>0):
        p=greater%x
        q=greater%y
        if (p == 0 and q == 0):
            lcm = greater
            break
        greater += 1
    return lcm

def main():
    n1 = 50 
    n2 = 20
    print("The H.C.F. is")
    print(calc_hcf(n1, n2))
    print("The L.C.M. is")
    print(calc_lcm(n1, n2))

if __name__ == "__main__":
    main()