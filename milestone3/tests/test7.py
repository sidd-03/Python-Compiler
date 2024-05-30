def profit_loss(cost_price: int, selling_price: int, goods: str) -> None:
    if cost_price > selling_price:
        loss: int = cost_price - selling_price
        print(goods)
        print(loss)
    else:
        profit: int = selling_price - cost_price
        print(goods)
        print(profit)

def calc_salary(salary: int, bonus: int, name: str) -> None:
    total: int = salary + bonus
    print(name)
    print(total)

def main():
    s1: str = "Chips"
    print("Profit/Loss")
    profit_loss(1000, 500, s1)
    s2: str = "Chocolate"
    print("Profit/Loss")
    profit_loss(500, 1000, s2)
    s3: str = "John"
    print("Salary")
    calc_salary(5000, 500, s3)

if __name__ == "__main__":
    main()