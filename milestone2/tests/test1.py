def fact_for(x: int) -> int:
  result = 1
  for i in range(1, x + 1):
    result *= i
  return result

def fact_while(x: int) -> int:
  result = 1
  while x > 0:
    result *= x
    x -= 1
  return result

def all_lesser_even_numbers(x: int) -> None:
  for i in range(x):
    if i % 2 == 0:
      continue
    print(i)

def all_numbers_less_than_half(x: int) -> None:
  for i in range(x):
    if i > x / 2:
      break
    print(i)

def all_factors(x: int) -> None:
  for i in range(1, x + 1):
    if x % i == 0:
      print(i)

def main():
  print(fact_rec(5))
  print(fact_for(5))
  print(fact_while(5))
  print(all_lesser_even_numbers(5))
  print(all_numbers_less_than_half(5))
  print(all_factors(6))

if __name__ == "__main__":
  main()