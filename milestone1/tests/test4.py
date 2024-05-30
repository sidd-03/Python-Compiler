def sum(arr,n):
    sum = 0
    for i in range(n):
        sum = sum + arr[i]
    return(sum)

def largest(arr, n):
    max = arr[0]
    for i in range(1, n):
        if arr[i] > max:
            max = arr[i]
    return max

def reverse_arr(arr,s,e):
    n = e - s + 1
    c = 0
    x=n//2
    while x != c:
      temp = arr[s+c]
      arr[s+c] = arr[e-c]
      arr[e-c] = temp
      c += 1
    return arr

def left_rotate_array(arr, n, d):
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

def main():
    arr : list[int] = [10, -10, 20, 50, -40]
    n = len(arr)
    ans = sum(arr,n)
    print('Sum of the array is:')
    print(ans)
    max = largest(arr, n)
    print('Largest element of the array is:')
    print(max)
    print('Size of the array is')
    print(n)
    print('Original array:')
    print(arr)
    print('Left rotating the array by 2')
    d = 2
    print('Rotated array: ')
    print(left_rotate_array(arr, n, d))

if(__name__ == "__main__"):
    main()