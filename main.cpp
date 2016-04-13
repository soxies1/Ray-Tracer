#include "Image.h"
#include "raytrace2.h"

using namespace std;
int main(int, char**){

    Image image(512, 512);
    //float starttime = glfwGetTime();
    RayTraceImage(&image);
    //float endtime = glfwGetTime();
    //cout << "Time:" << endtime - starttime << endl;

    image.show("GLFW3+Libpng Image Window Demo");
    //image.save("/Users/Tyson/Desktop/demo.png");
	
}
