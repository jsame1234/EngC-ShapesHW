//Created By: Sam Fishman
//EE553: Engineering C++ Section A?
//I pledge my honor that I have abided by the Stevens honor system

#include <iostream>
#include <fstream>
#include <ostream>
#include <cmath>
#include <vector>
using namespace std;

#define PI 3.14159265

class Vertex{
public:
    double x,y,z;

    Vertex(double x, double y, double z) : x(x), y(y), z(z){}
};

void genFacet(Vertex* A, Vertex* B, Vertex* C, ofstream& s, double size=1){
    //A should always be inputted as the reference/ initial point for this to work properly
    double ax = A -> x; double ay = A -> y; double az = A -> z;

    double bx = B -> x; double by = B -> y; double bz = B -> z;

    double cx = C -> x; double cy = C -> y; double cz = C -> z;

    double ABx = bx - ax; double ABy = by - ay; double ABz = bz - az;

    double ACx = cx - ax; double ACy = cy - ay; double ACz = cz - az;

    double fx = (ABy*ACz - ABz*ACy) /(size);
    double fy = (ABz*ACx - ABx*ACz) /(size);
    double fz = (ABx*ACy - ABy*ACx)/(size);

    s << "facet normal " << fx << " " << fy << " " << fz <<'\n' ;
    s << "outer loop\n";
    s << "vertex " << ax << " " << ay << " " << az << '\n';
    s << "vertex " << bx << " " << by << " " << bz << '\n';
    s << "vertex " << cx << " " << cy << " " << cz << '\n';
    s<< "end loop\n";
    s<<"end fact\n";
}


class Shape {
private:
    double x, y, z; //location of shape
public:
    Shape(double x, double y, double z): x(x), y(y), z(z) {}

    double getX()const {return x;}
    double getY()const {return y;}
    double getZ()const {return z;}

    virtual void draw(ofstream& s){cout << "Nothing drawn yet";};
};


class Cylinder : public Shape{
private:
    double r, h;
    int facets;
public:
    Cylinder(double x, double y, double z, double r, double h, int facets) : Shape(x,y,z),
                                                                             r(r), h(h),
                                                                             facets(facets){}
    void draw(ofstream& s) override {

        double x1 = getX();
        double y1 = getY();
        double z1 = getZ();

        double theta = 360/ double(facets);
        vector<Vertex*> vertices;
        vertices.push_back(new Vertex(x1,y1,z1));
        vertices.push_back(new Vertex(x1,y1,(z1+h)));

        for(double i = 0; i <= 360; i+= theta){
            double q = i*PI / 180;
            double x = r*cos(q) + x1;
            double y = r*sin(q) + y1;
            vertices.push_back(new Vertex(x,y,z1));
            vertices.push_back(new Vertex(x,y,(z1+h)));
        }

       int totVertices = facets*2 + 1;
        //facet gen for all triangles
        for(int i=2 ; i <= totVertices+3; i+=2){
            if(i+1 < totVertices) {
                genFacet(vertices[0], vertices[i + 2], vertices[i], s);//bottom triangles
                genFacet(vertices[1], vertices[i + 3], vertices[i + 1], s);//top triangles
                genFacet(vertices[i], vertices[i + 3], vertices[i+1], s);//left side of facet triangle
                genFacet(vertices[i + 3], vertices[i], vertices[i + 2], s);//right side of facet triangle
            }
            else{
                genFacet(vertices[0], vertices[2], vertices[totVertices - 1], s);//bottom triangles
                genFacet(vertices[1], vertices[3], vertices[totVertices], s);//top triangles
                genFacet(vertices[totVertices -1 ], vertices[3], vertices[totVertices], s);//left side of facet triangle
                genFacet(vertices[3], vertices[totVertices - 1], vertices[2], s);//right side of facet triangle
            }
        }

    }

};


class Cube: public Shape {
private:
    double size;
public:
    Cube(double x, double y, double z, double size) : Shape(x, y, z), size(size){}

     void draw(ofstream& s) override{
        //get initial coordinates
        double x1 = getX();
        double y1 = getY();
        double z1 = getZ();

    //first need to determine all vertices in order to create triangle
        /*assume x = x1, y=y1, z=z1 ; size = s, therefore vertices will be
          (x1,y1,z1) , (x1,y1,z1+s) , (x1,y1+s,z1), (x1,y1+s,z1+s), (x1+s,y1,z1), (x1+s,y1,z1+s), (x1+s,y1+s,z1) , (x1+s,y1+s,z1+s)
            000   ,   001   ,    010 ,   011  ,   100  ,   101  ,   110   ,   111
            1         2          3       4        5        6        7         8
        */

        double l = x1, m = y1, n = z1;
        vector<Vertex*> vertices;
        vertices.push_back(new Vertex(l,m,n));
        for(int i = 1; i <9; i++){
            l = x1; m = y1; n = z1;
            if(i%2 != 0)
                n += size;

            if((i>1 && i<4) || i >= 6)
                m += size;

            if(i >= 4)
                l += size;
            vertices.push_back(new Vertex(l,m,n));
        }

        genFacet(vertices[0], vertices[5], vertices[4], s, size);//1
        genFacet(vertices[5], vertices[0], vertices[1], s, size);//2
        genFacet(vertices[5], vertices[6], vertices[4], s, size);//3
        genFacet(vertices[6], vertices[5], vertices[7], s, size);//4
        genFacet(vertices[5], vertices[3], vertices[1], s, size);//5
        genFacet(vertices[3], vertices[5], vertices[7], s, size);//6
        genFacet(vertices[0], vertices[6], vertices[4], s, size);//7**
        genFacet(vertices[6], vertices[0], vertices[2], s, size);//8**
        genFacet(vertices[0], vertices[3], vertices[1], s, size);//9
        genFacet(vertices[3], vertices[0], vertices[2], s, size);//10
        genFacet(vertices[6], vertices[3], vertices[2], s, size);//11
        genFacet(vertices[3], vertices[6], vertices[7], s, size);//12
     }

};


class CAD {
private:
    vector<Shape*> shapes;
public:
    CAD() : shapes(){}

   /*~CAD(){
        for(auto s: shapes){
            delete s;
        }
   }*/

    void add(Shape* c){
        shapes.push_back(c);
    }

    void write(const char* s){
        //open/create file within write
        ofstream STL;
        STL.open(s);
        //initialize necessary file headers
        STL << "solid OpenSCAD_MODEL\n";


        for(int i = 0; i < shapes.size(); i++){
            shapes[i]->draw(STL);
            //send file name into draw function
        }

        STL << "endsolid OpenSCAD_MODEL\n";
        STL.close();

    }

};

//https://www.stratasysdirect.com/resources/how-to-prepare-stl-files/
//https://www.viewstl.com/
int main() {
    CAD c;
    c.add(new Cube(0,0,0,5)); //at position x=0, y=0, z=0, with cube size of 5
    c.add(new Cylinder(100,0,0, 3, 10, 10));//at position x=100,y=0,z=0, with r=3, h=10, and facets=10
    c.write("test.stl"); //write all shapes to one stl file

}