typedef struct {
    char[50] Brand;
    char[2] TractionRating;
    char[2] SpeedRating;
} Wheel;

typedef struct {
    Wheel*[4] Wheels;
    char* Make;
    char* Model;
} Car;

Car* car = (Car*)malloc(sizeof(Car));
car.Make = (char*)malloc(sizeof(char)*50);
car.Model = (char*)malloc(sizeof(char)*50);
car.Wheels[0] = (Wheel*)malloc(sizeof(Wheel));
car.Wheels[1] = (Wheel*)malloc(sizeof(Wheel));
car.Wheels[2] = (Wheel*)malloc(sizeof(Wheel));
car.Wheels[3] = (Wheel*)malloc(sizeof(Wheel));

car.Wheels[1]->Brand
