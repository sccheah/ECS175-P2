/*
 * Simple glut demo that can be used as a template for
 * other projects by Garrett Aldrich
 */


#ifdef WIN32
#include <windows.h>
#endif

#if defined (__APPLE__) || defined(MACOSX)
#include <OpenGL/gl.h>
//#include <OpenGL/glu.h>
#include <GLUT/glut.h>

#else //linux
#include <GL/gl.h>
#include <GL/glut.h>
#endif

//other includes
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <string>
#include <math.h>
#include <list>
#include <vector>

using namespace std;

/****set in main()****/
//the number of pixels in the grid
int grid_width;
int grid_height;

//the size of pixels sets the inital window height and width
//don't make the pixels too large or the screen size will be larger than
//your display size
float pixel_size;

/*Window information*/
int win_height;
int win_width;


void init();
void idle();
void display();
void draw_lines();
void reshape(int width, int height);
void key(unsigned char ch, int x, int y);
void mouse(int button, int state, int x, int y);
void motion(int x, int y);
void check();
void display_all_lines();
void display_xy();
void display_xz();
void display_yz();

class wcPt3D {
    public:
    GLfloat x, y, z;
};

typedef float Matrix4x4 [4][4];
Matrix4x4 matRot;

class Point
{
    double x;
    double y;
    double z;
    
    public:
    void set_x(double new_x) {x = new_x;}
    void set_y(double new_y) {y = new_y;}
    void set_z(double new_z) {z = new_z;}
    double get_x() {return x;}
    double get_y() {return y;}
    double get_z() {return z;}
};


class Edge
{
public:
    int start_pt;
    int end_pt;
};

class Polygon
{
    public:
    int numberOfPoints;
    int numberOfEdges;
    Point *points;
    Edge *edges;
};

Polygon *polygons;
int numberOfPolygons = 0;

void display_menu();
void read_file(string);
void print_polygons();
void translate_polygon();
void rotate_polygon();
void scale_polygon();
int display_polygon_id = 0;
double min_x = 0.0, min_y = 0.0, min_z = 0.0;
double max_x = 0.0, max_y = 0.0, max_z = 0.0;
double view_x = 0.0, view_y = 0.0, view_z = 3.0;

bool is_display_xy = true, is_display_xz = false, is_display_yz = false;
bool is_display_all_polygons = true;
int display_plane_id;

int mainWindow, subWindow1, subWindow2, subWindow3;
string output_file_name;

