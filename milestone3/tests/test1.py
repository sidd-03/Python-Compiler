def arith_op():
    x: int = 5
    y: int = 3
    print("plus")
    z: int = x + y
    print(z)
    print("minus")
    z: int = x - y
    print(z)
    print("multiply")
    z: int = x * y
    print(z)
    print("mod")
    z: int = x % y
    print(z)
    print("floordiv")
    z: int = x // y
    print(z)

def assignment_op():
    x: int = 5
    y: int = 3
    z: int = 0
    print("equal")
    z = x
    print(z)
    print("plus equal")
    z += y
    print(z)
    print("minus equal")
    z -= y
    print(z)
    print("multiply equal")
    z *= y
    print(z)
    print("and equal")
    z &= y
    print(z)
    print("or equal")
    z |= y
    print(z)
    print("xor equal")
    z ^= y
    print(z)
    print("left shift equal")
    z <<= y
    print(z)
    print("right shift equal")
    z >>= y
    print(z)
    print("mod equal")
    z %= y
    print(z)

def main():
    print("arith_op output")
    arith_op()
    print("assignment_op output")
    assignment_op()

if __name__ == "__main__":
    main()