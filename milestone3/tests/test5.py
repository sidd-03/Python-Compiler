def sum(arr: list[int], n: int):
    sum: int = 0
    for i in range(n):
        sum = sum + arr[i]
    return(sum)

def largest(arr: list[int], n: int):
    j: int=0
    max: int = arr[j]
    for i in range(1, n):
        if arr[i] > max:
            max = arr[i]
    return max

def floor_divide(dividend: int, divisor: int) -> int:
    negated = 0
    if dividend < 0:
        dividend = dividend*(-1)
        negated = 1
    if divisor < 0:
        divisor = divisor*(-1)
        negated = 1

    quotient = 0

    while dividend >= divisor:
        dividend -= divisor
        quotient += 1

    if negated:
        quotient = quotient*(-1)

    return quotient

def reverse_arr(arr: list[int], s: int, e: int):
    n: int = e - s + 1
    c: int = 0
    x: int = floor_divide(n, 2)
    while x != c:
      temp = arr[s+c]
      arr[s+c] = arr[e-c]
      arr[e-c] = temp
      c += 1
    return arr

def left_rotate_array(arr: list[int], n: int, d: int):
    s = 0
    e = n - 1
    arr = reverse_arr(arr, s, e)
    s = 0
    e = n - d - 1
    arr = reverse_arr(arr, s, e)
    s = n - d
    e = n - 1
    arr = reverse_arr(arr, s, e)
    return arr

def do_print(arr: list[int], size: int):
    for i in range(size):
        x=arr[i]
        print(x)

def main():
    arr : list[int] = [10, -10, 20, 50, -40]
    n = len(arr)
    x = arr[1]
    print("First element")
    print(x)
    ans = sum(arr,n)
    print("Sum")
    print(ans)
    max = largest(arr, n)
    print("Largest element")
    print(max)
    print("Array before rotation")
    print(n)
    do_print(arr, n)
    d = 2
    print("Array after rotation")
    arr = left_rotate_array(arr, n, d)
    do_print(arr, n)

if(__name__ == "__main__"):
    main()