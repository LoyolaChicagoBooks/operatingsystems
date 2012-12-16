typedef struct {
    char[50] Brand;
    char[2] TractionRating;
    char[2] SpeedRating;
} Wheel;

typedef struct {
    Wheel[4] Wheels;
    char[50] Make;
    char[50] Model;
} Car;

Car* car = (Car*)malloc(sizeof(Car));
