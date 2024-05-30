def if_else():
    x: int = 10
    y: int = 20
    z: int = 0
    if x > y:
        z = x
    elif x < y:
        z = y
    else:
        z = 0
    print(z)

def for_loop():
    x: int = 0
    for i in range(10):
        x += i
    print(x)

def while_loop():
    x: int = 0
    i: int = 0
    while i < 10:
        x += i
        i += 1
    print(x)

def break_stmt():
    x: int = 0
    for i in range(10):
        x += i
        if i == 5:
            break
    print(x)

def continue_stmt():
    x: int = 0
    for i in range(10):
        if i == 5:
            continue
        x += i
    print(x)

def range_for():
    for i in range(0, 10, 2):
        print(i)

def factorial(n: int) -> int:
    if n == 0:
        return 1
    else:
        return n * factorial(n - 1)

def main():
    print("if_else output")
    if_else()
    print("for_loop output")
    for_loop()
    print("while_loop output")
    while_loop()
    print("break_stmt output")
    break_stmt()
    print("continue_stmt output")
    continue_stmt()
    print("factorial output")
    r: int = factorial(5)
    print(r)
    print("range_for output")
    range_for()

if __name__ == "__main__":
    main()
