def selectionSort(array, n):
     for i in range(n):
        min_index = i
        for j in range(i + 1, n):
            if array[j] < array[min_index]:
                min_index = j
        temp = array[i]
        array[i] = array[min_index]
        array[min_index] = temp

def insertion_sort(arr, n):
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

def main():
  arr : list[int] = [ -8, 10, 6, 12, 0, 11, -3, 4]
  n = len(arr)
  selectionSort(arr, n)
  print('Sorted Array in Ascending Order using Selction Sort is :')
  print(arr)
  arr : list[int] = [ -8, 10, 6, 12, 0, 11, -3, 4]
  insertion_sort(arr, n)
  print('Sorted Array in Ascending Order using Insertion Sort is :')
  print(arr)

if __name__ == "__main__":
  main()