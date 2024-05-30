def relational_op():
    x: int = 5
    y: int = 3
    print("greater")    
    z: bool = x > y
    print(z)
    print("less")
    z: bool = x < y
    print(z)
    print("greater equal")
    z: bool = x >= y
    print(z)
    print("less equal")
    z: bool = x <= y
    print(z)
    print("equal")
    z: bool = x == y
    print(z)
    print("not equal")
    z: bool = x != y
    print(z)

def logical_op():
    x: bool = True
    y: bool = False
    print("and")
    z: bool = x and y
    print(z)
    print("or")
    z: bool = x or y
    print(z)
    print("not")
    z: bool = not x
    print(z)
    print("not")
    z: bool = not y
    print(z)

def bitwise_op():
    x: int = 5
    y: int = 3
    print("bitwise and")
    z: int = x & y
    print(z)
    print("bitwise or")
    z: int = x | y
    print(z)
    print("bitwise xor")
    z: int = x ^ y
    print(z)
    print("bitwise not")
    z: int = ~x
    print(z)
    print("left shift")
    z: int = x << y
    print(z)
    print("right shift")
    z: int = x >> y
    print(z)

def main():
    print("relational_op output")
    relational_op()
    print("logical_op output")
    logical_op()
    print("bitwise_op output")
    bitwise_op()

if __name__ == "__main__":
    main()