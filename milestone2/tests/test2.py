class Vehicle:
    def __init__(self, make, model):
        self.make = make
        self.model = model
    def display_info(self):
        print(self.make)
        print(self.model)

class Car(Vehicle):
    def __init__(self, make, model, year):
        Vehicle.__init__(self, make, model)
        self.year = year
    def display_info(self):
        Vehicle.display_info(self)
        print(self.year)

class ElectricCar(Car):
    def __init__(self, make, model, year, battery_capacity):
        Car.__init__(self, make, model, year)
        self.battery_capacity = battery_capacity
    def display_info(self):
        Car.display_info(self)
        print(self.battery_capacity)

class HybridCar(Car):
    def __init__(self, make, model, year, fuel_capacity):
        Car.__init__(self, make, model, year)
        self.fuel_capacity = fuel_capacity
    def display_info(self):
        Car.display_info(self)
        print(self.fuel_capacity)

def main():
    vehicle = Vehicle("Toyota", "Camry")
    vehicle.display_info()
    car = Car("Ford", "Mustang", 2022)
    car.display_info()
    electric_car = ElectricCar("Tesla", "Model S", 2023, 100)
    electric_car.display_info()
    hybrid_car = HybridCar("Toyota", "Prius", 2023, 10)
    hybrid_car.display_info()

if __name__ == "__main__":
    main()
