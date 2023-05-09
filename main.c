#include <GL/glut.h>

void display();

int main(int argc, char** argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_DEPTH | GLUT_RGBA);
    glutInitWindowPosition(683, 384);
    glutInitWindowSize(1000, 600);
    glutCreateWindow("Somniac");
    glutDisplayFunc(display);
    glutMainLoop();
} 

void display()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // to clear the buffer from any colors to clarify
    glBegin(GL_POINTS);
        glVertex2f(0, 0);
    glEnd();

    glutSwapBuffers();
// another comment
}
