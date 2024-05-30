def selectionSort(array: list[int], n: int):
     for i in range(n):
        min_index = i
        for j in range(i + 1, n):
            if array[j] < array[min_index]:
                min_index = j
        temp = array[i]
        array[i] = array[min_index]
        array[min_index] = temp

def insertion_sort(arr: list[int], n: int):
  for i in range(1, n):
    key = arr[i]
    j = i - 1
    flag=0
    if(key < arr[j]):
      flag=1
    while j >= 0 and flag:
      arr[j + 1] = arr[j]
      j -= 1
      flag=0
      if(key < arr[j]):
        flag=1
    arr[j + 1] = key 

def do_print(arr , size):
  for i in range(size):
    x = arr[i]
    print(x)

def add(a: int, b: int) -> int:
  return a + b

def main():
  arr : list[int] = [ -8, 10, 6, 12, 0, 11, -3]
  len1 = len(arr)
  print("Array before sorting:")
  do_print(arr , len1)
  selectionSort(arr, len1)
  print("Array after sorting using selection sort:")
  do_print(arr , len1)
  arr2 : list[int] = [ -8, 10, 6, 12, 0, 11, -3]
  len2 = len(arr2)
  insertion_sort(arr2, len2)
  print("Array after sorting using insertion sort:")
  do_print(arr2 , len2)

if __name__ == "__main__":
  main()