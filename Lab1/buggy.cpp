#include <iostream>
// included cstring for memcpy
#include <cstring>

struct Point
{
    int x, y;

    Point() : x(), y() {}
    Point(int _x, int _y) : x(_x), y(_y) {}
};

class Shape
{
    int vertices;
    Point **points;

public:
    // made these functions public so main() can use Shape constructors and methods
    Shape(int _vertices)
    {
        vertices = _vertices;
        points = new Point *[vertices + 1];

        // initialized points with default Point values
        for (int i = 0; i <= vertices; ++i)
            points[i] = new Point();
    }

    ~Shape()
    {
        for (int i = 0; i <= vertices; ++i)
        {
            delete points[i];
            points[i] = nullptr;
        }
        delete[] points;
        points = nullptr;
        vertices = 0;
    }

    // added Point *pts parameter
    void addPoints(Point *pts)
    {
        for (int i = 0; i <= vertices; i++)
        {
            memcpy(points[i], &pts[i % vertices], sizeof(Point));
        }
    }

    // changed return type from double* to double
    double area()
    {
        int temp = 0;
        for (int i = 0; i < vertices; i++)
        {
            // two methods of accessing pointer members:
            // -> operator
            int lhs = points[i]->x * points[i + 1]->y;
            // dereferencing the pointer
            int rhs = (*points[i + 1]).x * (*points[i]).y;
            temp += (lhs - rhs);
        }
        double area = abs(temp) / 2.0;
        return area;
    }
};

int main()
{
    // three methods of creating structs:
    // default constructor
    struct Point tri1;
    // assignment operator
    struct Point tri2 = {1, 2};
    // parameterized constructor
    struct Point tri3(2, 0);

    // adding points to tri
    Point triPts[3] = {tri1, tri2, tri3};
    Shape *tri = new Shape(3);
    // used -> operator to reference addPoints(), rather than .addPoints()
    tri->addPoints(triPts);

    // created using parameterized constructor
    struct Point quad1(0, 0);
    struct Point quad2(0, 2);
    struct Point quad3(2, 2);
    struct Point quad4(2, 0);

    // adding points to quad
    Point quadPts[4] = {quad1, quad2, quad3, quad4};
    Shape *quad = new Shape(4);

    // used -> operator to reference addPoints(), rather than .addPoints()
    quad->addPoints(quadPts);

    // printed out the area() functions for both tri and quad
    std::cout << "Area of tri is: " << tri->area() << std::endl;
    std::cout << "Area of quad is: " << quad->area() << std::endl;

    // deallocate memory and ensure no dangling pointers
    delete quad;
    delete tri;
    quad = nullptr;
    tri = nullptr;
}
