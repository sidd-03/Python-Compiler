class Vehicle:
    def __init__(self, make: int, model: int):
        self.make = make
        self.model = model
    def display_info_Vehicle(self):
        print(self.make)
        print(self.model)

class Car(Vehicle):
    def __init__(self, make: int, model: int, year: int):
        self.make = make
        self.model = model
        self.year = year
    def display_info_Car(self):
        print(self.make)
        print(self.model)
        print(self.year)

class ElectricCar(Car):
    def __init__(self, make: int, model: int, year: int, battery_capacity: int):
        self.make = make
        self.model = model
        self.year = year
        self.battery_capacity = battery_capacity
    def display_info_ElectricCar(self):
        print(self.make)
        print(self.model)
        print(self.year)
        print(self.battery_capacity)

class HybridCar(Car):
    def __init__(self, make: int, model: int, year: int, fuel_capacity: int):
        self.make = make
        self.model = model
        self.year = year
        self.fuel_capacity = fuel_capacity
    def display_info_HybridCar(self):
        print(self.make)
        print(self.model)
        print(self.year)
        print(self.fuel_capacity)

def main():
    vehicle : Vehicle = Vehicle(400, 300)
    vehicle.display_info_Vehicle()
    car : Car = Car(500,100,10)  # Updated to pass year as a string
    car.display_info_Car()
    electric_car : ElectricCar = ElectricCar(200,400,800,900)  # Updated to pass year and battery_capacity as strings
    electric_car.display_info_ElectricCar()
    hybrid_car : HybridCar = HybridCar(1,2,3,4)  # Updated to pass year and fuel_capacity as strings
    hybrid_car.display_info_HybridCar()

if __name__ == "__main__":
    main()