int main(int argc, char **argv)
{
    string filename;
    cout << "Enter name of data file: ";
    cin >> filename;
    read_file(filename);
    //print_polygons();
    cout << endl;
    
    display_menu();
    
    //the number of pixels in the grid
    grid_width = 200;
    grid_height = 200;
    
    pixel_size = 5;
    
    /*Window information*/
    win_height = grid_height*pixel_size;
    win_width = grid_width*pixel_size;
    
    /*Set up glut functions*/
    /** See https://www.opengl.org/resources/libraries/glut/spec3/spec3.html ***/
    
    glutInit(&argc,argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    /*initialize variables, allocate memory, create buffers, etc. */
    //create window of size (win_width x win_height
    glutInitWindowSize(win_width,win_height);
    //windown title is "glut demo"
    mainWindow = glutCreateWindow("3D Polygons");
        glutDisplayFunc(display);
    
    /*defined glut callback functions*/
    //glutDisplayFunc(display); //rendering calls here
    glutReshapeFunc(reshape); //update GL on window size change
    glutMouseFunc(mouse);     //mouse button events
    glutMotionFunc(motion);   //mouse movement events
    glutKeyboardFunc(key);    //Keyboard events
    glutIdleFunc(idle);       //Function called while program is sitting "idle"
    
    //initialize opengl variables
    init();
    
    subWindow1 = glutCreateSubWindow(mainWindow, 0, 500, 500, 500);
        glutDisplayFunc(display_yz);
        glutReshapeFunc(reshape); //update GL on window size change
        glutMouseFunc(mouse);     //mouse button events
        glutMotionFunc(motion);   //mouse movement events
        glutKeyboardFunc(key);    //Keyboard events
        glutIdleFunc(idle);
        init();

    subWindow2 = glutCreateSubWindow(mainWindow, 0, 0, 500, 500);
        glutDisplayFunc(display_xy);
        glutReshapeFunc(reshape); //update GL on window size change
        glutMouseFunc(mouse);     //mouse button events
        glutMotionFunc(motion);   //mouse movement events
        glutKeyboardFunc(key);    //Keyboard events
        glutIdleFunc(idle);
        init();
    
    subWindow3 = glutCreateSubWindow(mainWindow, 500, 0, 500, 500);
        glutDisplayFunc(display_xz);
        glutReshapeFunc(reshape); //update GL on window size change
        glutMouseFunc(mouse);     //mouse button events
        glutMotionFunc(motion);   //mouse movement events
        glutKeyboardFunc(key);    //Keyboard events
        glutIdleFunc(idle);
        init();
    //start glut event loop
    glutMainLoop();
    return 0;
}

//////////////////////////////////////////////////////

void display_menu()
{
    cout << endl;
    cout << "1. Load New Data File" << endl;
    cout << "2. Show all polygons" << endl;
    cout << "3. Select polygon to be displayed" << endl;
    cout << "4. 3D Translation" << endl;
    cout << "5. 3D Rotation" << endl;
    cout << "6. 3D Scaling" << endl;
    cout << "9. Reset \"gluLookAt\" View" << endl;
    cout << "0. Exit Program" << endl;
    cout << endl;
}

void free_polygons()
{
    for (int i = 0; i < numberOfPolygons; i++)
    {
        delete [] polygons[i].points;
        delete [] polygons[i].edges;
    }
    
    delete [] polygons;
}

void read_file(string input)
{
    ifstream file(input);
    
    while (!file.good())
    {
        cout << "Please enter a valid file: " << endl;
        cin >> input;
        file.open(input);
    }
    
    output_file_name = input;
    
    file >> numberOfPolygons;
    
    polygons = new Polygon [numberOfPolygons];
    
    for (int i = 0; i < numberOfPolygons; i++)
    {
        
        file >> polygons[i].numberOfPoints;
        polygons[i].points = new Point [polygons[i].numberOfPoints];
        
        for (int j = 0; j < polygons[i].numberOfPoints; j++)
        {
            double x, y, z;
            
            file >> x;
            file >> y;
            file >> z;
            polygons[i].points[j].set_x(x);
            polygons[i].points[j].set_y(y);
            polygons[i].points[j].set_z(z);
        }
        
        file >> polygons[i].numberOfEdges;
        polygons[i].edges = new Edge [polygons[i].numberOfEdges];
        
        for (int j = 0; j < polygons[i].numberOfEdges; j++)
        {
            file >> polygons[i].edges[j].start_pt;
            file >> polygons[i].edges[j].end_pt;
        }
    }
    file.close();
}

void print_polygons()
{
    for (int i = 0; i < numberOfPolygons; i++)
    {
        cout << "Polygon " << i << endl;
        for (int j = 0; j < polygons[i].numberOfPoints; j++)
        {
            cout << polygons[i].points[j].get_x() << " " << polygons[i].points[j].get_y() << " " << polygons[i].points[j].get_z() << endl;
        }
        
        for (int j = 0; j < polygons[i].numberOfEdges; j++)
        {
            cout << polygons[i].edges[j].start_pt << " " << polygons[i].edges[j].end_pt << endl;
        }
    }
}

void translate_polygon()
{
    int selectedPolygon = display_polygon_id;
    double tx, ty, tz;
    double px, py, pz;
    
    if (is_display_all_polygons)
    {
        cout << "Please select the polygon you would like to translate [0 - " << numberOfPolygons - 1 << "]: ";
        cin >> selectedPolygon;
        
        while (selectedPolygon < 0 || selectedPolygon > numberOfPolygons - 1)
        {
            cout << "Invalid selection. Please enter [0 - " << numberOfPolygons - 1 << "]: ";
            cin >> selectedPolygon;
        }
    }
    
    cout << "Enter translation vector [x y z]: ";
    cin >> tx >> ty >> tz;
    
    for (int i = 0; i < polygons[selectedPolygon].numberOfPoints; i++)
    {
        px = polygons[selectedPolygon].points[i].get_x();
        py = polygons[selectedPolygon].points[i].get_y();
        pz = polygons[selectedPolygon].points[i].get_z();
        
        polygons[selectedPolygon].points[i].set_x(px + tx);
        polygons[selectedPolygon].points[i].set_y(py + ty);
        polygons[selectedPolygon].points[i].set_z(pz + tz);
    }
    
}

/* Construct the 4 x 4 identity matrix. */
void matrix4x4SetIdentity (Matrix4x4 matIdent4x4)
{
    GLint row, col;
    
    for (row = 0; row < 4; row++)
        for (col = 0; col < 4 ; col++)
            matIdent4x4 [row][col] = (row == col);
}

/* Premultiply matrix m1 by matrix m2, store result in m2. */
void matrix4x4PreMultiply (Matrix4x4 m1, Matrix4x4 m2)
{
    GLint row, col;
    Matrix4x4 matTemp;
    
    for (row = 0; row < 4; row++)
        for (col = 0; col < 4 ; col++)
            matTemp [row][col] = m1 [row][0] * m2 [0][col] + m1 [row][1] *
                                m2 [1][col] + m1 [row][2] * m2 [2][col] +
                                m1 [row][3] * m2 [3][col];
    
    for (row = 0; row < 4; row++)
        for (col = 0; col < 4; col++)
            m2 [row][col] = matTemp [row][col];
}

void translate3D (GLfloat tx, GLfloat ty, GLfloat tz)
{
    Matrix4x4 matTransl3D;
    
    /*  Initialize translation matrix to identity.  */
    matrix4x4SetIdentity (matTransl3D);
    
    matTransl3D [0][3] = tx;
    matTransl3D [1][3] = ty;
    matTransl3D [2][3] = tz;
    
    /*  Concatenate translation matrix with matRot.  */
    matrix4x4PreMultiply (matTransl3D, matRot);
}

void rotate3D (wcPt3D p1, wcPt3D p2, GLfloat radianAngle)
{
    Matrix4x4 matQuaternionRot;
    
    GLfloat axisVectLength = sqrt ((p2.x - p1.x) * (p2.x - p1.x) +
                                   (p2.y - p1.y) * (p2.y - p1.y) +
                                   (p2.z - p1.z) * (p2.z - p1.z));
    
    GLfloat cosA = cos (radianAngle);
    GLfloat oneC = 1 - cosA;
    GLfloat sinA = sin (radianAngle);
    GLfloat ux = (p2.x - p1.x) / axisVectLength;
    GLfloat uy = (p2.y - p1.y) / axisVectLength;
    GLfloat uz = (p2.z - p1.z) / axisVectLength;
    
    /*  Set up translation matrix for moving p1 to origin.  */
    translate3D (-p1.x, -p1.y, -p1.z);
    
    /*  Initialize matQuaternionRot to identity matrix.  */
    matrix4x4SetIdentity (matQuaternionRot);
    
    matQuaternionRot [0][0] = ux*ux*oneC + cosA;
    matQuaternionRot [0][1] = ux*uy*oneC - uz*sinA;
    matQuaternionRot [0][2] = ux*uz*oneC + uy*sinA;
    matQuaternionRot [1][0] = uy*ux*oneC + uz*sinA;
    matQuaternionRot [1][1] = uy*uy*oneC + cosA;
    matQuaternionRot [1][2] = uy*uz*oneC - ux*sinA;
    matQuaternionRot [2][0] = uz*ux*oneC - uy*sinA;
    matQuaternionRot [2][1] = uz*uy*oneC + ux*sinA;
    matQuaternionRot [2][2] = uz*uz*oneC + cosA;
    /*  Combine matQuaternionRot with translation matrix.  */
    matrix4x4PreMultiply (matQuaternionRot, matRot);
    /*  Set up inverse matTransl3D and concatenate with
     *  product of previous two matrices.
     */
    translate3D (p1.x, p1.y, p1.z);
}

void rotate_polygon(void)
{
    int selectedPolygon = display_plane_id;
    wcPt3D pt1, pt2;
    GLfloat angle, pi = 3.1415926535897;
    
    if (is_display_all_polygons)
    {
        cout << "Please select the polygon you would like to rotate [0 - " << numberOfPolygons - 1 << "]: ";
        cin >> selectedPolygon;
        
        while (selectedPolygon < 0 || selectedPolygon > numberOfPolygons - 1)
        {
            cout << "Invalid selection. Please enter [0 - " << numberOfPolygons - 1 << "]: ";
            cin >> selectedPolygon;
        }
    }

    cout << "Enter first rotational axis point separated by spaces [x y z]: ";
    cin >> pt1.x >> pt1.y >> pt1.z;
    
    cout << "Enter second rotational axis point separated by spaces [x y z]: ";
    cin >> pt2.x >> pt2.y >> pt2.z;
    
    cout << "Enter the rotation angle: ";
    cin >> angle;
    
    angle = (angle / 180) * pi;
    
    
    /*  Initialize matRot to identity matrix:  */
    matrix4x4SetIdentity (matRot);
    
    rotate3D(pt1, pt2, angle);
    
    for (int i = 0; i < polygons[selectedPolygon].numberOfPoints; i++)
    {
        double new_x = (double)(polygons[selectedPolygon].points[i].get_x()) * (double)(matRot[0][0])
                       + (double)(polygons[selectedPolygon].points[i].get_y()) * (double)(matRot[0][1])
                       + (double)(polygons[selectedPolygon].points[i].get_z()) * (double)(matRot[0][2])
                       + matRot[0][3];
        
        double new_y = (double)(polygons[selectedPolygon].points[i].get_x()) * (double)(matRot[1][0])
                        + (double)(polygons[selectedPolygon].points[i].get_y()) * (double)(matRot[1][1])
                        + (double)(polygons[selectedPolygon].points[i].get_z()) * (double)(matRot[1][2])
                        + matRot[1][3];
        
        double new_z = (double)(polygons[selectedPolygon].points[i].get_x()) * (double)(matRot[2][0])
                        + (double)(polygons[selectedPolygon].points[i].get_y()) * (double)(matRot[2][1])
                        + (double)(polygons[selectedPolygon].points[i].get_z()) * (double)(matRot[2][2])
                        + matRot[2][3];
        
        polygons[selectedPolygon].points[i].set_x(new_x);
        polygons[selectedPolygon].points[i].set_y(new_y);
        polygons[selectedPolygon].points[i].set_z(new_z);
    }
}

void scale_polygon()
{
    double sx = 0.0, sy = 0.0, sz = 0.0;
    double fixed_x = 0.0, fixed_y = 0.0, fixed_z = 0.0;
    double new_x, new_y, new_z;
    int selectedPolygon = display_polygon_id;
    double scale_factor;
    
    if (is_display_all_polygons)
    {
        cout << "Please select the polygon you would like to scale [0 - " << numberOfPolygons - 1 << "]: ";
        cin >> selectedPolygon;
        
        while (selectedPolygon < 0 || selectedPolygon > numberOfPolygons - 1)
        {
            cout << "Invalid selection. Please enter [0 - " << numberOfPolygons - 1 << "]: ";
            cin >> selectedPolygon;
        }
    }
    
    cout << "Enter scale factor: ";
    cin >> scale_factor;
    
    sx = scale_factor;
    sy = scale_factor;
    sz = scale_factor;
    
    for (int k = 0; k < polygons[selectedPolygon].numberOfPoints; k++)
    {
        fixed_x += polygons[selectedPolygon].points[k].get_x();
        fixed_y += polygons[selectedPolygon].points[k].get_y();
        fixed_z += polygons[selectedPolygon].points[k].get_z();
    }
    
    fixed_x = fixed_x / polygons[selectedPolygon].numberOfPoints;
    fixed_y = fixed_y / polygons[selectedPolygon].numberOfPoints;
    fixed_z = fixed_z / polygons[selectedPolygon].numberOfPoints;
    
    for (int k = 0; k < polygons[selectedPolygon].numberOfPoints; k++)
    {
        new_x = polygons[selectedPolygon].points[k].get_x() * sx + fixed_x * (1 - sx);
        new_y = polygons[selectedPolygon].points[k].get_y() * sy + fixed_y * (1 - sy);
        new_z = polygons[selectedPolygon].points[k].get_z() * sz + fixed_z * (1 - sz);
        
        polygons[selectedPolygon].points[k].set_x(new_x);
        polygons[selectedPolygon].points[k].set_y(new_y);
        polygons[selectedPolygon].points[k].set_z(new_z);
    }
}

void display_xy()
{
    //clears the screen
    glClear(GL_DEPTH_BUFFER_BIT|GL_COLOR_BUFFER_BIT);
    //clears the opengl Modelview transformation matrix
    
    glLoadIdentity();
    gluLookAt(view_x, view_y, view_z, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
    
    glLineWidth(2.0); //sets the "width" of each line we are rendering
    
    glBegin(GL_LINES);
    
    glColor3f(0, 0, 1.0);
    
    if (is_display_all_polygons)
    {
        for (int i = 0; i < numberOfPolygons; i++)
        {
            for (int j = 0; j < polygons[i].numberOfEdges; j++)
            {
                double pt1_x, pt1_y, pt1_z;
                double pt2_x, pt2_y, pt2_z;
                
                pt1_x = polygons[i].points[polygons[i].edges[j].start_pt - 1].get_x();
                pt1_y = polygons[i].points[polygons[i].edges[j].start_pt - 1].get_y();
                pt1_z = polygons[i].points[polygons[i].edges[j].start_pt - 1].get_z();
                
                pt2_x = polygons[i].points[polygons[i].edges[j].end_pt - 1].get_x();
                pt2_y = polygons[i].points[polygons[i].edges[j].end_pt - 1].get_y();
                pt2_z = polygons[i].points[polygons[i].edges[j].end_pt - 1].get_z();
                
                glVertex3f(pt1_x, pt1_y, pt1_z);
                glVertex3f(pt2_x, pt2_y, pt2_z);
            }
        }
    }
    else
    {
        int i = display_polygon_id;
        for (int j = 0; j < polygons[i].numberOfEdges; j++)
        {
            double pt1_x, pt1_y, pt1_z;
            double pt2_x, pt2_y, pt2_z;
            
            pt1_x = polygons[i].points[polygons[i].edges[j].start_pt - 1].get_x();
            pt1_y = polygons[i].points[polygons[i].edges[j].start_pt - 1].get_y();
            pt1_z = polygons[i].points[polygons[i].edges[j].start_pt - 1].get_z();
            
            pt2_x = polygons[i].points[polygons[i].edges[j].end_pt - 1].get_x();
            pt2_y = polygons[i].points[polygons[i].edges[j].end_pt - 1].get_y();
            pt2_z = polygons[i].points[polygons[i].edges[j].end_pt - 1].get_z();
            
            glVertex3f(pt1_x, pt1_y, pt1_z);
            glVertex3f(pt2_x, pt2_y, pt2_z);
        }
    }
    
    glEnd();
    
    
    //blits the current opengl framebuffer on the screen
    glutSwapBuffers();
    //checks for opengl errors
    check();
    glutPostRedisplay();
}

void display_xz()
{
    //clears the screen
    glClear(GL_DEPTH_BUFFER_BIT|GL_COLOR_BUFFER_BIT);
    //clears the opengl Modelview transformation matrix
    
    glLoadIdentity();
    gluLookAt(view_x, view_y, view_z, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
    
    glLineWidth(2.0); //sets the "width" of each line we are rendering
    
    glBegin(GL_LINES);
    
    glColor3f(0, 0, 1.0);
    
    if (is_display_all_polygons)
    {
        for (int i = 0; i < numberOfPolygons; i++)
        {
            for (int j = 0; j < polygons[i].numberOfEdges; j++)
            {
                double pt1_x, pt1_y, pt1_z;
                double pt2_x, pt2_y, pt2_z;
                
                pt1_x = polygons[i].points[polygons[i].edges[j].start_pt - 1].get_x();
                pt1_y = polygons[i].points[polygons[i].edges[j].start_pt - 1].get_y();
                pt1_z = polygons[i].points[polygons[i].edges[j].start_pt - 1].get_z();
                
                pt2_x = polygons[i].points[polygons[i].edges[j].end_pt - 1].get_x();
                pt2_y = polygons[i].points[polygons[i].edges[j].end_pt - 1].get_y();
                pt2_z = polygons[i].points[polygons[i].edges[j].end_pt - 1].get_z();
                
                glVertex3f(pt1_x, pt1_z, pt1_y);
                glVertex3f(pt2_x, pt2_z, pt2_y);
            }
        }
    }
    else
    {
        int i = display_polygon_id;
        for (int j = 0; j < polygons[i].numberOfEdges; j++)
        {
            double pt1_x, pt1_y, pt1_z;
            double pt2_x, pt2_y, pt2_z;
            
            pt1_x = polygons[i].points[polygons[i].edges[j].start_pt - 1].get_x();
            pt1_y = polygons[i].points[polygons[i].edges[j].start_pt - 1].get_y();
            pt1_z = polygons[i].points[polygons[i].edges[j].start_pt - 1].get_z();
            
            pt2_x = polygons[i].points[polygons[i].edges[j].end_pt - 1].get_x();
            pt2_y = polygons[i].points[polygons[i].edges[j].end_pt - 1].get_y();
            pt2_z = polygons[i].points[polygons[i].edges[j].end_pt - 1].get_z();
            
            glVertex3f(pt1_x, pt1_z, pt1_y);
            glVertex3f(pt2_x, pt2_z, pt2_y);
        }
    }
    
    glEnd();
    
    
    //blits the current opengl framebuffer on the screen
    glutSwapBuffers();
    //checks for opengl errors
    check();
    glutPostRedisplay();
}

void display_yz()
{
    //clears the screen
    glClear(GL_DEPTH_BUFFER_BIT|GL_COLOR_BUFFER_BIT);
    //clears the opengl Modelview transformation matrix
    
    glLoadIdentity();
    gluLookAt(view_x, view_y, view_z, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
    
    glLineWidth(2.0); //sets the "width" of each line we are rendering
    
    glBegin(GL_LINES);
    
    glColor3f(0, 0, 1.0);
    
    if (is_display_all_polygons)
    {
        for (int i = 0; i < numberOfPolygons; i++)
        {
            for (int j = 0; j < polygons[i].numberOfEdges; j++)
            {
                double pt1_x, pt1_y, pt1_z;
                double pt2_x, pt2_y, pt2_z;
                
                pt1_x = polygons[i].points[polygons[i].edges[j].start_pt - 1].get_x();
                pt1_y = polygons[i].points[polygons[i].edges[j].start_pt - 1].get_y();
                pt1_z = polygons[i].points[polygons[i].edges[j].start_pt - 1].get_z();
                
                pt2_x = polygons[i].points[polygons[i].edges[j].end_pt - 1].get_x();
                pt2_y = polygons[i].points[polygons[i].edges[j].end_pt - 1].get_y();
                pt2_z = polygons[i].points[polygons[i].edges[j].end_pt - 1].get_z();
                
                glVertex3f(pt1_y, pt1_z, pt1_x);
                glVertex3f(pt2_y, pt2_z, pt2_x);
            }
        }
    }
    else
    {
        int i = display_polygon_id;
        for (int j = 0; j < polygons[i].numberOfEdges; j++)
        {
            double pt1_x, pt1_y, pt1_z;
            double pt2_x, pt2_y, pt2_z;
            
            pt1_x = polygons[i].points[polygons[i].edges[j].start_pt - 1].get_x();
            pt1_y = polygons[i].points[polygons[i].edges[j].start_pt - 1].get_y();
            pt1_z = polygons[i].points[polygons[i].edges[j].start_pt - 1].get_z();
            
            pt2_x = polygons[i].points[polygons[i].edges[j].end_pt - 1].get_x();
            pt2_y = polygons[i].points[polygons[i].edges[j].end_pt - 1].get_y();
            pt2_z = polygons[i].points[polygons[i].edges[j].end_pt - 1].get_z();
            
            glVertex3f(pt1_y, pt1_z, pt1_x);
            glVertex3f(pt2_y, pt2_z, pt2_x);
        }
    }
    
    glEnd();
    
    
    //blits the current opengl framebuffer on the screen
    glutSwapBuffers();
    //checks for opengl errors
    check();
    glutPostRedisplay();
}

void output_to_file()
{
    fstream file;
    file.open(output_file_name, fstream::out);
    //file.open("output.txt", fstream::out);
    file << numberOfPolygons;
    file << endl;
    
    for (int i = 0; i < numberOfPolygons; i++)
    {
        file << endl;
        file << polygons[i].numberOfPoints;
        
        for (int j = 0; j < polygons[i].numberOfPoints; j++)
        {
            file << endl;
            file << polygons[i].points[j].get_x() << " " << polygons[i].points[j].get_y() << " " << polygons[i].points[j].get_z();
        }
        
        file << endl;
        file << polygons[i].numberOfEdges;
        
        for (int j = 0; j < polygons[i].numberOfEdges; j++)
        {
            file << endl;
            file << polygons[i].edges[j].start_pt << " " << polygons[i].edges[j].end_pt;
        }
        file << endl;
    }
    file.close();
}
 
//////////////////////////////////////////////////////


/*initialize gl stufff*/
void init()
{
    //set clear color (Default background to white)
    glClearColor(1.0,1.0,1.0,1.0);
    //checks for OpenGL errors
    check();
}

//called repeatedly when glut isn't doing anything else
void idle()
{
    //redraw the scene over and over again
    glutPostRedisplay();
}


//this is where we render the screen
void display()
{
    //clears the screen
    glClear(GL_DEPTH_BUFFER_BIT|GL_COLOR_BUFFER_BIT);
    //clears the opengl Modelview transformation matrix
    
    glLoadIdentity();
    //gluLookAt(view_x, view_y, view_z, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
    
    //blits the current opengl framebuffer on the screen
    glutSwapBuffers();
    //checks for opengl errors
    check();
}

/*Gets called when display size changes, including initial craetion of the display*/
/*this needs to be fixed so that the aspect ratio of the screen is consistant with the orthographic projection*/

void reshape(int width, int height)
{
    /*set up projection matrix to define the view port*/
    //update the ne window width and height
    win_width = width;
    win_height = height;
    
    //creates a rendering area across the window
    glViewport(0,0,width,height);
    
    // uses an orthogonal projection matrix so that
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    
    //you guys should look up this function
    glOrtho(0,1.0,0.0,1.0,-10,10);
    
    //clear the modelview matrix
    //the ModelView Matrix can be used in this project, to change the view on the projection
    //but you can also leave it alone and deal with changing the projection to a different view
    //for project 2, do not use the modelview matrix to transform the actual geometry, as you won't
    //be able to save the results
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    
    //check for opengl errors
    check();
}

//gets called when a key is pressed on the keyboard
void key(unsigned char ch, int x, int y)
{
    switch(ch)
    {
        case '1':
        {
            if (polygons != NULL)
                free_polygons();
            
            string filename;
            cout << "Enter name of data file: ";
            cin >> filename;
            read_file(filename);
            print_polygons();
            cout << endl;
            display_menu();
            
            break;
        }
        case '2':
        {
            cout << "Showing all polygons" << endl;
            is_display_all_polygons = true;
            break;
        }
        case '3':
        {
            cout << "Enter the ID of the polygon vertex coordinates to be shown[0 - " << numberOfPolygons - 1 << "]: ";
            cin >> display_polygon_id;
            
            is_display_all_polygons = false;
            display_menu();
            break;
        }
        case '4':
        {
            translate_polygon();
            display_menu();
            break;
        }
        case '5':
        {
            rotate_polygon();
            display_menu();
            break;
        }
        case '6':
        {
            scale_polygon();
            display_menu();
            break;
        }
        case '9':
        {
            cout << "Resetting LookAt View..." << endl;
            view_x = 0.0;
            view_y = 0.0;
            view_z = 3.0;
            
            display_menu();
            break;
        }
        case '0':
        {
            cout << "Exitting program and outputting data into " << output_file_name << endl;
            output_to_file();
            exit(0);
            break;
        }
        case 'd':
        {
            view_x += 0.1;
            break;
        }
        case 'a':
        {
            view_x -= 0.1;
            break;
        }
        case 'w':
        {
            view_y += 0.1;
            break;
        }
        case 's':
        {
            view_y -= 0.1;
            break;
        }
        case 'q':
        {
            view_z -= 0.1;
            break;
        }
        case 'e':
        {
            view_z += 0.1;
            break;
        }
            
        default:
        //prints out which key the user hit
        printf("User hit the \"%c\" key\n",ch);
        break;
    }
    //redraw the scene after keyboard input
    glutPostRedisplay();
}


//gets called when a mouse button is pressed
void mouse(int button, int state, int x, int y)
{
    //print the pixel location, and the grid location
    printf ("MOUSE AT PIXEL: %d %d, GRID: %d %d\n",x,y,(int)(x/pixel_size),(int)((win_height-y)/pixel_size));
    switch(button)
    {
        case GLUT_LEFT_BUTTON: //left button
        printf("LEFT ");
        break;
        case GLUT_RIGHT_BUTTON: //right button
        printf("RIGHT ");
        default:
        printf("UNKNOWN "); //any other mouse button
        break;
    }
    if(state !=GLUT_DOWN)  //button released
    printf("BUTTON UP\n");
    else
    printf("BUTTON DOWN\n");  //button clicked
    
    //redraw the scene after mouse click
    glutPostRedisplay();
}

//gets called when the curser moves accross the scene
void motion(int x, int y)
{
    //redraw the scene after mouse movement
    glutPostRedisplay();
}

//checks for any opengl errors in the previous calls and
//outputs if they are present
void check()
{
    GLenum err = glGetError();
    if(err != GL_NO_ERROR)
    {
        printf("GLERROR: There was an error %s\n",gluErrorString(err) );
        exit(1);
    }
}
